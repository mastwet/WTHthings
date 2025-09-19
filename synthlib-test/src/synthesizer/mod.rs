// src/synthesizer/mod.rs
use std::f64::consts::PI;

#[derive(Debug, Clone, Copy)]
pub enum Waveform {
    Sine,
    Square,
    Sawtooth,
    Triangle,
}

#[derive(Debug, Clone)]
pub struct Envelope {
    pub attack: f32,
    pub decay: f32,
    pub sustain: f32,
    pub release: f32,
}

impl Default for Envelope {
    fn default() -> Self {
        Envelope {
            attack: 0.01,
            decay: 0.1,
            sustain: 0.7,
            release: 0.2,
        }
    }
}

#[derive(Debug, Clone)]
pub struct SynthParams {
    pub frequency: f64,
    pub amplitude: f32,
    pub waveform: Waveform,
    pub envelope: Envelope,
    pub fine_tune: f64,
    pub phase: f64,
}

impl Default for SynthParams {
    fn default() -> Self {
        SynthParams {
            frequency: 440.0,
            amplitude: 0.5,
            waveform: Waveform::Sine,
            envelope: Envelope::default(),
            fine_tune: 0.0,
            phase: 0.0,
        }
    }
}

pub struct Synthesizer {
    sample_rate: u32,
    params: SynthParams,
    phase_acc: f64,
}

impl Synthesizer {
    pub fn new(sample_rate: u32) -> Self {
        println!("Creating synthesizer with sample rate: {}", sample_rate);
        Synthesizer {
            sample_rate,
            params: SynthParams::default(),
            phase_acc: 0.0,
        }
    }

    pub fn set_params(&mut self, params: SynthParams) {
        self.params = params;
    }

    pub fn set_frequency(&mut self, freq: f64) {
        self.params.frequency = freq;
    }

    pub fn set_amplitude(&mut self, amp: f32) {
        self.params.amplitude = amp.clamp(0.0, 1.0);
    }

    pub fn set_waveform(&mut self, waveform: Waveform) {
        self.params.waveform = waveform;
    }

    pub fn next_sample(&mut self) -> f32 {
        let frequency = self.params.frequency + self.params.fine_tune;
        let phase_inc = frequency / self.sample_rate as f64;

        self.phase_acc = (self.phase_acc + phase_inc) % 1.0;
        let phase = (self.phase_acc + self.params.phase) % 1.0;

        let raw_sample = match self.params.waveform {
            Waveform::Sine => {
                (phase * 2.0 * PI).sin()
            },
            Waveform::Square => {
                if phase < 0.5 { 1.0 } else { -1.0 }
            },
            Waveform::Sawtooth => {
                2.0 * phase - 1.0
            },
            Waveform::Triangle => {
                if phase < 0.25 {
                    4.0 * phase
                } else if phase < 0.75 {
                    2.0 - 4.0 * phase
                } else {
                    -4.0 + 4.0 * phase
                }
            }
        };

        // 应用音量并确保输出范围在 -1.0 到 1.0 之间
        let final_sample = (raw_sample * self.params.amplitude as f64) as f32;
        final_sample.clamp(-1.0, 1.0)
    }
}