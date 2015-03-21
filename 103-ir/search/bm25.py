import argparse
import os
import re

from pysearch.search import Search

from collections import defaultdict

rENG = re.compile('([a-zA-Z0-9._+*/-]+)')
NGRAM_NUM = 2

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

def use_pysearch(q, pysearch, num):
    ranked_list = pysearch.search(text_ngrams(q))

    return [os.path.basename(entry['path']) for entry in ranked_list[:num]]

def process_commands():
    parser = argparse.ArgumentParser()

    parser.add_argument('--m', dest='wm_dir', metavar='wm-dir', required=True,
            help='The input wm directory, which includes the files: '
            'vocab.all, file-list, inverted-index, stop-list, doc-length')

    # options for pysearch
    parser.add_argument('--r', dest='rel_feedback', action='store_true',
            help='If specified, turn on the relevance feedback.')
    parser.add_argument('--d', dest='news_dir', metavar='news-dir', required=True,
            help='The directory of NEWS documents. ')
    parser.add_argument('--l', dest='db_path', metavar='DB-PATH', required=True,
            help='Load SQLite3 database.')

    parser.add_argument('--q', dest='query_path', required=True)
    parser.add_argument('--o', dest='output_path', required=True)

    parser.add_argument('--n', dest='num', type=int, default=100)

    args = parser.parse_args()

    return args

if __name__ == '__main__':
    args = process_commands()

    pysearch_eng = Search(
            os.path.abspath(args.wm_dir),
            os.path.abspath(args.news_dir),
            os.path.abspath(args.db_path),
            args.rel_feedback)

    with open(args.query_path, 'r') as f, open(args.output_path, 'w') as of:
        for l in f:
            q, text = l.rstrip().split(' ', 1)
            print('process', q)
            of.write(q + '\n')
            results = use_pysearch(text, pysearch_eng, args.num)
            of.write('{}\n'.format(' '.join(results)))

