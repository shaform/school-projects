# convert from vectors to doc features
import argparse
import numpy as np
from collections import defaultdict

def process_commands():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('vector_path')
    parser.add_argument('output_path')
    parser.add_argument('--minmax', action='store_true')

    return parser.parse_args()

def get_vector(l):
    tokens = l.strip().split()
    vector = [float(x) for x in tokens[1:]]
    return tokens[0][4:], vector

if __name__ == '__main__':
    args = process_commands()

    print('loading data')
    docs = defaultdict(list)
    nums = 0
    with open(args.vector_path, 'r') as f:
        for l in f:
            if l.startswith('@@@@'):
                item, vector = get_vector(l)
                doc_id, _ = item.split('-')
                docs[doc_id].append(vector)

            nums += 1
            if nums % 100000 == 0:
                print('{} processed'.format(nums))


    print('transform data')
    with open(args.output_path, 'w') as f:
        for i, item in sorted(docs.items(), key=lambda x: int(x[0])):
            features = []
            # mean
            features.extend((key, value) for key, value in enumerate(np.mean(item,axis=0)))
            if args.minmax:
                # min
                nums = len(features)
                features.extend((key+nums, value) for key, value in enumerate(np.min(item,axis=0)))
                # max
                nums = len(features)
                features.extend((key+nums, value) for key, value in enumerate(np.max(item,axis=0)))

            f.write('{} {}\n'.format(i, ' '.join(
                '{}:{}'.format(key+1, value) for key, value in features
                )))
