import argparse
import itertools
import json
import os

from collections import OrderedDict
from collections import defaultdict

def process_commands():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--which', required=True)
    parser.add_argument('--output', required=True)

    return parser.parse_args()

def pairs(iterable):
    a, b = itertools.tee(iterable)
    next(b, None)

    return list(zip(a, b))[::2]

def get_results(path, media=False):
    q = None
    results = OrderedDict()
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

if __name__ == '__main__':
    args = process_commands()

    with open(args.which, 'r') as f:
        config = json.load(f)

    # load all results
    result_dict = {}
    queries = None
    for item in config.values():
        for path in item.values():
            if path not in result_dict:
                t = result_dict[path] = get_results(path, media=True)
                if queries is None:
                    queries = list(t.keys())

    perfect_result = {}
    for q in queries:
        if q in config['media']:
            t = result_dict[config['media'][q]][q]
        else:
            t = result_dict[config['media']['default']][q]
        perfect_result[q] = t

    for q in queries:
        if q in config['date']:
            t = result_dict[config['date'][q]][q]
        else:
            t = result_dict[config['date']['default']][q]

        perfect_order = {x[0]: i for i, x in enumerate(t)}

        perfect_result[q].sort(key=lambda x: perfect_order[x[0]])

    with open(args.output, 'w') as f:
        for i, q in enumerate(queries):
            f.write('{}\n'.format(q))
            f.write('{}\n'.format(
                ' '.join('{} {}'.format(x[0], x[1]) for x in perfect_result[q])
                ))
