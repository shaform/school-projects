# concat grams and vectors
import argparse
import numpy as np
from collections import defaultdict

def process_commands():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('vector_path')
    parser.add_argument('grams_path')
    parser.add_argument('output_path')

    return parser.parse_args()

def get_vector(l):
    tokens = l.strip().split()
    vector = [float(x) for x in tokens[1:]]
    return tokens[0][4:], vector

if __name__ == '__main__':
    args = process_commands()

    print('transform data')
    with open(args.vector_path, 'r') as vf, open(args.grams_path, 'r') as gf, open(args.output_path, 'w') as of:
        vtk = {tk[0]: tk[1:] for tk in (l.rstrip().split(' ') for l in vf)}
        gtk = {tk[0]: tk[1:] for tk in (l.rstrip().split(' ') for l in gf)}

        for key in sorted(vtk, key=int):
            of.write('{} {} '.format(key, ' '.join(vtk[key])))
            vlen = len(vtk[key])
            of.write('{}\n'.format(' '.join(
                '{}:{}'.format(int(key)+vlen, value) for key, value in map(lambda x: x.split(':'), gtk[key])
                )))
