import argparse
import os
import re
import socket
import xml.etree.ElementTree as etree

class CKIP(object):
    def __init__(self, server, port, user, password, pos=False):
        self.server = server
        self.port = port
        self.user = user
        self.password = password
        self.pos = pos

    def segment(self, text, *, pos=None):
        if pos is None:
            pos = self.pos
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.connect((self.server, self.port))
        query = '''<?xml version="1.0" ?>
<wordsegmentation version="0.1" charsetcode="utf-8">
<option showcategory="0" />
<authentication username="{}" password="{}" />
<text><![CDATA[{}]]></text>
</wordsegmentation>'''.format(self.user, self.password, text)
        sock.send(query.encode(encoding="utf-8"))
        result = sock.recv(1048576).decode("utf-8", errors='ignore')
        sock.close()
        try:
            stripped = result.replace("</sentence><sentence>","").split("<sentence>")[1].split("</sentence>")[0]
        except:
            print(text)
            print(result)
            raise
        if pos:
            return stripped.replace('\u3000', ' ').strip()
        else:
            return re.sub(r'\([^) ]+\)(\u3000|$)', ' ', stripped).strip()

def parse_doc(text):
    try:
        doc = etree.fromstring(text).find('doc')
    except etree.ParseError:
        text = re.sub(r'<[Bb][Rr]>', '', text)
        text = re.sub(r'<(script|style)[^\n]*\n', '', text)
        doc = etree.fromstring(text).find('doc')

    title = ''
    text = ''
    try:
        title = doc.find('title').text.strip()
    except:
        pass
    try:
        text = etree.tostring(doc.find('text'), encoding='unicode', method='text').strip()
    except:
        pass
    return {'doc_id': doc.find('id').text.strip().lower(),
            'title': title,
            'text': text
            }

def split_text(text):
    r_sen = re.compile(r'([^。；！？…]+[。；！？…])')
    return (x.strip() for l in text.split('\n\n\n') for x in r_sen.split(' '.join(l.replace('\ufeff', '').split('\n'))) if x.strip())

def process_commands():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('source_dir')
    parser.add_argument('dest_path')
    parser.add_argument('--cont', type=int)
    parser.add_argument('--pos', action='store_true')
    parser.add_argument('--server', required=True)
    parser.add_argument('--port', required=True, type=int)
    parser.add_argument('--user', required=True)
    parser.add_argument('--password', required=True)

    return parser.parse_args()

if __name__ == '__main__':
    args = process_commands()

    input_dir = os.path.abspath(args.source_dir)
    mode = 'w'
    if args.cont:
        mode = 'a'
    ckip = CKIP(args.server, args.port, args.user, args.password, args.pos)
    with open(args.dest_path, mode) as of:
        nums = 0
        for root, dirs, files in os.walk(input_dir):
            for f_name in sorted(files, key=lambda x: int(x)):
                if args.cont and int(f_name) < args.cont:
                    continue
                    
                file_path = os.path.join(root, f_name)
                with open(file_path, 'r') as f:
                    doc = parse_doc(f.read())
                    dnum = doc['doc_id']
                    if int(dnum) != int(f_name):
                        print('id mismatch {} {}'.format(dnum, f_name))

                    if doc['title']:
                        of.write('@@@@{}-t {}\n'.format(dnum, ckip.segment(doc['title'])))
                    if doc['text']:
                        for i, sen in enumerate(split_text(doc['text'])):
                            of.write('@@@@{}-{} {}\n'.format(dnum, i, ckip.segment(sen)))
                nums += 1

                if nums % 5000 == 0:
                    print('{} processed'.format(nums))
