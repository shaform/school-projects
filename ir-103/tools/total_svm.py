import argparse

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

    train_dict = {}
    print('loading data')
    with open(args.train, 'r') as f:
        for l in f:
            num, media, _ = l.split(' ')
            train_dict[num] = media

    test_docs = get_results(args.test)

    print('outputing')
    with open(args.feature, 'r') as f, open(args.output, 'w') as of, open(args.output + '.test', 'w') as tf, open(args.output + '.test.name', 'w') as nf:
        for l in f:
            num, features = l.split(' ', 1)
            if num in train_dict:
                of.write('{} {}'.format(train_dict[num], features))
            if num in test_docs:
                tf.write('1 {}'.format(features))
                nf.write('{}\n'.format(num))
