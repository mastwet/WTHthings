from pydub import AudioSegment

# 加载背景音乐和语音文件
bgm = AudioSegment.from_file("bgm.mp3")
voice = AudioSegment.from_file("timeline_output.mp3")

# 确保语音长度不超过背景音乐长度（如果语音比背景音乐长，截断语音）
if len(bgm) > len(voice):
    bgm = bgm[:len(voice)]

# 将语音叠加到背景音乐上
mixed = bgm.overlay(voice)

# 导出混合后的音频文件
mixed.export("mixed.mp3", format="mp3")
