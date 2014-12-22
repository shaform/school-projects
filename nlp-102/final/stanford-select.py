import sys

from collections import defaultdict


op_dict = defaultdict(int)

# standford input
for l in sys.stdin:
    tks = l.strip().split(' ')
    for tk in tks:
        xs = tk.split('#')
        if len(xs) == 2 and xs[1] == 'VA':
            op_dict[xs[0]] += 1

for k, v in sorted(list(op_dict.items()), key=lambda x: x[1], reverse=True):
    print(k, v)
