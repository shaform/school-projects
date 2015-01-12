"""
Index all text files and produce:
    1. file-list
    2. inverted-index
    3. vocab.all
    4. doc-length
    5. stop-list
"""
import argparse
import os
import sys
import re
import xml.etree.ElementTree as etree

from collections import defaultdict

rENG = re.compile('([a-zA-Z0-9._+*/-]+)')
NGRAM_NUM = 2

class Logger(object):
    def __init__(self, log_file=None):
        self.log_file = log_file

    def log_error(self, level, text):
        msg = '{}: {}\n'.format(level, text)
        print(msg)
        if self.log_file is not None:
            with open(self.log_file, 'a') as f:
                f.write(msg)

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

def tokenize(text):
    tokens = []
    for each in text.split():
        each = each.strip()
        for t in rENG.split(each):
            m = rENG.match(t)
            if m is not None:
                tokens.append(t)
            else:
                for u in re.split('\W+', t):
                    for c in u:
                        tokens.append(c)
    return tokens

def text_ngrams(text):
    tokens = tokenize(text)
    ngram_list = []
    for gnum in range(1, NGRAM_NUM+1):
        for i in range(len(tokens)-gnum+1):
            ngram_list.append(tuple(tokens[i:i+gnum]))
    return ngram_list


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('source_dir')
    parser.add_argument('dest_dir')
    parser.add_argument('--debug', dest='debug_file', metavar='LOG-FILE',
            help='store the debug information')

    args = parser.parse_args()

    for d in args.source_dir, args.dest_dir:
        if not os.path.isdir(d):
            print('`{}` is not a directory'.format(d))
            sys.exit()

    input_dir = os.path.abspath(args.source_dir)
    output_dir = os.path.abspath(args.dest_dir)
    logger = Logger(args.debug_file)

    print('construct file-list')
    token_dict = defaultdict(int)
    f_num = 1
    error_num = 0
    with open(os.path.join(output_dir, 'file-list'), 'w') as f_list, open(os.path.join(output_dir, 'doc-length'), 'w') as doc_length:
        for root, dirs, files in os.walk(input_dir):
            for f_name in sorted(files, key=lambda x: int(x)):
                if root.find(input_dir) == -1:
                    print('something is wrong with {}'.format(root))
                    sys.exit()
                else:
                    try:
                        file_path = os.path.join(root, f_name)
                        with open(file_path, 'r') as f:
                            doc = parse_doc(f.read())

                            text = []
                            
                            if doc['title']:
                                text.append(doc['title'])
                            if doc['text']:
                                text.append(doc['text'])

                            tokens = tokenize(' '.join(text))
                            for token in tokens:
                                token_dict[token] += 1
                    except Exception as e:
                        print(e)
                        logger.log_error('parse error', file_path)
                        error_num += 1
                        continue

                    doc_length.write('{}\n'.format(len(tokens)))
                    f_list.write(os.path.join(root[len(input_dir):].strip('/'), f_name) + '\n')

                    if f_num % 1000 == 0:
                        print('parsed {} docs'.format(f_num))
                    f_num += 1
    print('{} error files'.format(error_num))
    
    print('construct vocab.all')
    vocab_list = []
    for k, v in token_dict.items():
        vocab_list.append((v, k))
    vocab_list.sort(reverse=True)
    with open(os.path.join(output_dir, 'vocab.all'), 'w') as f:
        v_num = 1
        for _, k in vocab_list:
            f.write(k + '\n')
            token_dict[k] = v_num
            v_num += 1

    print('index files')
    stop_list = []
    with open(os.path.join(output_dir, 'inverted-index'), 'w') as inv_index:
        for gnum in range(1, NGRAM_NUM+1):
            print('index {}-gram'.format(gnum))
            index_dict = defaultdict(lambda: defaultdict(int))
            with open(os.path.join(output_dir, 'file-list'), 'r') as f_list:
                f_num = 0
                for l in f_list:
                    with open(os.path.join(input_dir, l.strip()), 'r') as f:
                        doc = parse_doc(f.read())

                        text = []
                        
                        if doc['title']:
                            text.append(doc['title'])
                        if doc['text']:
                            text.append(doc['text'])

                        tokens = tokenize(' '.join(text))
                        for i in range(len(tokens)-gnum+1):
                            ngram = tuple([token_dict[x] for x in tokens[i:i+gnum]])
                            index_dict[ngram][f_num] += 1
                    f_num += 1
                    if f_num % 1000 == 0:
                        print('parsed {} docs'.format(f_num))
            print('write to inverted-index')
            for ngram in sorted(index_dict.keys()):
                ngram_dict = index_dict[ngram]
                ngram_text = ','.join([str(x) for x in ngram])
                ngram_count = 0
                inv_index.write('#{} {}\n'.format(ngram_text, len(ngram_dict)))
                for doc, count in ngram_dict.items():
                    ngram_count += count
                    inv_index.write('{} {}\n'.format(doc, count))
                stop_list.append((ngram_count, ngram_text))

    print('construct stop_list')
    stop_list.sort(reverse=True)
    with open(os.path.join(output_dir, 'stop-list'), 'w') as f:
        for count, text in stop_list:
            f.write('{} {}\n'.format(text, count))
