import argparse
import datetime
import itertools
import math
import os

def process_commands():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--input', required=True)
    parser.add_argument('--output', required=True)
    parser.add_argument('--feature', required=True)
    parser.add_argument('--train', required=True)
    parser.add_argument('--rank', required=True)
    parser.add_argument('--num', type=int, default=1000)
    parser.add_argument('--k', type=int, default=1)
    parser.add_argument('--media', action='store_true', default=False)
    parser.add_argument('--all', action='store_true', default=False)

    return parser.parse_args()

def pairs(iterable):
    a, b = itertools.tee(iterable)
    next(b, None)

    return list(zip(a, b))[::2]

def average_date(dates):
    return datetime.date.fromordinal(int(sum(map(lambda x: x.toordinal(), dates))/len(dates)))

def get_results(path, *, media=False, maxi=None):
    q = None
    results = {}
    with open(path, 'r') as f:
        for l in f:
            if q:
                results[q] = l.rstrip().split(' ')
                if media:
                    results[q] = pairs(results[q])
                if maxi:
                    results[q] = results[q][:maxi]
                q = None
            else:
                q = l.rstrip()
    return results

def get_dates(path):
    date_dict = {}
    media_dict = {}
    with open(path, 'r') as f:
        for l in f:
            num, media, d = l.rstrip().split()
            date_dict[num] = datetime.datetime.strptime(d, '%Y-%m-%d').date()
            media_dict[num] = media
    return date_dict, media_dict

def get_features(path, docs, get_all=False):
    feature_dict = {}
    with open(path, 'r') as f:
        for l in f:
            num, features = l.split(' ', 1)
            if get_all or num in docs:
                features = map(lambda x: x.split(':'), features.rstrip().split())
                feature_dict[num] = {key: float(value) for key, value in features}
    print('start normalizing features')
    for key, features in feature_dict.items():
        to_div = math.sqrt(sum(map(lambda x: x*x, features.values())))
        for fkey in features:
            features[fkey] /= to_div

    return feature_dict

def cosine(a, b):
    total = 0
    for x in a:
        if x in b:
            total += a[x] * b[x]
    return total

def get_computed_date(num, rank_lst, *, features, k, dates, media):
    cosine_dict = {}
    for x in rank_lst:
        cosine_dict[x] = cosine(features[num], features[x])
    rank_lst.sort(key=lambda x: cosine_dict[x], reverse=True)
    return average_date(list(dates[x] for x in rank_lst[:k])), media[rank_lst[0]]

def get_computed_dates(lst, rank_lst, *, features, k, dates, media):
    average_dates = {}
    k_media = {}
    for x in lst:
        d, m = get_computed_date(x[0], rank_lst, features=features, k=k, dates=dates, media=media)
        average_dates[x[0]] = d
        k_media[x[0]] = m

    return average_dates, k_media

if __name__ == '__main__':
    args = process_commands()

    print('get ranked list')
    rank_dict = get_results(args.rank, maxi=args.num)
    docs = {num for d in rank_dict.values() for num in d}

    print('get input')
    input_dict = get_results(args.input, media=True)

    for d in input_dict.values():
        for num, _ in d:
            docs.add(num)

    print('get features')
    feature_dict = get_features(args.feature, docs, args.all)

    print('get train')
    date_dict, media_dict = get_dates(args.train)

    print('filter rank')
    if args.all:
        all_list = list(date_dict.keys())
        for key in rank_dict:
            rank_dict[key] = all_list
    else:
        for key in rank_dict:
            rank_dict[key] = list(filter(lambda x: x in date_dict, rank_dict[key]))

    print('start knn for k={}'.format(args.k))
    with open(args.output, 'w') as of:
        for key, lst in sorted(input_dict.items(), key=lambda x: int(x[0])):
            print('knn for {}'.format(key))
            average_dates, k_media = get_computed_dates(lst, rank_dict[key], features=feature_dict, k=args.k, dates=date_dict, media=media_dict)
            print('sorting')
            lst.sort(key=lambda x: average_dates[x[0]])
            of.write('{}\n'.format(key))
            if args.media:
                of.write('{}\n'.format(
                    ' '.join('{} {}'.format(x[0], k_media[x[0]]) for x in lst)
                    ))
            else:
                of.write('{}\n'.format(
                    ' '.join('{} {}'.format(x[0], x[1]) for x in lst)
                    ))
