import sys

from collections import defaultdict

SELECTED_OP = [
'i',
'eng',
'a']

SELECTED_AP = [
'n',
'eng',
]

SELECTED_OT = [
'eng',
]

# get training for string search
train_txt = ''
with open('data/207884_hotel_training.txt', 'r') as f:
    train_txt = f.read()

# load all opinion words
op_dict = set()
with open('data/NTUSD_negative_unicode.txt', 'r') as f:
    for l in f:
        op_dict.add(l.strip())
with open('data/NTUSD_positive_unicode.txt', 'r') as f:
    for l in f:
        op_dict.add(l.strip())
with open('data/positive-words.txt', 'r') as f:
    for l in f:
        op_dict.add(l.strip())
with open('data/negative-words.txt', 'r') as f:
    for l in f:
        op_dict.add(l.strip())

# load stop words
stop_words = set()
with open('data/stop_words', 'r') as f:
    for l in f:
        if not l.startswith('#'):
            stop_words.add(l.strip())

d = defaultdict(lambda : defaultdict(int))

# jieba input
with open('data/segged.txt', 'r') as f:
    for l in f:
        tks = l.strip().split(' ')
        for tk in tks:
            xs = tk.split('/')
            if len(xs) == 2 and xs[1] not in stop_words:
                weight = 1
                if xs[1] in op_dict and xs[0] in SELECTED_OP:
                    weight = 2
                d[xs[0]][xs[1]] += weight

# jieba input #2
with open('data/selection_output.txt', 'r') as f:
    sep = True
    for l in f:
        l = l.strip()
        if sep:
            d['n'][l] += 1
            if l == '':
                sep = False
        else:
            d['a'][l] += 1

# standford input
with open('data/tc_selected', 'r') as f:
    for l in f:
        tks = l.strip().split(' ')
        for tk in tks:
            xs = tk.split('#')
            if len(xs) == 2 and (xs[1] == 'NN' or xs[1] == 'VA'):
                weight = 2
                if xs[0] in op_dict:
                    weight = 4
                if xs[1] == 'NN':
                    d['n'][xs[0]] += weight
                elif xs[1] == 'VA':
                    d['a'][xs[0]] += weight

# CKIP input

pos_d = defaultdict(lambda : defaultdict(int))
neg_d = defaultdict(lambda : defaultdict(int))
uniq_d = defaultdict(lambda : defaultdict(int))

with open('data/negative_seg.txt', 'r') as f:
    for l in f:
        tks = l.strip().split(' ')
        for tk in tks:
            xs = tk.strip().split('(')
            if len(xs) == 2 and (xs[1] == 'Na)' or xs[1] == 'VH)'):
                if xs[1] == 'Na)':
                    neg_d['n'][xs[0]] += 1
                    uniq_d['n'][xs[0]] += 1
                elif xs[1] == 'VH)':
                    neg_d['a'][xs[0]] += 1
                    uniq_d['a'][xs[0]] += 1

with open('data/positive_seg.txt', 'r') as f:
    for l in f:
        tks = l.strip().split('　')
        for tk in tks:
            xs = tk.strip().split('(')
            if len(xs) == 2 and (xs[1] == 'Na)' or xs[1] == 'VH)'):
                if xs[1] == 'Na)':
                    pos_d['n'][xs[0]] += 1
                    uniq_d['n'][xs[0]] += 1
                elif xs[1] == 'VH)':
                    pos_d['a'][xs[0]] += 1
                    uniq_d['a'][xs[0]] += 1

for k in uniq_d:
    for j in uniq_d[k]:
        d[k][j] += abs(pos_d[k][j] - neg_d[k][j]) * 4

# standford input with pos/neg

#pos_d = defaultdict(lambda : defaultdict(int))
#neg_d = defaultdict(lambda : defaultdict(int))
#uniq_d = defaultdict(lambda : defaultdict(int))
#
#with open('data/neg_tagged.txt', 'r') as f:
#    for l in f:
#        tks = l.strip().split(' ')
#        for tk in tks:
#            xs = tk.strip().split('#')
#            if len(xs) == 2 and (xs[1] == 'NN' or xs[1] == 'VA'):
#                if xs[1] == 'NN':
#                    neg_d['n'][xs[0]] += 1
#                    uniq_d['n'][xs[0]] += 1
#                elif xs[1] == 'VA':
#                    neg_d['a'][xs[0]] += 1
#                    uniq_d['a'][xs[0]] += 1
#
#with open('data/neg_tagged.txt', 'r') as f:
#    for l in f:
#        tks = l.strip().split('　')
#        for tk in tks:
#            xs = tk.strip().split('#')
#            if len(xs) == 2 and (xs[1] == 'NN' or xs[1] == 'VA'):
#                if xs[1] == 'NN':
#                    pos_d['n'][xs[0]] += 1
#                    uniq_d['n'][xs[0]] += 1
#                elif xs[1] == 'VA':
#                    pos_d['a'][xs[0]] += 1
#                    uniq_d['a'][xs[0]] += 1
#
#for k in uniq_d:
#    for j in uniq_d[k]:
#        d[k][j] += abs(pos_d[k][j] - neg_d[k][j])
#
#
for k in SELECTED_OT:
    for j in list(d[k].keys()):
        if j not in op_dict:
            del d[k][j]

for k in SELECTED_OP:
    if k in SELECTED_OT:
        continue
    for j in list(d[k].keys()):
        for prefix in ['很', '非常', '一點也不', '十分', '超']:
            d[k][prefix + j] += max(min(d[k][j] / 20, 30), max(d[k][j] / 100, 1))
    for j in d[k]:
        if j in op_dict:
            d[k][j] = min(d[k][j]*2, d[k][j] + 10)

for k in d:
    for j in list(d[k].keys()):
        if j in stop_words or train_txt.find(j) == -1:
            del d[k][j]

op_tdict = defaultdict(int)
op_result = []
for k in SELECTED_OP:
    for j in d[k]:
        op_tdict[j] += d[k][j]

for k, v in op_tdict.items():
    op_result.append((v, k))

op_result.sort(reverse=True)

with open('data/opnion.txt', 'w') as f:
    for x in op_result:
        f.write('{} {}\n'.format(x[1], x[0]))

with open('data/opnion_5.txt', 'w') as f:
    for x in op_result[:500]:
        f.write('{}\n'.format(x[1]))

ap_tdict = defaultdict(int)
ap_result = []
for k in SELECTED_AP:
    for j in d[k]:
        if j not in op_tdict:
            ap_tdict[j] += d[k][j]

for k, v in ap_tdict.items():
    ap_result.append((v, k))

ap_result.sort(reverse=True)

with open('data/aspect.txt', 'w') as f:
    for x in ap_result:
        f.write('{} {}\n'.format(x[1], x[0]))

with open('data/aspect_5.txt', 'w') as f:
    for x in ap_result[:100]:
        f.write('{}\n'.format(x[1]))
