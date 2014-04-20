"""
train.py truth answer
"""
import sys

truth = {}
with open(sys.argv[1], 'r') as f:
    for l in f:
        q, d = l.split()
        if q not in truth:
            truth[q] = set()
        truth[q].add(d)

answer = {}
with open(sys.argv[2], 'r') as f:
    for l in f:
        q, d = l.split()
        if q not in answer:
            answer[q] = []
        answer[q].append(d)

MAP = 0.0
for q in sorted(truth.keys()):
    total = 1
    correct = 0
    tt = len(truth[q])
    lMAP = 0.0
    for ans in answer[q]:
        if ans in truth[q]:
            correct += 1
            lMAP += (correct / total)
            truth[q] -= set([ans])
        total += 1
    MAP += (lMAP / tt)
    print('query {}: {}'.format(q, lMAP / tt))

print('total: {}'.format(MAP / len(truth)))
