"""
index_word.py WORD-LIST FILE-LIST CIRB010-DIR
"""
import re
import sys
import os.path

word_list = []
with open(sys.argv[1], 'r') as f:
    for l in f:
        word_list.append([l[:-1]])

file_list = []
with open(sys.argv[2], 'r') as f:
    for l in f:
        file_list.append(l[len('./CIRB010/'):-1])

f_id = 1
for fn in file_list:
    with open(os.path.join(sys.argv[3], fn)) as f:
        content = ''.join(f.read().splitlines())
        for this_word in word_list:
            c = content.count(this_word[0])
            if c > 0:
                this_word.append('{}:{}'.format(f_id, c))
    f_id += 1

for this_word in word_list:
    if len(this_word) > 1:
        print(' '.join(this_word))
