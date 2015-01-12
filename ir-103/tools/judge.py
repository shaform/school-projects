import argparse
import itertools
import operator
import os

from collections import OrderedDict as odict
from collections import defaultdict as ddict

class rel_set(set):

    def __init__(self):
        super().__init__()
        self.nrel_set = set()

    def is_rel(self, v):
        return v in self

    def is_nrel(self, v):
        return v in self.nrel_set

    def add_rel(self, v):
        self.add(v)
        if self.is_nrel(v):
            self.remove_nrel(v)

    def add_nrel(self, v):
        self.nrel_set.add(v)
        if self.is_rel(v):
            self.remove_rel(v)

    def remove_rel(self, v):
        self.remove(v)

    def remove_nrel(self, v):
        self.nrel_set.remove(v)

    def rel(self):
        return self

    def nrel(self):
        return self.nrel_set

def process_commands():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('source_dir')
    parser.add_argument('query_path')
    parser.add_argument('result_path')
    parser.add_argument('--output_path')
    parser.add_argument('--judged_path')
    parser.add_argument('--merged_path')

    return parser.parse_args()

def get_queries(query_path):
    d = odict()
    with open(query_path, 'r') as f:
        for l in f:
            q, text = l.rstrip().split(' ', 1)
            d[q] = text
    return d

def get_judged(judged, judged_path):
    q = None
    with open(judged_path, 'r') as f:
        for l in f:
            if q:
                for r in l.rstrip().split(' '):
                    if r:
                        if r[0] == '@':
                            judged[q].add_nrel(r[1:])
                        else:
                            judged[q].add_rel(r)
                q = None
            else:
                q = l.rstrip()

def put_judged(keys, judged, output_path):
    with open(output_path, 'w') as f:
        for k in keys:
            f.write('{}\n'.format(k))
            f.write('{}\n'.format(' '.join(itertools.chain(
                sorted(judged[k].rel(), key=int),
                ('@' + x for x in sorted(judged[k].nrel(), key=int))
                ))))

def get_results(result_dict, result_path):
    q = None
    results = ddict(list)
    with open(result_path, 'r') as f:
        for l in f:
            if q:
                for r in l.rstrip().split(' ')[:100]:
                    if r:
                        results[q].append(r)
                q = None
            else:
                q = l.rstrip()

    name = os.path.basename(result_path.rsplit('.', 1)[0])
    result_dict[name] = results

def eval_map(gold, result):
    if len(gold) == 0:
        return 1

    dup = set()
    total, total_correct = 0, 0
    m = 0
    for x in result:
        total += 1

        if x in dup:
            continue
        else:
            dup.add(x)

        if x in gold:
            total_correct += 1
            m += total_correct / total

    return m / len(gold)

def eval_prec(gold, result, num):
    if len(gold) == 0:
        return 1

    dup = set()
    total_correct = 0
    m = 0
    for x in result[:num]:
        if x in dup:
            continue
        else:
            dup.add(x)

        if x in gold:
            total_correct += 1

    return total_correct / num

def human_judge(queries, result_dict, judged, path):
    skipped_set = set()
    for k in queries:
        skipped = False
        for name, results in result_dict.items():
            for num, doc in enumerate(results[k]):
                if doc in judged[k].rel() or doc in judged[k].nrel():
                    continue
                # get human judgement
                with open(os.path.join(path, doc), 'r') as f:
                    d = f.read()
                while True:
                    print('\n\n\n\n====================\n\n\n\n')
                    print(d)
                    print('({}:{}:{}) Is this relevance to {}? (y/n/s)'.format(name, k, num+1, queries[k]), end=' ')
                    yn = input().strip()
                    if yn == 'y':
                        judged[k].add_rel(doc)
                        break
                    elif yn == 'n':
                        judged[k].add_nrel(doc)
                        break
                    elif yn == 's':
                        skipped = True
                        break
                if skipped:
                    skipped_set.add(k)
                    break
            if skipped:
                break
    return skipped_set

def evaluation(queries, skipped, judged, result_dict, name, func, args=[], merge=None):
    print('{}\t{}'.format(name, '\t'.join(result_dict.keys())))
    totals = [0] * len(result_dict)
    for k in queries:
        metrics = []
        if k not in skipped:
            for results in result_dict.values():
                m = func(judged[k].rel(), results[k], *args)
                metrics.append(m)

            if merge is not None:
                maxi = max(zip(metrics, result_dict.values()), key=lambda x: x[0])
                mf.write('{}\n{}\n'.format(k, ' '.join(maxi[1][k][:100])))
            totals = map(operator.add, totals, metrics)
        print('{}\t{}'.format(k, '\t'.join(str(x) for x in metrics)))
    t = len(queries)-len(skipped)
    print('total\t{}'.format('\t'.join(str(x / t) for x in totals)))

if __name__ == '__main__':
    args = process_commands()

    queries = get_queries(args.query_path)

    judged = ddict(rel_set)

    if args.judged_path:
        for path in args.judged_path.split(':'):
            get_judged(judged, path)

    result_dict = odict()

    for path in args.result_path.split(':'):
        get_results(result_dict, path)

    skipped = human_judge(queries, result_dict, judged, path=args.source_dir)

    if args.output_path:
        put_judged(queries.keys(), judged, args.output_path)

    evaluation(queries, skipped, judged, result_dict, 'map', eval_map)
    evaluation(queries, skipped, judged, result_dict, 'p@5', eval_prec, args=[5])
    evaluation(queries, skipped, judged, result_dict, 'p@10', eval_prec, args=[10])

    if args.merged_path and len(skipped) == 0:
        with open(args.merged_path, 'w') as mf:
            evaluation(queries, skipped, judged, result_dict, 'map', eval_map, merge=mf)
