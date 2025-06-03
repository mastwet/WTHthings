from spleeter.separator import Separator
import os

# 设置输入输出路径
INPUT_AUDIO = ".\\20250513.wav"       # 替换为你自己的音频文件路径
OUTPUT_DIR = "output"              # 输出目录
MODEL = "spleeter:2stems"          # 使用2音轨模型：人声 + 背景

def separate_vocals_and_background(input_file, output_dir, model_name="spleeter:2stems"):
    # 创建输出目录
    os.makedirs(output_dir, exist_ok=True)

    # 初始化 Separator 对象
    separator = Separator(model_name)

    # 执行分离并保存结果
    print(f"Separating {input_file} using {model_name} model...")
    separator.separate_to_file(
        input_file,
        output_dir,
        filename_format="{filename}/{instrument}.{codec}",
    )

    print("✅ Separation complete!")

if __name__ == "__main__":
    separate_vocals_and_background(INPUT_AUDIO, OUTPUT_DIR, MODEL)
