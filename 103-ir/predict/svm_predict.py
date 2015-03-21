import argparse
import os
import sys
from collections import defaultdict
from subprocess import Popen, PIPE

def process_commands():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--test', required=True)
    parser.add_argument('--path', required=True)
    parser.add_argument('--train', required=True)
    parser.add_argument('--predict', required=True)
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

if __name__ == '__main__':
    args = process_commands()

    test_dict = get_results(args.test)

    cv_results = []
    with open(args.output, 'w') as out:
        for key, test_set in sorted(test_dict.items(), key=lambda x: int(x[0])):
            print('predict {}'.format(key))
            name = os.path.join(args.path, key)
            with open('{}.output'.format(name), 'r') as f, open('{}.test.name'.format(name), 'r') as nf:
                *_, l = f
                c, g, cv = l.split()

                cv_results.append((key, float(cv)/100))

                cmdline = '{} -c {} -g {} {} {}.model'.format(args.train, c, g, name, name)

                result =  Popen(cmdline, shell=True, stdout=PIPE, stderr=PIPE, stdin=PIPE).stdout
                for l in result:
                    print(l)

                cmdline = '{} {}.test {}.model {}.result'.format(args.predict, name, name, name)

                result =  Popen(cmdline, shell=True, stdout=PIPE, stderr=PIPE, stdin=PIPE).stdout
                for l in result:
                    print(l)

                out.write('{}\n'.format(key))
                items = []
                with open('{}.result'.format(name), 'r') as rf:
                    for num, result in zip(nf, rf):
                        num = num.strip()
                        result = result.strip()

                        if num not in test_set:
                            print('wrong with {} in {}'.format(num, key))
                        else:
                            items.append(num)
                            items.append(result)
                out.write('{}\n'.format(' '.join(items)))

    for key, cv in cv_results:
        print('{}\t{}'.format(key, cv))
