import argparse
import os
import query
import sqlite3
import sys

if __name__ == '__main__':

    # command line arguments
    parser = argparse.ArgumentParser(description='Search for news.')
    parser.add_argument('-r', action='store_true',
            help='If specified, turn on the relevance feedback in your program.')
    parser.add_argument('-i', action='store', metavar='query-file', required=True,
            help='The input query file.')
    parser.add_argument('-o', action='store', metavar='ranked-list', required=True,
            help='The output ranked list file.')
    parser.add_argument('-m', action='store', metavar='model-dir', required=True,
            help='The input model directory, which includes three files: '
            'model-dir/vocab.all, '
            'model-dir/file-list, '
            'model-dir/inverted-index')
    parser.add_argument('-d', action='store', metavar='NTCIR-dir', required=True,
            help='The directory of NTCIR documents, which is the path name of CIRB010 directory. ex. If the directory\'s pathname is /tmp2/CIRB010, it will be "-d /tmp2/CIRB010".')
    args = parser.parse_args()

    conn = sqlite3.connect(':memory:')
    c = conn.cursor()
    c.execute('create table vocab (id INTEGER PRIMARY KEY, vocab text)')

    print('== build word index...\n')

    # build vocab index
    with open(os.path.join(args.m, 'vocab.all'), 'r') as f:
        next(f)
        c.executemany('insert into vocab(vocab) values (?)', 
                ([l[:-1]] for l in f))

    # build word index
    raw_queries = query.parse_xml(args.i)
    word_index = query.collect_words(raw_queries)
    with open(os.path.join('data', 'words'), 'r') as f:
        for l in f:
            word_index.add(l[:-1])

    print('== build query vectors...\n')
    queries = query.parse_queries(raw_queries)


    for q in queries:
        print('== process query \'\'...\n')
        print('retrieve candidate documents...\n')
        print('rank documents...\n')
        print('rank feedback...\n')
        print('store results...\n')

    c.close()
