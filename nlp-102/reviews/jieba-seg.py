# -*- coding: utf-8 -*-
import jieba.posseg as pseg

from collections import defaultdict

aspect = defaultdict(int)
opinion = defaultdict(int)
with open('uniq_stripped.txt', 'r') as f:
    with open('segged.txt', 'w') as wf:
        for l in f:
            words = pseg.cut(l)
            wf.write(' '.join([ '%s/%s' % (x.flag, x.word) for x in words]).encode('utf-8'))
            for w in words:
                if w.flag == 'n':
                    aspect[w.word] += 1
                if w.flag == 'a':
                    opinion[w.word] += 1

print '---Aspect---'
for w in aspect:
    print w, aspect[w]

print '---Opinion---'
for w in opinion:
    print w, opinion[w]
