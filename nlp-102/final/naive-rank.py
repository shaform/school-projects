import argparse
import json
import re
import sys

from collections import defaultdict

class NaiveRanker(object):
    def __init__(self, fpos, fneg, weight=0.85):
        self.wdict = defaultdict(float)
        for l in fpos:
            l = l.strip()
            if l != '':
                self.wdict[l] += weight
        for l in fneg:
            l = l.strip()
            if l != '':
                self.wdict[l] -= 1

    def rank(self, text):
        score = 0
        for t, w in self.wdict.items():
            score += text.count(t)*w
        return score
if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('-j', type=argparse.FileType('r'), required=True)
    parser.add_argument('-p', type=argparse.FileType('r'), required=True)
    parser.add_argument('-n', type=argparse.FileType('r'), required=True)
    parser.add_argument('-o', type=argparse.FileType('w'), required=True)
    parser.add_argument('-a', action='store_true')
    args = parser.parse_args()

    ranker = NaiveRanker(args.p, args.n)
    am = 0
    for item in json.load(args.j):
        text = item['cn']
        score = ranker.rank(text)
        if score > 0:
            polarity = 1
        else:
            polarity = 2
        op_dict = defaultdict(float)
        for tw, cn in zip(item['tw_ap'], item['cn_ap']):
            for m in re.finditer(cn, text):
#                score = ranker.rank(re.split('[,.!，。！]', text[m.start():])[0])
                score = ranker.rank(re.split('[.!。！]', text[m.start():])[0])
                op_dict[tw] += score

        pos_aps, neg_aps = [], []
        for name, score in op_dict.items():
            if score > 0:
                pos_aps.append(name)
            elif score < 0:
                neg_aps.append(name)
            elif polarity == 1:
                pos_aps.append(name)
                am += 1
            else:
                neg_aps.append(name)
                am += 1
        if args.a:
            if len(neg_aps) > len(pos_aps):
                polarity = 2
            elif len(neg_aps) < len(pos_aps):
                polarity = 1

        args.o.write('{}\n{}\n{}\n{}\n'.format(item['id'],
            '\t'.join(pos_aps),
            '\t'.join(neg_aps),
            polarity))
    print('totally {} ambigious terms'.format(am))
