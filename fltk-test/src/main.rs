use fltk::{app, valuator,valuator::Dial, group::Group, prelude::*, valuator::Slider, window::Window};

struct UserInterface {
    _group: Group,
}

impl UserInterface {
    pub fn new(x: i32, y: i32, w: i32, h: i32, l: &str) -> Self {
        let mut group = Group::new(x, y, w, h, None);

        // 创建多个 Dial 控件
        Dial::new(25, 25, 25, 25, Some("`"));
        Dial::new(60, 25, 25, 25, Some("`"));
        Dial::new(95, 25, 25, 25, None);

        // 创建多个 Slider 控件
        Slider::new(130, 20, 25, 90, None);
        Slider::new(165, 20, 25, 90, None);
        Slider::new(200, 20, 25, 90, None);
        Slider::new(240, 20, 25, 90, None);

        group.end();

        UserInterface { _group: group }
    }
}

fn main() {
    let app = app::App::default();
    let mut wind = Window::new(100, 100, 400, 300, "FLTK Rust Example");
    let _ui = UserInterface::new(0, 0, 400, 300, "");
    wind.end();
    wind.show();
    app.run().unwrap();
}
