#!/usr/bin/env python
import fileinput
import re

def add_set(d, k, e):
    '''Add e to the set of d[k]'''
    if k in d:
        d[k].add(e)
    else:
        d[k] = set([e])

# create the mapping
rline = re.compile('^(\S+)\s+(\S+)$')
mapping = dict()
for line in fileinput.input():
    m = rline.match(line)
    if m is not None:
        char = m.group(1)
        mapping[char] = set([char])
        for zuin in m.group(2).split('/'):
            add_set(mapping, zuin[:2], char)
    else:
        raise Exception('wrong')


# output the mapping
for k in mapping:
    print '%s %s' % (k, ' '.join(mapping[k]))
