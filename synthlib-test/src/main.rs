use anyhow;
use cpal;
use cpal::traits::{DeviceTrait, HostTrait, StreamTrait};
use std::sync::{Arc, Mutex};
use std::thread;
use std::time::Duration;

mod synthesizer;
use synthesizer::{Synthesizer, SynthParams, Waveform};

fn main() -> Result<(), anyhow::Error> {
    println!("Starting audio synthesis...");

    let host = cpal::default_host();
    let device = host
        .default_output_device()
        .expect("failed to find a default output device");

    // 获取支持的配置
    let supported_configs_range = device.supported_output_configs()?;
    let supported_config = supported_configs_range
        .filter(|config| config.channels() == 2) // 确保是双声道
        .filter(|config| config.sample_format() == cpal::SampleFormat::F32) // 优先使用F32格式
        .next()
        .expect("no supported config?!")
        .with_sample_rate(cpal::SampleRate(44100));

    println!("Using config: {:?}", supported_config);

    let config: cpal::StreamConfig = supported_config.into();
    let sample_rate = config.sample_rate.0;

    println!("Sample rate: {}", sample_rate);

    let synth = Arc::new(Mutex::new(Synthesizer::new(sample_rate)));

    // 设置初始参数
    {
        let mut synth = synth.lock().unwrap();
        let params = SynthParams {
            frequency: 440.0,
            amplitude: 0.5, // 降低初始音量以防止过响
            waveform: Waveform::Sine,
            ..Default::default()
        };
        synth.set_params(params);
    }

    println!("Creating audio stream...");

    let err_fn = |err| eprintln!("an error occurred on stream: {}", err);

    let synth_clone = Arc::clone(&synth);
    let stream = device.build_output_stream(
        &config,
        move |data: &mut [f32], _: &cpal::OutputCallbackInfo| {
            write_data(data, 2, &synth_clone);
        },
        err_fn
    )?;

    println!("Starting stream...");
    stream.play()?;

    // 保持stream在作用域内
    let stream = std::sync::Arc::new(stream);
    let stream_clone = Arc::clone(&stream);

    // 控制线程
    let synth_control = Arc::clone(&synth);
    thread::spawn(move || {
        println!("Starting frequency modulation...");
        let mut current_freq = 440.0;
        let mut ascending = true;
        loop {
            {
                let mut synth = synth_control.lock().unwrap();
                if ascending {
                    current_freq *= 1.01;
                    if current_freq >= 880.0 {
                        ascending = false;
                    }
                } else {
                    current_freq *= 0.99;
                    if current_freq <= 440.0 {
                        ascending = true;
                    }
                }
                synth.set_frequency(current_freq);
            }
            thread::sleep(Duration::from_millis(50));
        }
    });

    // 主线程等待
    println!("Press Enter to exit...");
    let mut input = String::new();
    std::io::stdin().read_line(&mut input)?;

    // 确保stream在程序结束前不会被丢弃
    drop(stream_clone);

    Ok(())
}

fn write_data(output: &mut [f32], channels: usize, synth: &Arc<Mutex<Synthesizer>>) {
    let mut synth = synth.lock().unwrap();

    for frame in output.chunks_mut(channels) {
        let value = synth.next_sample();
        // 直接使用f32值，不需要类型转换
        for sample in frame.iter_mut() {
            *sample = value;
        }
    }
}