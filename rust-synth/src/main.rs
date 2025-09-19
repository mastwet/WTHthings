extern crate ffmpeg_next as ffmpeg;

use cpal::{Sample, SampleFormat};
use ffmpeg::format::sample::Type as SampleType;
use ffmpeg::format::{Sample as FFmpegSample, input};
use ffmpeg::frame;
use ffmpeg::media::Type as MediaType;
use ffmpeg::software::resampling::{context::Context as ResamplingContext};
use cpal::traits::{DeviceTrait, HostTrait, StreamTrait};
use ringbuf::RingBuffer;

//采样率转换
trait SampleFormatConversion {
    fn as_ffmpeg_sample(&self) -> FFmpegSample;
}

impl SampleFormatConversion for SampleFormat {
    fn as_ffmpeg_sample(&self) -> FFmpegSample {
        match self {
            Self::I16 => FFmpegSample::I16(SampleType::Packed),
            Self::U16 => {
                panic!("ffmpeg resampler doesn't support u16")
            }, 
            Self::F32 => FFmpegSample::F32(SampleType::Packed)
        }
    }
}

//
fn write_audio<T: Sample>(data: &mut [T], samples: &mut ringbuf::Consumer<T>, _: &cpal::OutputCallbackInfo) {
    for d in data {
        // copy as many samples as we have.
        // if we run out, write silence
        match samples.pop() {
            Some(sample) => *d = sample,
            None => *d = Sample::from(&0.0)
        }
    }
}

fn init_cpal() -> (cpal::Device, cpal::SupportedStreamConfig) {
    let device = cpal::default_host()
        .default_output_device()
        .expect("no output device available");

    // Create an output stream for the audio so we can play it
    // NOTE: If system doesn't support the file's sample rate, the program will panic when we try to play,
    //       so we'll need to resample the audio to a supported config
    let supported_config_range = device.supported_output_configs()
        .expect("error querying audio output configs")
        .next()
        .expect("no supported audio config found");

    // Pick the best (highest) sample rate
    (device, supported_config_range.with_max_sample_rate())
}

// Interpret the audio frame's data as packed (alternating channels, 12121212, as opposed to planar 11112222)
pub fn packed<T: frame::audio::Sample>(frame: &frame::Audio) -> &[T] {
    if !frame.is_packed() {
        panic!("data is not packed");
    }

    if !<T as frame::audio::Sample>::is_valid(frame.format(), frame.channels()) {
        panic!("unsupported type");
    }

    unsafe { std::slice::from_raw_parts((*frame.as_ptr()).data[0] as *const T, frame.samples() * frame.channels() as usize) }
}

fn main() -> Result<(), ffmpeg::Error> {
    ffmpeg::init().unwrap();

    let file = &std::env::args().nth(1).expect("Cannot open file.");

    //初始化cpal
    let (device, stream_config) = init_cpal();

    // 打开文件
    let mut ictx = input(&file)?;

    // Find the audio stream and its index
    let audio = ictx
        .streams()
        .best(MediaType::Audio)
        .ok_or(ffmpeg::Error::StreamNotFound)?;
    let audio_stream_index = audio.index();

    // 添加解码器
    let mut audio_decoder = audio.codec().decoder().audio()?;

    // 设置音频重采样器
    let mut resampler = ResamplingContext::get(
        audio_decoder.format(),
        audio_decoder.channel_layout(),
        audio_decoder.rate(),
        
        stream_config.sample_format().as_ffmpeg_sample(),
        audio_decoder.channel_layout(),
        stream_config.sample_rate().0
    )?;

    // 使用环形缓冲区放置音频采样
    let buffer = RingBuffer::<f32>::new(8192);
    let (mut producer, mut consumer) = buffer.split();
    
    // 根据样本格式设置音频输出流
    let audio_stream = match stream_config.sample_format() {
        SampleFormat::F32 => device.build_output_stream(&stream_config.into(), move |data: &mut [f32], cbinfo| {
            // 将数据复制到音频缓冲区（如果样本不足，write_audio 会写入静音）
            write_audio(data, &mut consumer, &cbinfo)
        }, |err| {
            eprintln!("error occurred on the audio output stream: {}", err)
        }),
        SampleFormat::I16 => panic!("i16 output format unimplemented"),
        SampleFormat::U16 => panic!("u16 output format unimplemented")
    }.unwrap();

    // 定义一个闭包来接收解码后的音频帧并将其排队以供播放
    let mut receive_and_queue_audio_frames =
        |decoder: &mut ffmpeg::decoder::Audio| -> Result<(), ffmpeg::Error> {
            let mut decoded = frame::Audio::empty();

            // 请求解码器获取音频帧
            while decoder.receive_frame(&mut decoded).is_ok() {
                // 对解码后的音频帧进行重采样
                let mut resampled = frame::Audio::empty();
                resampler.run(&decoded, &mut resampled)?;

            // 不要直接使用 resampled.data(0).len() —— 它可能没有完全填充
            // 根据样本数量、每个样本的字节数和通道数获取正确的字节数。
                let both_channels = packed(&resampled);

            // 如果缓冲区空间不足以容纳所有样本，则等待
            // （生产者可以接受部分写入，但我们不希望这样）
                while producer.remaining() < both_channels.len() {
                    std::thread::sleep(std::time::Duration::from_millis(10));
                }

            // 将样本缓冲到队列中以供播放
                producer.push_slice(both_channels);
            }
            Ok(())
        };

    //开始播放
    audio_stream.play().unwrap();

    // 主循环!
    for (stream, packet) in ictx.packets() {
        // 查找音频包（忽略视频和其他类型的包）
        if stream.index() == audio_stream_index {
        // 将包发送给解码器；解码器会将它们组合成帧。
        // 实际上，通常情况下1个包等于1个帧。
            audio_decoder.send_packet(&packet)?;

        // 将音频排队以供播放（如果队列已满则阻塞）
            receive_and_queue_audio_frames(&mut audio_decoder)?;
        }
    }

    Ok(())
}