use eframe::egui::{self, Align, Color32, Frame, RichText, Rounding, ScrollArea, Vec2, TopBottomPanel, Align2};
use eframe::egui::style::Margin;
use eframe::{App, NativeOptions};
use std::path::PathBuf;
use rfd::FileDialog;

struct ChatApp {
    chats: Vec<String>,       // 所有会话名称
    current_chat: usize,      // 当前选中会话的索引
    messages: Vec<Message>,   // 当前会话的消息列表
    input: String,
    file_imported: bool,      // 跟踪是否已导入文件
    selected_file: Option<PathBuf>, // 存储选择的文件路径
    show_settings: bool,      // 控制设置面板显示
    settings: Settings,       // 存储配置项
}

struct Message {
    sender: String,
    content: String,
    timestamp: String,
    is_self: bool,
}

struct Settings {
    theme: Theme,
    message_font_size: f32,
}

#[derive(PartialEq)]
enum Theme {
    Light,
    Dark,
}

impl Default for ChatApp {
    fn default() -> Self {
        let chats: Vec<String> = vec!["Alice".into(), "Bob".into(), "Charlie".into()];
        let current_chat = 0;
        let messages = vec![
            Message { 
                sender: chats[current_chat].clone(), 
                content: "Hey, how's it going?".into(), 
                timestamp: get_current_time(), 
                is_self: false 
            },
            Message { 
                sender: "Me".into(), 
                content: "Doing great, you?".into(), 
                timestamp: get_current_time(), 
                is_self: true 
            },
        ];
        Self {
            chats,
            current_chat,
            messages,
            input: String::new(),
            file_imported: false,
            selected_file: None,
            show_settings: false,
            settings: Settings {
                theme: Theme::Light,
                message_font_size: 14.0,
            },
        }
    }
}

impl App for ChatApp {
    fn update(&mut self, ctx: &egui::Context, _frame: &mut eframe::Frame) {
        // 应用主题
        if self.settings.theme == Theme::Dark {
            ctx.set_visuals(egui::Visuals::dark());
        } else {
            ctx.set_visuals(egui::Visuals::light());
        }

        if !self.file_imported {
            // 显示文件导入界面
            egui::CentralPanel::default()
                .frame(Frame::default().inner_margin(Margin::same(20.0)))
                .show(ctx, |ui| {
                    ui.vertical_centered(|ui| {
                        ui.heading("Welcome to Chat App");
                        ui.add_space(20.0);
                        ui.label("Please import a file to continue:");
                        ui.add_space(10.0);

                        if ui.button("Select File").clicked() {
                            if let Some(path) = FileDialog::new()
                                .add_filter("Text", &["txt"])
                                .pick_file() {
                                self.selected_file = Some(path);
                            }
                        }

                        if let Some(path) = &self.selected_file {
                            ui.add_space(10.0);
                            ui.label(format!("Selected: {}", path.display()));
                            if ui.button("Confirm Import").clicked() {
                                self.file_imported = true;
                                self.messages.push(Message {
                                    sender: "System".into(),
                                    content: format!("Imported file: {}", path.display()),
                                    timestamp: get_current_time(),
                                    is_self: false,
                                });
                            }
                        }
                    });
                });
        } else {
            // 顶部标签栏：打开/关闭 会话 + 设置按钮
            TopBottomPanel::top("chat_tabs").show(ctx, |ui| {
                ui.horizontal(|ui| {
                    // 聊天标签
                    let mut remove_idx: Option<usize> = None;
                    for (i, label) in self.chats.iter().enumerate() {
                        ui.horizontal(|ui| {
                            let selected = i == self.current_chat;
                            if ui.selectable_label(selected, label).clicked() {
                                self.current_chat = i;
                                self.messages = vec![ Message {
                                    sender: label.clone(),
                                    content: format!("Welcome to {} chat!", label),
                                    timestamp: get_current_time(),
                                    is_self: false,
                                }];
                            }
                            if ui.small_button("×").clicked() {
                                remove_idx = Some(i);
                            }
                        });
                    }
                    if let Some(i) = remove_idx {
                        self.chats.remove(i);
                        if self.current_chat >= self.chats.len() {
                            self.current_chat = self.chats.len().saturating_sub(1);
                        }
                        self.messages.clear();
                    }
                    if ui.button("+ New Chat").clicked() {
                        let name = format!("Chat {}", self.chats.len() + 1);
                        self.chats.push(name.clone());
                        self.current_chat = self.chats.len() - 1;
                        self.messages = vec![ Message {
                            sender: name.clone(),
                            content: "Started new chat.".into(),
                            timestamp: get_current_time(),
                            is_self: false,
                        }];
                    }
                    // 设置按钮
                    ui.with_layout(egui::Layout::right_to_left(Align::Center), |ui| {
                        if ui.button("⚙ Settings").clicked() {
                            self.show_settings = true;
                        }
                    });
                });
                ui.separator();
            });

            // 设置面板
            if self.show_settings {
                egui::Window::new("Settings")
                    .collapsible(false)
                    .resizable(false)
                    .anchor(Align2::CENTER_CENTER, [0.0, 0.0])
                    .show(ctx, |ui| {
                        ui.vertical(|ui| {
                            ui.heading("Chat Settings");
                            ui.add_space(10.0);
                            ui.horizontal(|ui| {
                                ui.label("Theme: ");
                                ui.radio_value(&mut self.settings.theme, Theme::Light, "Light");
                                ui.radio_value(&mut self.settings.theme, Theme::Dark, "Dark");
                            });
                            ui.add_space(10.0);
                            ui.horizontal(|ui| {
                                ui.label("Message Font Size: ");
                                ui.add(egui::Slider::new(&mut self.settings.message_font_size, 10.0..=20.0).text("pt"));
                            });
                            ui.add_space(20.0);
                            if ui.button("Close").clicked() {
                                self.show_settings = false;
                            }
                        });
                    });
            }

            // 中央面板：标题 + 消息 + 输入框
            egui::CentralPanel::default()
                .frame(Frame::default().inner_margin(Margin::same(10.0)))
                .show(ctx, |ui| {
                    ui.horizontal(|ui| {
                        ui.heading(&self.chats[self.current_chat]);
                        ui.with_layout(egui::Layout::right_to_left(Align::Center), |ui| {
                            ui.label(RichText::new("Online").color(Color32::GRAY));
                        });
                    });
                    ui.add_space(5.0);
                    ui.separator();

                    let bottom_height = 60.0;
                    ScrollArea::vertical()
                        .auto_shrink([false, false])
                        .stick_to_bottom(true)
                        .max_height(ui.available_height() - bottom_height)
                        .show(ui, |ui| {
                            for msg in &self.messages {
                                ui.add_space(8.0);
                                if msg.is_self {
                                    ui.horizontal(|ui| {
                                        ui.add_space(ui.available_width() * 0.3);
                                        draw_bubble(ui, msg, true, self.settings.message_font_size);
                                    });
                                } else {
                                    ui.horizontal(|ui| {
                                        ui.label(RichText::new("👤").size(24.0));
                                        ui.add_space(4.0);
                                        draw_bubble(ui, msg, false, self.settings.message_font_size);
                                    });
                                }
                            }
                        });

                    ui.add_space(10.0);
                    ui.separator();
                    ui.add_space(5.0);
                    ui.horizontal(|ui| {
                        let resp = ui.add(
                            egui::TextEdit::singleline(&mut self.input)
                                .hint_text("Type a message...")
                                .desired_width(ui.available_width() - 80.0)
                                .margin(Vec2::new(10.0, 8.0)),
                        );
                        if ui.button("Send").clicked() || (resp.lost_focus() && ui.input().key_pressed(egui::Key::Enter)) {
                            if !self.input.trim().is_empty() {
                                self.messages.push(Message {
                                    sender: "Me".into(),
                                    content: self.input.clone(),
                                    timestamp: get_current_time(),
                                    is_self: true,
                                });
                                self.input.clear();
                                resp.request_focus();
                            }
                        }
                    });
                });
        }
    }
}

fn draw_bubble(ui: &mut egui::Ui, msg: &Message, is_self: bool, font_size: f32) {
    let bubble = Frame::none()
        .fill(if is_self { Color32::from_rgb(200, 230, 200) } else { Color32::from_rgb(240, 240, 240) })
        .rounding(Rounding::same(10.0))
        .inner_margin(Margin::symmetric(10.0, 6.0));
    bubble.show(ui, |ui| {
        ui.vertical(|ui| {
            ui.label(RichText::new(&msg.content).size(font_size));
            ui.add_space(3.0);
            ui.with_layout(egui::Layout::right_to_left(Align::Center), |ui| {
                ui.label(RichText::new(&msg.timestamp).size(10.0).color(Color32::GRAY));
            });
        });
    });
}

fn get_current_time() -> String {
    chrono::Local::now().format("%H:%M").to_string()
}

fn main() {
    let app = ChatApp::default();
    let mut native_options = NativeOptions::default();
    native_options.initial_window_size = Some(Vec2::new(600.0, 600.0));
    eframe::run_native("Chat App", native_options, Box::new(|_cc| Box::new(app)));
}