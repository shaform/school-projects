import argparse
import json
import os
import random
import requests
import time

from dateutil import parser

GOOG_URL = 'https://ajax.googleapis.com/ajax/services/search/news'

def process_commands():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('pool_path')
    parser.add_argument('raw_dir')
    parser.add_argument('cache_dir')
    parser.add_argument('output_path')

    return parser.parse_args()

def download_data(cache_dir, num, title):
    if not os.path.exists(os.path.join(cache_dir, num)):
        while True:
            try:
                print('downloading ... ', title)
                r = requests.get(GOOG_URL, params={'v': '1.0', 'q': title})
                jdata = r.json()
                print(r.text)
                if jdata['responseStatus'] == 200:
                    with open(os.path.join(cache_dir, num), 'w') as f:
                        f.write(r.text)
                    break
                else:
                    raise Exception('Could not get data')
            except:
                    print('error')
                    time.sleep(3600)
            time.sleep(2*random.random())

def get_data(cache_dir, num):
    with open(os.path.join(cache_dir, num), 'r') as f:
        return json.load(f)

if __name__ == '__main__':
    args = process_commands()

    items = set()
    with open(args.pool_path, 'r') as f:
        ignore = True
        for l in f:
            if not ignore:
                for x in l.rstrip().split(' '):
                    items.add(x)
            ignore = not ignore


    with open(args.output_path, 'w') as wf:
        for num in items:
            with open(os.path.join(args.raw_dir, num), 'r') as f:
                for i, l in enumerate(f):
                    if i == 4:
                        if l[:7] != '<title>' or l[-9:] != '</title>\n':
                            print('error on {} for {}'.format(num, l))
                            print(l[:7], l[-9:])
                        else:
                            try:
                                download_data(args.cache_dir, num, l[7:-9])
                                jdata = get_data(args.cache_dir, num)
                                results = jdata['responseData']['results']

                                media = date = 'Unknown'
                                for x in results:
                                    candm = x['publisher']
                                    date = parser.parse(x['publishedDate']).astimezone().strftime('%Y-%m-%d')

                                    if '中時' in candm:
                                        media = '0'
                                    elif '蘋果' in candm:
                                        media = '1'
                                    elif '自由' in candm:
                                        media = '2'
                                    else:
                                        continue
                                    break
                            except:
                                media = date = 'ERROR'
                            wf.write('{} {} {}\n'.format(num, media, date))
                            break
                else:
                    print('error on {}'.format(num))
