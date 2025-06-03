import yt_dlp

def format_selector(ctx):
    """ Select the best video and the best audio that won't result in an mkv.
    NOTE: This is just an example and does not handle all cases """

    # formats are already sorted worst to best
    formats = ctx.get('formats')[::-1]

    # acodec='none' means there is no audio
    best_video = next(f for f in formats
                      if f['vcodec'] != 'none' and f['acodec'] == 'none')

    # find compatible audio extension
    audio_ext = {'mp4': 'm4a', 'webm': 'webm'}[best_video['ext']]
    # vcodec='none' means there is no video
    best_audio = next(f for f in formats if (
        f['acodec'] != 'none' and f['vcodec'] == 'none' and f['ext'] == audio_ext))

    # These are the minimum required fields for a merged format
    yield {
        'format_id': f'{best_video["format_id"]}+{best_audio["format_id"]}',
        'ext': best_video['ext'],
        'requested_formats': [best_video, best_audio],
        # Must be + separated list of protocols
        'protocol': f'{best_video["protocol"]}+{best_audio["protocol"]}'
    }


def download_youtube_video(url, output_path="."):
    # 配置选项
    ydl_opts = {
        'format': format_selector,  # 下载最佳质量的视频+音频
        'outtmpl': f'{output_path}/%(title)s.%(ext)s',  # 输出文件名格式
        'noplaylist': True,  # 如果 URL 是播放列表，只下载单个视频
        'quiet': False,  # 显示下载信息
        'progress_hooks': [my_hook],  # 下载进度钩子
        'cookiefile':'./cookies.txt'
    }

    try:
        # 创建 yt-dlp 对象
        with yt_dlp.YoutubeDL(ydl_opts) as ydl:
            print(f"正在下载: {url}...")
            ydl.download([url])  # 下载视频
            print("下载完成！")
    except Exception as e:
        print(f"下载失败: {e}")

# 下载进度钩子函数
def my_hook(d):
    if d['status'] == 'downloading':
        print(f"下载进度: {d['_percent_str']} | 已下载: {d['_downloaded_bytes_str']}")
    elif d['status'] == 'finished':
        print("视频下载完成，正在处理...")

if __name__ == "__main__":
    # 输入 YouTube 视频的 URL
    video_url = input("请输入 YouTube 视频的 URL: ")

    # 输入保存路径（可选，默认为当前目录）
    save_path = input("请输入保存路径（可选，默认为当前目录）: ") or "."

    # 调用下载函数
    download_youtube_video(video_url, save_path)