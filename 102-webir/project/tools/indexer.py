#
# Copyright 2014 Yong-Siang Shih.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 3,
# as published by the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

'''Index all text files and produce:
    1. file-list
    2. inverted-index
    3. vocab.all
    4. doc-length
    5. stop-list
'''
import argparse
import os
import re
import sys
import xml.etree.ElementTree as etree
from collections import defaultdict

NGRAM_NUM = 2

class Logger(object):
    def __init__(self, log_file):
        self.log_file = log_file

    def log_error(self, level, text):
        if self.log_file is not None:
            with open(self.log_file, 'a') as f:
                f.write('{}: {}\n'.format(level, text))

def parse_doc(text):
    doc = etree.fromstring(text).find('doc')
    return {'doc_id': doc.find('id').text.strip().lower(),
            'date': doc.find('date').text.strip(),
            'title': doc.find('title').text.strip(),
            'text': etree.tostring(doc.find('text'), encoding='unicode',
                method='text').strip()
            }

def tokenize(text):
    rENG = re.compile('([a-zA-Z0-9._+*/-]+)')
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

def text_ngrams(text, ngram_num=2):
    tokens = tokenize(text)
    ngram_list = []
    for gnum in range(1, ngram_num+1):
        for i in range(len(tokens)-gnum+1):
            ngram_list.append(tuple(tokens[i:i+gnum]))
    return ngram_list

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('source_dir', metavar='SOURCE-DIR', nargs=1)
    parser.add_argument('dest_dir', metavar='DEST-DIR', nargs=1)
    parser.add_argument('--debug', dest='debug_file', metavar='LOG-FILE',
            help='store the debug information')

    args = parser.parse_args()

    for d in args.source_dir + args.dest_dir:
        if not os.path.isdir(d):
            print('`{}` is not a directory'.format(d))
            sys.exit()

    input_dir = os.path.abspath(args.source_dir[0])
    output_dir = os.path.abspath(args.dest_dir[0])
    logger = Logger(args.debug_file)

    print('construct file-list')
    token_dict = defaultdict(int)
    f_num = 1
    error_num = 0
    dup_dict = {}
    dupped = {}
    with open(os.path.join(output_dir, 'file-list'), 'w') as f_list:
        with open(os.path.join(output_dir, 'doc-length'), 'w') as doc_length:
            for root, dirs, files in os.walk(input_dir):
                for f_name in files:
                    if root.find(input_dir) == -1:
                        print('something is wrong with {}'.format(root))
                        sys.exit()
                    else:

                        try:
                            file_path = os.path.join(root, f_name)
                            with open(file_path, 'r') as f:
                                doc = parse_doc(f.read())

                                if doc['title'] == '' or doc['text'] == '':
                                    logger.log_error('empty title or text', file_path)
                                    error_num += 1
                                    continue
                                elif doc['title'] in dup_dict:
                                    logger.log_error('possible dup {}'.format(doc['title']), file_path)
                                    dupped[doc['title']] = dup_dict[doc['title']]
                                    error_num += 1
                                    continue
                                else:
                                    dup_dict[doc['title']] = file_path

                                tokens = tokenize(doc['title'] + ' ' + doc['text'])
                                for token in tokens:
                                    token_dict[token] += 1
                        except:
                            logger.log_error('parse error', file_path)
                            error_num += 1
                            continue

                        doc_length.write('{}\n'.format(len(tokens)))
                        f_list.write(os.path.join(root[len(input_dir):].strip('/'), f_name) + '\n')

                        if f_num % 1000 == 0:
                            print('parsed {} docs'.format(f_num))
                        f_num += 1
    print('{} error files'.format(error_num))
    for k, v in dupped.items():
        logger.log_error('possible dup {}'.format(k), v)
    
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
                f_num = 1
                for l in f_list:
                    with open(os.path.join(input_dir, l.strip()), 'r') as f:
                        doc = parse_doc(f.read())
                        tokens = tokenize(doc['title'] + ' ' + doc['text'])
                        for i in range(len(tokens)-gnum+1):
                            ngram = tuple([token_dict[x] for x in tokens[i:i+gnum]])
                            index_dict[ngram][f_num] += 1
                    if f_num % 1000 == 0:
                        print('parsed {} docs'.format(f_num))
                    f_num += 1
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
