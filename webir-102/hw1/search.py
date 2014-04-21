from multiprocessing import Pool

import argparse
import os
import random
import sys

import config
import db
import query
import vsm

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

def create_database(args):
    search_db = db.Database()
    db_path = search_db.MEMORY
    if args.l_db_path is not None:
        db_path = args.l_db_path
    elif args.s_db_path is not None:
        db_path = args.s_db_path
        if os.path.exists(db_path):
            os.remove(db_path)

    search_db.open(args.model_dir, args.doc_dir, db_path)
    
    # build index when no db loaded
    if args.l_db_path is None:
        search_db.build_index()

    print('== analyse queries...')
    queries = query.parse_queries(query.parse_xml(args.input), search_db)
    q_ngrams = query.collect_ngrams(queries)
    print('{} ngrams collected.'.format(len(q_ngrams)))

    # build doc index when no db loaded
    if args.l_db_path is None:
        search_db.build_doc_index(q_ngrams)

    search_db.collect_idfs(q_ngrams)

    return queries, search_db

def gen_tasks(terms, search_db):
    l = len(terms)
    sk = int(l/config.PP) + 1
    for i in range(0, l, sk):
        yield terms[i:i+sk], search_db.copy()

def worker(params):
    terms, search_db = params
    search_db.open_simple(search_db.db_path)
    doc_rank = vsm.rank_terms(terms, search_db)
    search_db.close()
    print('ranked {} terms...'.format(len(terms)))
    return doc_rank

if __name__ == '__main__':

    args = process_commands()

    queries, search_db = create_database(args)

    with open(args.output, 'w') as f:
        pool = None
        if config.PP > 0 and search_db.db_path != search_db.MEMORY:
            print('== multiprocessing enabled ==')
            pool = Pool(config.PP)

        fb_num = 1
        if args.rel_feedback:
            print('== feedback enabled ==')
            fb_num = config.FB_IT

        for q in queries:
            print('== process query \'{}\'...'.format(q['number']))

            print('rank documents...')
            for it in range(0, fb_num):
                print('process terms...')
                terms = query.process_terms(q['vector'], search_db)

                if pool is not None:
                    ranked_list = vsm.ranked_list(pool.map(worker,
                        gen_tasks(terms, search_db)))
                else:
                    ranked_list = vsm.ranked_list([vsm.rank_terms(terms, search_db)])

                if args.rel_feedback:
                    print('iteration {} done...'.format(it+1))
                    q = vsm.qfeedback(ranked_list, q, search_db)
#                   with open(args.output + '-{}'.format(it), 'a+') as nf:
#                       for r in ranked_list[:100]:
#                           nf.write('{} {}\n'.format(q['number'][-3:], search_db.doc_id(r['id'])))

            print('store results...')
            for r in ranked_list[:100]:
                f.write('{} {}\n'.format(q['number'][-3:], search_db.doc_id(r['id'])))
        if pool is not None:
            pool.close()

    search_db.close()
