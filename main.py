from datetime import datetime
from pathlib import Path
import os
import re


def get_path(path):
    ret = Path(path)
    if not ret.exists():
        ret.mkdir(parents=True)
    return ret


def init():
    now = datetime.now().astimezone()
    print(f'Current time: {now}')

    working_dir = now.isoformat(timespec='seconds')
    working_dir = working_dir.replace(':', '_')
    working_path = get_path(f'ig_dl/{working_dir}')
    os.chdir(working_path)
    print(f'Working directory: {os.getcwd()}')

    with open('../input.txt', 'r', encoding='utf-8') as input_file, \
         open('all.txt', 'w', encoding='utf-8') as all_requests:
        for line in input_file:
            all_requests.write(line)


def main():
    init()

    video_filename_pattern = re.compile('[A-Z0-9]*_video_dashinit\\.mp4')
    file_count = 0
    download_filename = None

    with open('all.txt', 'r', encoding='utf-8') as all_requests, \
         open('download.sh', 'w', encoding='utf-8',
              newline='\n') as download_script, \
         open('list.txt', 'w') as file_list:
        for line in all_requests:
            if line[:4] == 'curl':  # Start of a new request
                video_filename_match = video_filename_pattern.search(line)
                if video_filename_match is not None:  # Current request is video
                    video_filename = video_filename_match.group(0)
                    download_filename = f'{file_count}_{video_filename}'
                    print(download_filename)
                    file_list.write(f'{download_filename}\n')
                    file_count += 1
                else:
                    download_filename = None

            if download_filename is not None:
                if line[0:14] == '  --compressed':  # Last line of request
                    download_script.write(f'  -o {download_filename} \\\n')
                download_script.write(line)


if __name__ == '__main__':
    main()
