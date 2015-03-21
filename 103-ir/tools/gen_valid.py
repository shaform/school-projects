import argparse
import os

from collections import OrderedDict

SEL_NUM = 100

def process_commands():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--rank', required=True)
    parser.add_argument('--test', required=True)
    parser.add_argument('--train', required=True)
    parser.add_argument('--num', type=int, default=1000)
    parser.add_argument('--output', required=True)

    return parser.parse_args()

def get_truth(path):
    truth_dict = {}
    with open(path, 'r') as f:
        for l in f:
            num, media, date = l.rstrip().split()
            truth_dict[num] = (media, date)
    return truth_dict

def get_results(path):
    q = None
    results = OrderedDict()
    with open(path, 'r') as f:
        for l in f:
            if q:
                results[q] = l.rstrip().split(' ')
                q = None
            else:
                q = l.rstrip()
    return results

def filter_dict(d, train_dict):
    for k, v in d.items():
        d[k] = list(filter(lambda x: x in train_dict, v))
    return d

def get_valid(rank_dict, train_dict):
    valid_dict = OrderedDict()
    truth_dict = {}
    for k, v in rank_dict.items():
        valid_dict[k] = v[::len(v)//SEL_NUM]
        for x in valid_dict[k]:
            if x not in truth_dict:
                truth_dict[x] = train_dict[x]
                del train_dict[x]

    return valid_dict, train_dict, truth_dict

if __name__ == '__main__':
    args = process_commands()

    train_dict = get_truth(args.train)
    test_dict = get_results(args.test)
    rank_dict = filter_dict(get_results(args.rank), train_dict)
    valid_dict, train_dict, truth_dict = get_valid(rank_dict, train_dict)
    rank_dict = filter_dict(rank_dict, train_dict)

    train_path = os.path.join(args.output, 'p2.train.txt')
    truth_path = os.path.join(args.output, 'p2.truth.txt')
    test_path = os.path.join(args.output, 'p2.test.txt')
    result_path = os.path.join(args.output, 'p2.result.txt')
    rank_path = os.path.join(args.output, 'p2.rank.txt')
    with open(train_path, 'w') as train, open(truth_path, 'w') as truth, open(test_path, 'w') as test, open(rank_path, 'w') as rank:
        for num, v in sorted(train_dict.items(), key=lambda x: int(x[0])):
            train.write('{} {} {}\n'.format(num, v[0], v[1]))
        for num, v in sorted(truth_dict.items(), key=lambda x: int(x[0])):
            truth.write('{} {} {}\n'.format(num, v[0], v[1]))
        for q, v in valid_dict.items():
            test.write('{}\n{}\n'.format(q, ' '.join(v)))
        for q, v in rank_dict.items():
            rank.write('{}\n{}\n'.format(q, ' '.join(v)))

    with open(result_path, 'w') as result:
        for q, v in valid_dict.items():
            result.write('{}\n{}\n'.format(q, ' 1 '.join(v) + ' 1'))
