import ffmpeg

# 输入文件路径
input_video = "./tempvideo/21.webm"
subtitle_file = "srt_chn_temp.srt"  # SRT格式字幕文件
output_video = "output_with_subtitles1.mp4"

# 使用ffmpeg添加字幕
(
    ffmpeg
    .input(input_video)
    .output(output_video,  vcodec='h264_nvenc',vf=f"subtitles={subtitle_file}")
    .run()
)