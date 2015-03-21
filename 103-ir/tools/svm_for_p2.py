import argparse
import datetime
import os
import sys
from collections import defaultdict

def process_commands():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--train', required=True)
    parser.add_argument('--test', required=True)
    parser.add_argument('--feature', required=True)
    parser.add_argument('--rank', required=True)
    parser.add_argument('--output', required=True)
    parser.add_argument('--num', type=int, default=1000)
    parser.add_argument('--reg', action='store_true', default=False)

    return parser.parse_args()


def get_results(path, num=None):
    q = None
    results = {}
    with open(path, 'r') as f:
        for l in f:
            if q:
                results[q] = set(l.rstrip().split(' ')[:num])
                q = None
            else:
                q = l.rstrip()
    return results

def concat(f1, f2):
    vf1 = f1.strip().split(' ')
    vf2 = f2.strip().split(' ')
    vlen = len(vf1)

    return '{} {}'.format(f1.rstrip(), ' '.join('{}:{}'.format(int(k)+vlen, v) for k, v in map(lambda x: x.split(':'), vf2)))

def output_train(path, nums, features, dates, reg):
    with open(path, 'w') as f:
        nums = [x for x in nums if x in dates]
        nums.sort(key=lambda x: dates[x])
        if not reg:
            for x in range(len(nums)):
                for y in range(x,len(nums)):
                    xn, yn = nums[x], nums[y]
                    f.write('{} {}\n'.format(0, concat(features[xn], features[yn])))
                    f.write('{} {}\n'.format(1, concat(features[yn], features[xn])))
        else:
            for i, n in enumerate(nums):
                f.write('{} {}'.format(1000*i/len(nums), features[n]))


def output_test(path, nums, features, reg):
    with open(path, 'w') as f, open(path + '.name', 'w') as nf:
        nums = list(nums)
        if not reg:
            for x in range(len(nums)):
                for y in range(x+1, len(nums)):
                    f.write('1 {}\n'.format(concat(features[nums[x]], features[nums[y]])))
                    nf.write('{},{}\n'.format(nums[x], nums[y]))
        else:
            for n in nums:
                f.write('1 {}'.format(features[n]))
                nf.write('{}\n'.format(n))

if __name__ == '__main__':
    args = process_commands()

    print('loading features')
    feature_dict = {}
    with open(args.feature, 'r') as f:
        for l in f:
            num, features = l.split(' ', 1)
            feature_dict[num] = features

    print('loading dates')
    dates = {}
    with open(args.train, 'r') as f:
        for l in f:
            num, _, date = l.split(' ')
            dates[num] = datetime.datetime.strptime(date.rstrip(), '%Y-%m-%d').date()

    print('loading data')
    rank_dict = get_results(args.rank, args.num)
    test_dict = get_results(args.test)
    
    print('outputing')
    for key, lst in sorted(rank_dict.items(), key=lambda x: int(x[0])):
        print('output for ' + key)
        for x in list(lst):
            if x not in dates:
                lst.remove(x)
        output_train(os.path.join(args.output, key), nums=lst, features=feature_dict, dates=dates, reg=args.reg)
        output_test(os.path.join(args.output, key + '.test'), nums=test_dict[key], features=feature_dict, reg=args.reg)
