use cpal::traits::{DeviceTrait, HostTrait, StreamTrait};
use std::f32::consts::PI;
use std::sync::{Arc, Mutex};
use std::io::{stdin, stdout, Write};
use std::thread;
use std::time::Duration;
use cpal::SampleFormat;

fn main() {
    // 获取主机和输出设备
    let host = cpal::default_host();
    let device = host.default_output_device().expect("未找到输出设备");
    let config = device.default_output_config().expect("未找到默认输出配置");

    // 定义正弦波参数
    let sample_rate = config.sample_rate().0 as f32;
    let initial_frequency = 440.0; // A4 音符，频率为 440Hz
    let amplitude = 0.5; // 振幅范围为 0.0 到 1.0
    let mut sample_clock = 0f32;

    // 使用互斥锁来安全共享音频数据
    let sample_clock = Arc::new(Mutex::new(sample_clock));
    let frequency = Arc::new(Mutex::new(initial_frequency));

    // 创建输出流
    let stream = match config.sample_format() {
        cpal::SampleFormat::F32 => create_stream::<f32>(&device, &config.into(), sample_rate, amplitude, Arc::clone(&sample_clock), Arc::clone(&frequency)),
        cpal::SampleFormat::I16 => create_stream::<i16>(&device, &config.into(), sample_rate, amplitude, Arc::clone(&sample_clock), Arc::clone(&frequency)),
        cpal::SampleFormat::U16 => create_stream::<u16>(&device, &config.into(), sample_rate, amplitude, Arc::clone(&sample_clock), Arc::clone(&frequency)),
        _ => panic!("不支持的音频格式"),
    };

    // 开始播放
    stream.play().expect("播放流失败");

    // 创建一个线程来监听用户输入并调整频率
    let frequency_clone = Arc::clone(&frequency);
    thread::spawn(move || {
        loop {
            println!("请输入新的频率 (Hz)，按 Enter 键确认...");
            let mut input = String::new();
            stdin().read_line(&mut input).unwrap();

            match input.trim().parse::<f32>() {
                Ok(new_freq) if new_freq > 0.0 => {
                    let mut freq = frequency_clone.lock().unwrap();
                    *freq = new_freq;
                    println!("新的频率设置为: {} Hz", *freq);
                }
                _ => {
                    println!("无效输入，请输入一个正数作为频率。");
                }
            }
        }
    });

    // 持续播放，不再需要等待用户按键退出
    loop {
        thread::sleep(Duration::from_secs(1));
    }
}

fn create_stream<T>(
    device: &cpal::Device,
    config: &cpal::StreamConfig,
    sample_rate: f32,
    amplitude: f32,
    sample_clock: Arc<Mutex<f32>>,
    frequency: Arc<Mutex<f32>>,
) -> cpal::Stream
where
    T: cpal::Sample,
{
    let channels = config.channels as usize;

    device.build_output_stream(config, move |data: &mut [T], _: &cpal::OutputCallbackInfo| {
        // 生成正弦波
        let mut sample_clock = sample_clock.lock().unwrap();
        let current_frequency = *frequency.lock().unwrap();
        for frame in data.chunks_mut(channels) {
            let value = (2.0 * PI * current_frequency * *sample_clock / sample_rate).sin() * amplitude;
            *sample_clock += 1.0;
            let sample: T = cpal::Sample::from(&value);
            for sample_out in frame {
                *sample_out = sample;
            }
        }
    }, move |err| {
        eprintln!("播放出错: {}", err);
    }, None).expect("创建输出流失败")
}

