import argparse
import datetime
import itertools
import operator
import os

from collections import OrderedDict
from collections import defaultdict

def process_commands():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--truth', required=True)
    parser.add_argument('--test', required=True)
    parser.add_argument('--judge', required=True)
    parser.add_argument('--merge')

    return parser.parse_args()

def pairs(iterable):
    a, b = itertools.tee(iterable)
    next(b, None)

    return list(zip(a, b))[::2]

def get_ground(path):
    media_dict = {}
    date_dict = {}
    with open(path, 'r') as f:
        for l in f:
            num, media, d = l.rstrip().split()
            if media != 'Unknown':
                media_dict[num] = media
            if d != 'Unknown':
                date_dict[num] = datetime.datetime.strptime(d, '%Y-%m-%d').date()
    return media_dict, date_dict

def get_results(path, media=False):
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

def eval_media(gold, result):
    if len(gold) == 0:
        return 1

    total_correct = 0
    num = 0
    for x, m in result:
        if x in gold:
            num += 1
            if gold[x] == m:
                total_correct += 1

    return total_correct / num

def eval_date(gold, result):
    if len(gold) == 0:
        return 1

    total_correct = 0
    num = 0
    for x, y in itertools.combinations(map(lambda x: x[0], result), 2):
        if x in gold and y in gold:
            num += 1
            if gold[x] <= gold[y]:
                total_correct += 1

    if num == 0: return 1
    return total_correct / num

def evaluation(queries, truth, result_dict, name, func, args=[]):
    print('{}\t{}'.format(name, '\t'.join(result_dict.keys())))
    totals = [0] * len(result_dict)
    total_metrics = []
    for k in queries:
        metrics = []
        for results in result_dict.values():
            m = func(truth, results[k], *args)
            metrics.append(m)

        total_metrics.append(metrics)
        totals = map(operator.add, totals, metrics)
        print('{}\t{}'.format(k, '\t'.join(str(x) for x in metrics)))
    t = len(queries)
    print('total\t{}'.format('\t'.join(str(x / t) for x in totals)))
    return total_metrics

if __name__ == '__main__':
    args = process_commands()

    media_dict, date_dict = get_ground(args.truth)
    test_dict = get_results(args.test)
    result_dict = OrderedDict()

    for path in args.judge.split(':'):
        name = os.path.basename(path.rsplit('.', 1)[0])
        result_dict[name] = get_results(path, media=True)

    print('check results...')
    for path, d in result_dict.items():
        print('{}...'.format(path), end='')
        for key, lst in sorted(d.items(), key=lambda x: int(x[0])):
            if len(lst) != len(test_dict[key]):
                print('Failed in query {}'.format(key))
                break
            for num, _ in lst:
                if num not in test_dict[key]:
                    print('Failed in query {} num {}'.format(key, num))
                    break
            lset = {x[0] for x in lst}
            for num in test_dict[key]:
                if num not in lset:
                    print('Failed in query {} num {}'.format(key, num))
                    break
            else:
                continue
            break
        else:
            print('Passed')

    print('evaluation...')
    queries = list(sorted(test_dict, key=lambda x: int(x)))
    media_metrics = evaluation(queries, media_dict, result_dict, 'media', eval_media)
    date_metrics = evaluation(queries, date_dict, result_dict, 'date', eval_date)

    if args.merge is not None:
        perfect_result = {}
        rkeys = list(result_dict)
        # get perfect media
        for i, q in enumerate(queries):
            perfect_result[q] = result_dict[rkeys[media_metrics[i].index(max(media_metrics[i]))]][q]
        # get perfect order
        for i, q in enumerate(queries):
            perfect_order = {x[0]: i for i, x in enumerate(result_dict[rkeys[date_metrics[i].index(max(date_metrics[i]))]][q])}
            perfect_result[q].sort(key=lambda x: perfect_order[x[0]])
        with open(args.merge, 'w') as f:
            for i, q in enumerate(queries):
                f.write('{}\n'.format(q))
                f.write('{}\n'.format(
                    ' '.join('{} {}'.format(x[0], x[1]) for x in perfect_result[q])
                    ))
