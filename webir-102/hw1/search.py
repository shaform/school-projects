import argparse
import os
import sys

import db
import query

def process_commands():
    parser = argparse.ArgumentParser(description='Search for news.')
    parser.add_argument('-r', dest='rel_feedback', action='store_true',
            help='If specified, turn on the relevance feedback in your program.')
    parser.add_argument('-i', dest='input', metavar='query-file',
            action='store', required=True,
            help='The input query file.')
    parser.add_argument('-o', dest='output', metavar='ranked-list',
            action='store', required=True,
            help='The output ranked list file.')
    parser.add_argument('-m', dest='model_dir', metavar='model-dir',
            action='store', required=True,
            help='The input model directory, which includes three files: '
            'model-dir/vocab.all, '
            'model-dir/file-list, '
            'model-dir/inverted-index')
    parser.add_argument('-d', dest='doc_dir', metavar='NTCIR-dir',
            action='store', required=True,
            help='The directory of NTCIR documents, '
            'which is the path name of CIRB010 directory. '
            'ex. If the directory\'s pathname is /tmp2/CIRB010, '
            'it will be "-d /tmp2/CIRB010".')
    parser.add_argument('-l', dest='l_db_path', metavar='DB-PATH',
            action='store',
            help='Load SQLite3 database.')
    parser.add_argument('-s', dest='s_db_path', metavar='DB-PATH',
            action='store',
            help='Save SQLite3 database.')
    return parser.parse_args()

if __name__ == '__main__':

    args = process_commands()

    search_db = db.Database()
    db_path = search_db.MEMORY
    if args.l_db_path is not None:
        db_path = args.l_db_path
        search_db.open(args.model_dir, db_path)
    else:
        if args.s_db_path is not None:
            db_path = args.s_db_path
            if os.path.exists(db_path):
                os.remove(db_path)
        search_db.open(args.model_dir, db_path)
        search_db.build_index()

    
    print('== analyse queries...\n')
    queries = query.parse_queries(query.parse_xml(args.input), search_db)
    q_ngrams = query.collect_ngrams(queries)
    print('{} ngrams collected.\n'.format(len(q_ngrams)))


    search_db.build_doc_index(q_ngrams)


    for q in queries:
        print('== process query \'{}\'...\n'.format(q['number']))
        print('retrieve candidate documents...\n')
        print('rank documents...\n')
        print('rank feedback...\n')
        print('store results...\n')

    search_db.close()
