import argparse
import datetime

def process_commands():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--train', required=True)
    parser.add_argument('--test', required=True)
    parser.add_argument('--feature', required=True)
    parser.add_argument('--output', required=True)

    return parser.parse_args()


def get_results(path):
    q = None
    results = set()
    with open(path, 'r') as f:
        for l in f:
            if q:
                results.update(l.rstrip().split(' '))
                q = None
            else:
                q = l.rstrip()
    return results

if __name__ == '__main__':
    args = process_commands()

    print('loading data')
    dates = {}
    with open(args.train, 'r') as f:
        for l in f:
            num, _, date = l.split(' ')
            dates[num] = datetime.datetime.strptime(date.rstrip(), '%Y-%m-%d').date()

    nums = list(dates)
    nums.sort(key=lambda x: dates[x])
    for i, x in enumerate(nums):
        dates[x] = 1000*i/len(nums)

    test_docs = get_results(args.test)

    print('outputing')
    with open(args.feature, 'r') as f, open(args.output, 'w') as of, open(args.output + '.test', 'w') as tf, open(args.output + '.test.name', 'w') as nf:
        for l in f:
            num, features = l.split(' ', 1)
            if num in dates:
                of.write('{} {}'.format(dates[num], features))
            if num in test_docs:
                tf.write('1 {}'.format(features))
                nf.write('{}\n'.format(num))
