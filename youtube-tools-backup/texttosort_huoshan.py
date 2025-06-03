#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import re
import asyncio
import uuid
import base64
import json
import requests
import tempfile
import os
from pydub import AudioSegment

# 火山引擎API配置参数
APPID = ""
ACCESS_TOKEN = ""
CLUSTER = "volcano_tts"
VOICE_TYPE = "BV701_streaming"  # 可根据需求更换其他语音类型
API_URL = "https://openspeech.bytedance.com/api/v1/tts"

file_path = "srt_chn_temp.srt"

# 原始字幕内容
TEXT = """
1
0:00:00,486 --> 0:00:05,692
嘿大家 如果你们正在看这个视频 就说明你们想学机器人学 对吧？

"""
# 输出文件路径
OUTPUT_FILE = "timeline_output.mp3"


def time_to_seconds(time_str):
    """将时间字符串转换为秒数"""
    hours, minutes, rest = time_str.split(':', 2)
    seconds, milliseconds = rest.replace(',', '.').split('.')
    return (
            int(hours) * 3600
            + int(minutes) * 60
            + int(seconds)
            + int(milliseconds) / 1000
    )


def parse_subtitle(subtitle):
    """解析字幕文本为结构化数据"""
    blocks = []
    current_block = {}

    for line in subtitle.strip().split('\n'):
        line = line.strip()
        if not line:
            continue

        # 检测序号行
        if re.match(r'^\d+$', line):
            if current_block:
                blocks.append(current_block)
            current_block = {"text": []}
            continue

        # 检测时间轴
        if '-->' in line:
            start_time, end_time = line.split('-->')
            current_block["start"] = start_time.strip()
            current_block["end"] = end_time.strip()
            current_block["start_sec"] = time_to_seconds(start_time.strip())
            current_block["end_sec"] = time_to_seconds(end_time.strip())
            continue

        # 文本内容
        current_block["text"].append(line)

    if current_block:
        blocks.append(current_block)

    # 合并多行文本并过滤空内容
    for block in blocks:
        block["text"] = " ".join(block["text"]).strip()
    return sorted(blocks, key=lambda x: x["start_sec"])


def generate_audio(text, voice_type, output_path):
    """使用火山引擎API生成音频片段"""
    request_json = {
        "app": {
            "appid": APPID,
            "token": ACCESS_TOKEN,
            "cluster": CLUSTER
        },
        "user": {
            "uid": "388808087185088"
        },
        "audio": {
            "voice_type": voice_type,
            "encoding": "mp3",
            "speed_ratio": 1.0,
            "volume_ratio": 1.0,
            "pitch_ratio": 1.0,
        },
        "request": {
            "reqid": str(uuid.uuid4()),
            "text": text,
            "text_type": "plain",
            "operation": "query",
            "with_frontend": 1,
            "frontend_type": "unitTson"
        }
    }

    headers = {"Authorization": f"Bearer;{ACCESS_TOKEN}"}

    try:
        resp = requests.post(API_URL, data=json.dumps(request_json), headers=headers, timeout=10)
        resp.raise_for_status()
        resp_json = resp.json()

        if "data" in resp_json:
            audio_data = base64.b64decode(resp_json["data"])
            with open(output_path, 'wb') as f:
                f.write(audio_data)
        else:
            raise Exception(f"API response error: {resp_json}")

    except Exception as e:
        raise RuntimeError(f"API请求失败: {str(e)}")


async def main():
    global TEXT
    global file_path
    try:
        with open(file_path, "r", encoding="utf-8") as f:
            TEXT = f.read()
    except FileNotFoundError:
        raise FileNotFoundError(f"字幕文件未找到: {file_path}")
    except Exception as e:
        raise RuntimeError(f"读取字幕文件失败: {str(e)}")
    # 解析字幕
    subtitle_blocks = parse_subtitle(TEXT)

    # 创建主音轨
    final_audio = AudioSegment.silent(duration=0)
    last_end = 0  # 记录上一个音频结束时间

    for idx, block in enumerate(subtitle_blocks, 1):
        # 生成临时文件
        with tempfile.NamedTemporaryFile(suffix=".mp3", delete=False) as tf:
            temp_path = tf.name

        # 生成语音片段
        print(f"生成片段 {idx}/{len(subtitle_blocks)}...")
        await asyncio.to_thread(
            generate_audio,
            block["text"],
            VOICE_TYPE,
            temp_path
        )

        # 加载音频片段
        try:
            audio = AudioSegment.from_mp3(temp_path)
        except Exception as e:
            raise RuntimeError(f"音频加载失败: {str(e)}")
        finally:
            os.unlink(temp_path)  # 删除临时文件

        # 计算需要插入的静音时长
        silence_duration = block["start_sec"] - last_end

        # 插入静音（如果必要）
        if silence_duration > 0:
            final_audio += AudioSegment.silent(int(silence_duration * 1000))

        # 追加当前音频
        final_audio += audio
        last_end = block["start_sec"] + (len(audio) / 1000)

        print(f"✅ 已处理 {block['start']} 片段 | 文本长度: {len(block['text'])}字符")

    # 导出最终文件
    final_audio.export(OUTPUT_FILE, format="mp3")
    print(f"\n成功生成音频 | 保存路径: {os.path.abspath(OUTPUT_FILE)}")
    print(f"总时长: {len(final_audio) / 1000:.2f}秒")


if __name__ == "__main__":
    asyncio.run(main())