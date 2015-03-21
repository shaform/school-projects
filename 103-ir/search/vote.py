import argparse
import os

from collections import defaultdict as ddict
from collections import OrderedDict as odict

def vote(preferences): 

    # extending ranked lists
    cand_sets = []
    for i, (_, rlst) in enumerate(preferences):
        cand_sets.append(set(rlst))

    for i, (_, rlst) in enumerate(preferences):
        for j, (_, rlst2) in enumerate(preferences):
            if i == j:
                continue

            for x in rlst:
                if x not in cand_sets[j]:
                    rlst2.append(x)
                    cand_sets[j].add(x)

    print('merged lengths: {}'.format(' '.join(str(len(x)) for _, x in preferences)))

    # start voting
    candidates = cand_sets[0] 
    clst = list(candidates) 
    d = dict.fromkeys([(a,b) for a in candidates for b in candidates if a != b], 0) 
 
    for weight, relation in preferences:
        localWinnings = [(relation[index], localLoser) for index in range(len(relation) - 1) for localLoser in relation[index+1:]] 
        for pair in localWinnings: 
            d[pair] += weight 

    for a, b in d:
        if d[a, b] <= d[b, a]: 
            d[a, b] = 0 

    for (i, j, k) in [(a,b,c) for a in clst for b in clst for c in clst if a != b and a != c and b != c]: 
        indirectPathWidth = min(d[j, i], d[i,k])
        directPathWidth = d[j,k]
        if indirectPathWidth > directPathWidth:
            d[j,k] = indirectPathWidth

    
    results = []
    result_set = set()
    while len(results) < min(len(clst),100):
        print('resutls: {}, cands: {}, d: {}'.format(len(results), len(candidates), len(d)))

        r = {a for a, b in d if d[a, b] < d[b, a]}
        w = candidates - r
        results.extend(list(w))
        if len(w) == 0:
            for a in candidates:
                print('{} < {}'.format(a, ' '.join(b for b in candidates if a != b and d[a, b] < d[b, a])))
            raise 1

        candidates = r
        d = {(a,b): w for (a,b), w in d.items() if (a in candidates and b in candidates)}

    return results

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

def get_queries(query_path):
    d = odict()
    with open(query_path, 'r') as f:
        for l in f:
            q, text = l.rstrip().split(' ', 1)
            d[q] = text
    return d

def process_commands():
    parser = argparse.ArgumentParser()

    parser.add_argument('query_path')
    parser.add_argument('result_path')
    parser.add_argument('output_path')

    args = parser.parse_args()

    return args

if __name__ == '__main__':
    args = process_commands()

    queries = get_queries(args.query_path)

    result_dict = odict()
    for path in args.result_path.split(':'):
        get_results(result_dict, path)

    with open(args.output_path, 'w') as f:
        for k in queries:
            print('ranking {}'.format(k))
            prefs = []
            delta = 0.0001
            weight = 1
            for results in result_dict.values():
                prefs.append((weight, results[k]))
                weight -= delta

            f.write('{}\n{}\n'.format(k, ' '.join(vote(prefs)[:100])))
