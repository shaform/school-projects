import argparse
import os
import sys
from collections import defaultdict

def process_commands():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--train')
    parser.add_argument('--test')
    parser.add_argument('--feature')
    parser.add_argument('--rank')
    parser.add_argument('--output')
    parser.add_argument('--num', type=int, default=1000)

    return parser.parse_args()


def get_results(path):
    q = None
    results = {}
    with open(path, 'r') as f:
        for l in f:
            if q:
                results[q] = l.rstrip().split(' ')
                q = None
            else:
                q = l.rstrip()
    return results


if __name__ == '__main__':
    args = process_commands()

    feature_dict = {}
    print('loading data')
    with open(args.feature, 'r') as f:
        for l in f:
            num, features = l.split(' ', 1)
            feature_dict[num] = features

    print('loading test')
    test_dict = get_results(args.test)
    print('outputing test')
    for key, lst in test_dict.items():
        path = os.path.join(args.output, '{}.test'.format(key))
        with open(path, 'w') as f, open(path + '.name', 'w') as nf:
            for num in lst:
                f.write('1 {}'.format(feature_dict[num]))
                nf.write('{}\n'.format(num))

    print('check rankings')
    for key, lst in test_dict.items():
        test_dict[key] = set(lst)

    rank_dict = get_results(args.rank)
    for key, tset in sorted(test_dict.items(), key=lambda x: int(x[0])):
        lst = rank_dict[key]
        for idx, doc in enumerate(lst):
            if doc in tset:
                tset.remove(doc)
            if len(tset) == 0:
                print('Query {} got in {}'.format(key, idx+1))
                break
        else:
            print('Query {} not got'.format(key))
    
    extract_dict = defaultdict(set)
    for key, lst in rank_dict.items():
        for x in lst[:args.num]:
            extract_dict[x].add(key)

    output_dict = defaultdict(list)
    print('outputing')
    with open(args.train, 'r') as f:
        for l in f:
            num, media, _ = l.split(' ')
            if num in extract_dict:
                for key in extract_dict[num]:
                    output_dict[key].append((media, feature_dict[num]))
    for key, lst in output_dict.items():
        with open(os.path.join(args.output, key), 'w') as f:
            for media, features in lst:
                f.write('{} {}'.format(media, features))
