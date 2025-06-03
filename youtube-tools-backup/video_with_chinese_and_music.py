from moviepy import VideoFileClip, AudioFileClip, concatenate_videoclips

# 统一目标分辨率（720P）
TARGET_RESOLUTION = (1280, 720)

# 处理主视频：调整尺寸并替换音频
def process_main_video():
    main_video = (
        VideoFileClip("tempvideo/21.mp4")
        .resized(TARGET_RESOLUTION)  # 强制拉伸至目标尺寸
        .without_audio()
    )
    new_audio = AudioFileClip("tempvideo/331.mp3")
    return main_video.with_audio(new_audio)

# 处理主视频：调整尺寸并替换音频
def process_main_video_no_voicepros():
    main_video = (
        VideoFileClip("202505132.mp4")
        .resized(TARGET_RESOLUTION)  # 强制拉伸至目标尺寸
    )
    return main_video

# 加载并调整片头/片尾尺寸
pian_tou = VideoFileClip("piantou.mp4").resized(TARGET_RESOLUTION)
pian_wei = VideoFileClip("pianwei.mp4").resized(TARGET_RESOLUTION)

# 处理主视频及拼接
processed_main = process_main_video()
#processed_main = process_main_video_no_voicepros()
final_clip = concatenate_videoclips([pian_tou, processed_main, pian_wei])

# 输出最终视频
final_clip.write_videofile(
    "final_output.mp4",
    codec="libx264",
    audio_codec="aac",
    fps=24,
    threads=8
)