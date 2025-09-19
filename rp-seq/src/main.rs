use fltk::{
    app,
    enums::{Align, Color, FrameType, Shortcut},
    frame::Frame,
    group::{Flex, Pack},
    prelude::*,
    window::Window,
    button::Button,
};

const TRACK_HEIGHT: i32 = 100;
const BUTTON_SIZE: i32 = 50;
const SIDEBAR_WIDTH: i32 = 70;
const WINDOW_WIDTH: i32 = 800;
const WINDOW_HEIGHT: i32 = 600;

fn main() {
    // 创建应用程序
    let app = app::App::default().with_scheme(app::Scheme::Gleam);

    // 创建主窗口
    let mut wind = Window::new(100, 100, WINDOW_WIDTH, WINDOW_HEIGHT, "音序器主界面");

    // 使用 Flex 分割窗口，水平分割为左右两部分
    let mut main_flex = Flex::default()
        .size_of_parent()
        .row(); // 水平排列子组件

    // 左侧功能栏
    let mut left_flex = Flex::default()
        .with_size(SIDEBAR_WIDTH, 0) // 设置左侧宽度为70
        .column(); // 垂直排列按钮

    // 添加一些上边距以使按钮与轨道中心对齐
    let top_padding = (WINDOW_HEIGHT - (4 * BUTTON_SIZE)) / 2;
    let mut padding = Frame::default().with_size(SIDEBAR_WIDTH, top_padding as i32);
    left_flex.set_size(&padding, top_padding as i32);

    // 创建功能按钮并垂直排列
    let mut buttons = Vec::new();
    for i in 1..=4 {
        let btn = Button::new(
            0,
            0,
            BUTTON_SIZE,
            BUTTON_SIZE,
            &format!("{}", i),
        );
        btn.set_color(Color::from_u32(0x4CAF50)); // 设置按钮颜色
        btn.set_label_size(14);
        btn.set_frame(FrameType::RoundUpBox);
        btn.set_label_color(Color::White);
        btn.set_tooltip(&format!("界面{}", i));
        buttons.push(btn);
        left_flex.set_size(&btn, BUTTON_SIZE);
    }

    // 添加一些下边距以保持整体居中
    let bottom_padding = top_padding;
    let mut padding_bottom = Frame::default().with_size(SIDEBAR_WIDTH, bottom_padding as i32);
    left_flex.set_size(&padding_bottom, bottom_padding as i32);

    left_flex.end();

    // 右侧主页面
    let mut right_flex = Flex::default()
        .with_size(WINDOW_WIDTH - SIDEBAR_WIDTH, 0)
        .column()
        .margin(10); // 增加外边距

    // 创建一个向量来存储标签字符串，确保它们在整个界面生命周期内有效
    let mut labels = Vec::new();

    // 创建4个 MIDI 轨道
    for i in 1..=4 {
        let label = format!("MIDI 轨道 {}", i);
        labels.push(label); // 将标签字符串存储在向量中

        let mut track = Frame::new(0, 0, WINDOW_WIDTH - SIDEBAR_WIDTH - 20, TRACK_HEIGHT, labels[i as usize - 1].as_str());
        track.set_label_size(16);
        track.set_align(Align::Left | Align::Inside);
        track.set_frame(FrameType::FlatBox);
        track.set_color(Color::White);
        track.set_label_color(Color::Black);
        track.set_frame(FrameType::EngravedFrame);
        right_flex.set_size(&track, TRACK_HEIGHT);
    }

    right_flex.end();

    main_flex.end();
    wind.end();
    wind.show();

    // 事件处理（示例：按钮点击）
    for (i, btn) in buttons.iter().enumerate() {
        let label = format!("界面{}", i + 1);
        btn.set_callback(move |_| {
            println!("{}按钮被点击", label);
            // 在这里添加切换到对应界面的逻辑
        });
    }

    // 运行应用程序
    app.run().unwrap();
}
