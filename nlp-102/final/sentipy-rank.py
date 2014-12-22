# -*- coding: utf-8 -*-
"""
use sentipy to rank
"""
import argparse
import json
import re
import sys
import time

import sentipy

from collections import defaultdict

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('-j', type=argparse.FileType('r'))
    parser.add_argument('-i', type=argparse.FileType('r'))
    parser.add_argument('-p', type=argparse.FileType('r'), required=True)
    parser.add_argument('-n', type=argparse.FileType('r'), required=True)
    parser.add_argument('-o', type=argparse.FileType('w'), required=True)
    parser.add_argument('-e', type=argparse.FileType('r'))
    args = parser.parse_args()

    senti = sentipy.senti.senti()

    rP = re.compile(r'[a-z]+\(([^-]+)-\d+, ([^-]+)-\d+\)')

    exist_set = set()
    if args.e is not None:
        while True:
            line = args.e.readline()
            if not line:
                break
            exist_set.add(int(line))
            args.e.readline()
            args.e.readline()
            args.e.readline()

    if args.j is not None:
        jobj = json.load(args.j)
        all_dict = defaultdict(int)
        for l in args.p:
            l = l.strip()
            if l != '':
                all_dict[l.decode('utf-8')] += 1
        for l in args.n:
            l = l.strip()
            if l != '':
                all_dict[l.decode('utf-8')] -= 1
        for item in jobj:
            if item['id'] in exist_set:
                continue
            while True:
                try:
                    res = senti.sentiFLY(item['cn'])
                    break
                except:
                    print('error connection')
                    time.sleep(1)
            if res[-1] < 0:
                polarity = 2
            else:
                polarity = 1
            ap_dict = defaultdict(float)
            for wnd in rP.findall(res[-4].decode('utf-8')):
                if wnd[0] in all_dict:
                    if wnd[1] in item['cn_ap']:
                        ap_dict[wnd[1]] += all_dict[wnd[0]]
                if wnd[1] in all_dict:
                    if wnd[0] in item['cn_ap']:
                        ap_dict[wnd[0]] += all_dict[wnd[1]]
            pos_aps, neg_aps = [], []
            for ap, pol in ap_dict.items():
                tw_ap = item['tw_ap'][item['cn_ap'].index(ap)]
                if pol > 0:
                    pos_aps.append(tw_ap)
                elif pol < 0:
                    neg_aps.append(tw_ap)

            args.o.write(u'{}\n{}\n{}\n{}\n'.format(item['id'],
                u'\t'.join(pos_aps),
                u'\t'.join(neg_aps),
                polarity).encode('utf-8'))
            args.o.flush()
    elif args.i is not None:
        for l in args.i:
            l = l.strip().decode('utf-8')
            if len(l) > 0:
                while True:
                    try:
                        res = senti.sentiFLY(l)
                        break
                    except:
                        print('error connection')
                        time.sleep(1)
                if res[-1] > 0:
                    args.o.write('1\n')
                elif res[-1] < 0:
                    args.o.write('2\n')
                else:
                    args.o.write('0\n')
                args.o.flush()
    else:
        print 'you must supply at least one input!'
        sys.exit()
    pass
