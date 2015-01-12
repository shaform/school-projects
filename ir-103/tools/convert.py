# convert from inverted_index to doc features
import argparse
from collections import defaultdict
from math import log

def process_commands():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('inverted_path')
    parser.add_argument('output_path')
    parser.add_argument('--idf', action='store_true', default=False)

    return parser.parse_args()

DOC_NUM = 102005

def gen_index(f, use_idf):
    num = 0
    gram_num = 0
    for l in f:
        if num <= 0:
            _, num = l.split()
            num = int(num)
            gram_num += 1
            idf = log(DOC_NUM/num)
        else:
            doc_id, count = l.split()
            if use_idf:
                count = log(1+int(count))*idf
            yield gram_num, int(doc_id), count
            num -= 1

P1 = 727840

if __name__ == '__main__':
    args = process_commands()
    docs = defaultdict(lambda: defaultdict(int))
    print('loading data')
    with open(args.inverted_path, 'r') as f:
        idx = 0
        for gram_num, doc_id, count in gen_index(f, args.idf):
            docs[doc_id][gram_num] = count
            idx += 1
            if idx % P1 == 0:
                print(idx/P1, '% processed')

    print('transform data')
    with open(args.output_path, 'w') as f:
        for i, item in sorted(docs.items(), key=lambda x: int(x[0])):
            f.write('{} {}\n'.format(i, ' '.join(
                '{}:{}'.format(key, value) for key, value in sorted(item.items(), key=lambda x: x[0])
                )))
