import argparse
import itertools
import os
import sys
from collections import defaultdict
from subprocess import Popen, PIPE

def process_commands():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--test', required=True)
    parser.add_argument('--path', required=True)
#    parser.add_argument('--train', required=True)
#    parser.add_argument('--predict', required=True)
    parser.add_argument('--input', required=True)
    parser.add_argument('--output', required=True)
    parser.add_argument('--reg', action='store_true', default=False)

    return parser.parse_args()

def pairs(iterable):
    a, b = itertools.tee(iterable)
    next(b, None)

    return list(zip(a, b))[::2]

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

def compare_to_key(cmp_dict):
    class K:
        def __init__(self, obj, *args):
            self.obj = obj
        def __lt__(self, other):
            return (not self.obj == other.obj) and cmp_dict[self.obj][other.obj]
        def __gt__(self, other):
            return (not self.obj == other.obj) and not cmp_dict[self.obj][other.obj]
        def __eq__(self, other):
            return self.obj == other.obj
        def __le__(self, other):
            return self.obj == other.obj or cmp_dict[self.obj][other.obj]
        def __ge__(self, other):
            return self.obj == other.obj or not cmp_dict[self.obj][other.obj]
        def __ne__(self, other):
            return self.obj != other.obj
    return K

if __name__ == '__main__':
    args = process_commands()

    print('get test')
    test_dict = get_results(args.test)

    print('get input')
    input_dict = get_results(args.input, media=True)

    with open(args.output, 'w') as of:
        for key, lst in sorted(input_dict.items(), key=lambda x: int(x[0])):
            test_set = test_dict[key]

            name = os.path.join(args.path, key)
            with open('{}.result'.format(name), 'r') as rf, open('{}.test.name'.format(name), 'r') as nf:
                if not args.reg:
                    compare_dict = defaultdict(dict)
                    for nums, result in zip(nf, rf):
                        a, b = nums.strip().split(',')
                        result = result.strip() == '0'

                        if a not in test_set or b not in test_set:
                            print('wrong with {}/{} in {}'.format(a, b, key))
                        else:
                            compare_dict[a][b] = result
                            compare_dict[b][a] = not result

                    Key = compare_to_key(compare_dict)
                    lst.sort(key=lambda x: Key(x[0]))
                else:
                    reg_dict = {}
                    for num, result in zip(nf, rf):
                        reg_dict[num.strip()] = float(result)
                    lst.sort(key=lambda x: reg_dict[x[0]])

            of.write('{}\n'.format(key))
            of.write('{}\n'.format(
                ' '.join('{} {}'.format(x[0], x[1]) for x in lst)
                ))
