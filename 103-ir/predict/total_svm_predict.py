import argparse
import os
import sys
from collections import defaultdict
from subprocess import Popen, PIPE

def process_commands():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--test', required=True)
    parser.add_argument('--predicted', required=True)
    parser.add_argument('--output', required=True)

    return parser.parse_args()

def get_results(path):
    q = None
    results = {}
    with open(path, 'r') as f:
        for l in f:
            if q:
                results[q] = set(l.rstrip().split(' '))
                q = None
            else:
                q = l.rstrip()
    return results

def get_predicted(path):
    predicted = {}
    with open(path + '.test.name', 'r') as nf, open(path + '.result', 'r') as rf:
        for num, result in zip(nf, rf):
            predicted[num.strip()] = result.strip()
    return predicted

if __name__ == '__main__':
    args = process_commands()

    test_dict = get_results(args.test)
    predicted = get_predicted(args.predicted)

    with open(args.output, 'w') as out:
        for key, test_set in sorted(test_dict.items(), key=lambda x: int(x[0])):
            items = []
            for num in test_set:
                items.append(num)
                items.append(predicted[num])
            out.write('{}\n'.format(key))
            out.write('{}\n'.format(' '.join(items)))
