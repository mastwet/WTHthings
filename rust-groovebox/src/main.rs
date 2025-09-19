use clap::Parser;
use cpal::traits::{DeviceTrait, HostTrait, StreamTrait};
use cpal::{Sample, SampleRate};
use hound::WavReader;
use rubato::{InterpolationParameters, InterpolationType, Resampler, SincFixedIn};
use std::sync::{Arc, Mutex};

const DEFAULT_SAMPLE_RATE: u32 = 44100;

#[derive(Parser, Debug)]
#[command(author, version, about, long_about = None)]
struct Args {
    /// Path to the WAV file to play
    #[arg(short, long, default_value = "/Users/mastwet/Desktop/1.wav")]
    file: String,
}

fn main() -> Result<(), anyhow::Error> {
    let args = Args::parse();

    // Open the WAV file
    let mut reader = WavReader::open(&args.file)?;
    let spec = reader.spec();

    // Initialize CPAL
    let host = cpal::default_host();
    let device = host.default_output_device().expect("no output device available");

    // Set up the output config with the default sample rate
    let config = cpal::StreamConfig {
        channels: spec.channels as cpal::ChannelCount,
        sample_rate: SampleRate(DEFAULT_SAMPLE_RATE),
        buffer_size: cpal::BufferSize::Default,
    };

    println!("Playing audio with {} channels", spec.channels);
    println!("Original sample rate: {} Hz", spec.sample_rate);
    println!("Output sample rate: {} Hz", DEFAULT_SAMPLE_RATE);

    // Prepare the audio samples
    let samples: Vec<f32> = reader.samples().map(|s| s.unwrap()).collect();

    // Resample if necessary
    let resampled_samples = if spec.sample_rate != DEFAULT_SAMPLE_RATE {
        let params = InterpolationParameters {
            sinc_len: 256,
            f_cutoff: 0.95,
            interpolation: InterpolationType::Linear,
            oversampling_factor: 256,
            window: rubato::WindowFunction::BlackmanHarris2,
        };
        let mut resampler = SincFixedIn::<f32>::new(
            DEFAULT_SAMPLE_RATE as f64 / spec.sample_rate as f64, // ratio
            0.0, // new f64 parameter (e.g., phase offset)
            params, // interpolation parameters
            samples.len() / spec.channels as usize, // chunk size
            spec.channels as usize, // number of channels
        )?;
        let waves_in = samples
            .chunks(spec.channels as usize)
            .map(|c| c.to_vec())
            .collect::<Vec<Vec<f32>>>();
        let waves_out = resampler.process(&waves_in, None)?;
        waves_out.into_iter().flatten().collect()
    } else {
        samples
    };

    let samples = Arc::new(Mutex::new(resampled_samples));
    let sample_index = Arc::new(Mutex::new(0));

    let samples_clone = Arc::clone(&samples);
    let sample_index_clone = Arc::clone(&sample_index);

    // Create the audio stream
    let stream = device.build_output_stream(
        &config,
        move |data: &mut [f32], _: &cpal::OutputCallbackInfo| {
            let samples = samples_clone.lock().unwrap();
            let mut index = sample_index_clone.lock().unwrap();
            for frame in data.chunks_mut(spec.channels as usize) {
                for (channel, sample) in frame.iter_mut().enumerate() {
                    *sample = if *index + channel < samples.len() {
                        samples[*index + channel]
                    } else {
                        0.0
                    };
                }
                *index += spec.channels as usize;
            }
        },
        |err| eprintln!("an error occurred on stream: {}", err),
    )?;

    // Play the audio
    stream.play()?;

    // Wait for playback to finish
    std::thread::sleep(std::time::Duration::from_secs_f32(
        (samples.lock().unwrap().len() as f32 / (DEFAULT_SAMPLE_RATE as f32 * spec.channels as f32)) + 1.0,
    ));

    Ok(())
}
