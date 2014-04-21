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

if __name__ == '__main__':

    args = process_commands()

    search_db = db.Database()
    db_path = search_db.MEMORY
    if args.l_db_path is not None:
        db_path = args.l_db_path
        search_db.open(args.model_dir, args.doc_dir, db_path)
    else:
        if args.s_db_path is not None:
            db_path = args.s_db_path
            if os.path.exists(db_path):
                os.remove(db_path)
        search_db.open(args.model_dir, args.doc_dir, db_path)
        search_db.build_index()

    
    print('== analyse queries...')
    queries = query.parse_queries(query.parse_xml(args.input), search_db)
    q_ngrams = query.collect_ngrams(queries)
    print('{} ngrams collected.'.format(len(q_ngrams)))

    if args.l_db_path is None:
        search_db.build_doc_index(q_ngrams)

    with open(args.output, 'w') as f:
        pool = None
        if config.PP > 0 and db_path != search_db.MEMORY:
            print('== multiprocessing enabled ==')
            pool = Pool(config.PP)
        for q in queries:
            print('== process query \'{}\'...'.format(q['number']))

            print('retrieve candidate documents...')
            ranked_list = search_db.retrieve_docs(q['vector'])
            print('{} docs retrieved'.format(len(ranked_list)))

            print('rank documents...')
            fb_it = 1
            if args.rel_feedback:
                print('== feedback enabled ==')
                fb_it = config.FB_IT
                config.OKAPI_BM25 = False

            for it in range(0,fb_it):
                if pool is not None:
                    def gen_task():
                        l = len(ranked_list)
                        sk = int(l/config.PP) + 1
                        for i in range(0, l, sk):
                            yield ranked_list[i:i+sk], q, (search_db, db_path)
                    new_list = pool.map(vsm.rank_docs, gen_task())
                    ranked_list = []
                    for l in new_list:
                        ranked_list.extend(l)
                else:
                    vsm.rank_docs((ranked_list, q, search_db))

                ranked_list.sort(key=lambda x: x['value'], reverse=True)
                if args.rel_feedback:
                    print('iteration {} done...'.format(it+1))
                    fb, q = vsm.feedback_prepare(ranked_list, q, search_db)
                    ranked_list = ranked_list[:config.FB_CUT]
                    with open(args.output + '-{}'.format(it), 'a+') as nf:
                        for r in ranked_list[:100]:
                            nf.write('{} {}\n'.format(q['number'][-3:], search_db.doc_id(r['id'])))
#
#           if args.rel_feedback:
#               print('start feedback process...')
#               for it in range(config.FB_IT):
#                   print('iteration {}...'.format(it+1))
#                   if len(ranked_list) < config.FB_REL + config.FB_NREL:
#                       continue
#                   fb, q = vsm.feedback_prepare(ranked_list, q, search_db)
#                   for d in ranked_list:
#                       val = vsm.sim_feedback(fb, d, q, search_db)
#                       d['value'] = val
#                   ranked_list.sort(key=lambda x: x['value'], reverse=True)
#
            print('store results...')
            for r in ranked_list[:100]:
                f.write('{} {}\n'.format(q['number'][-3:], search_db.doc_id(r['id'])))
        if pool is not None:
            pool.close()

    search_db.close()
