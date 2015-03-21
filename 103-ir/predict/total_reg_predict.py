import argparse
import itertools
import os
import sys
from collections import defaultdict
from subprocess import Popen, PIPE

def process_commands():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--test', required=True)
    parser.add_argument('--predicted', required=True)
    parser.add_argument('--input', required=True)
    parser.add_argument('--output', required=True)

    return parser.parse_args()

def pairs(iterable):
    a, b = itertools.tee(iterable)
    next(b, None)

    return list(zip(a, b))[::2]

def get_results(path, *, media=False):
    q = None
    results = {}
    with open(path, 'r') as f:
        for l in f:
            if q:
                results[q] = l.rstrip().split(' ')
                if media:
                    results[q] = pairs(results[q])
                q = None
            else:
                q = l.rstrip()
    return results

def get_predicted(path):
    predicted = {}
    with open(path + '.test.name', 'r') as nf, open(path + '.result', 'r') as rf:
        for num, result in zip(nf, rf):
            predicted[num.strip()] = float(result.strip())
    return predicted

if __name__ == '__main__':
    args = process_commands()

    print('get test')
    test_dict = get_results(args.test)

    print('get input')
    input_dict = get_results(args.input, media=True)

    predicted = get_predicted(args.predicted)

    with open(args.output, 'w') as of:
        for key, lst in sorted(input_dict.items(), key=lambda x: int(x[0])):
            test_set = test_dict[key]

            lst.sort(key=lambda x: predicted[x[0]])

            of.write('{}\n'.format(key))
            of.write('{}\n'.format(
                ' '.join('{} {}'.format(x[0], x[1]) for x in lst)
                ))
