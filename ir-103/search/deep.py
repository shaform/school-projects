import argparse
import os
import numpy as np

from collections import defaultdict

from scipy.spatial.distance import cdist

def process_commands():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('vector_path')
    parser.add_argument('sentence_path')
    parser.add_argument('output_path')
    parser.add_argument('--n', type=int, default=100)
    return parser.parse_args()


def get_vector(l):
    tokens = l.strip().split()
    vector = np.array([float(x) for x in tokens[1:]])
    return tokens[0][4:], vector

if __name__ == '__main__':
    args = process_commands()

    print('detect short sentences')
    short_sen = set()
    nums = 0
    with open(args.sentence_path, 'r') as f:
        for l in f:
            tokens = [x for x in l.strip().split() if x not in '「」，。！：；『』、']
            if len(tokens) <= 2:
                item = tokens[0]
                if item.startswith('@@@@') and 't' not in item:
                    short_sen.add(item[4:])
            nums += 1
            if nums % 100000 == 0:
                print('{} processed'.format(nums))


    print('load vectors')
    sen_vectors, query_vectors = [], []
    sen_entries, query_entries = [], []
    nums = 0
    short_nums = 0
    with open(args.vector_path, 'r') as f:
        for l in f:
            if l.startswith('@@@@'):
                item, vector = get_vector(l)
                doc_id, t = item.split('-')
                if item not in short_sen:
                    sen_entries.append((doc_id, t))
                    sen_vectors.append(vector)
                else:
                    short_nums += 1
            elif l.startswith('!!!!'):
                item, vector = get_vector(l)
                query_entries.append(item)
                query_vectors.append(vector)
            nums += 1
            if nums % 100000 == 0:
                print('{} processed'.format(nums))
    print('{} short vectors detected'.format(len(short_sen)))
    print('{} short vectors processed'.format(short_nums))

    query_vectors = np.array(query_vectors)
    sen_vectors = np.array(sen_vectors)


    print('process queries')
    with open(args.output_path, 'w') as f:

        print('computing matrix')
        d_matrix = cdist(query_vectors, sen_vectors, 'cosine')
        for q, q_dists in zip(query_entries, d_matrix):
            print('process query', q)
            calc = [(entry, 1-dist) for entry, dist in zip(sen_entries, q_dists)]
            calc.sort(key=lambda x: x[1], reverse=True)

            print('compute highest docs')
            idx = 0
            d = defaultdict(float)
            while len(d) < args.n and idx < len(calc):
                item, dist = calc[idx]
                d[item[0]] += dist
                idx += 1

            print('output')
            f.write('{}\n'.format(q))
            docs = []
            for key, value in sorted(d.items(), key=lambda x: x[1], reverse=True):
                print(key, value)
                docs.append(key)

            f.write('{}\n'.format(' '.join(docs)))
