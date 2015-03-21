"""
count_doc.py FILE-LIST CIRB010-DIR
"""
import sys
import os.path
import xml.etree.ElementTree as etree

file_list = []
with open(sys.argv[1], 'r') as f:
    for l in f:
        file_list.append(l[len('./CIRB010/'):-1])

for fn in file_list:
    with open(os.path.join(sys.argv[2], fn)) as f:
        content = ''.join(f.read().splitlines())
        tree = etree.fromstring(content)
        notags = etree.tostring(tree, encoding='unicode', method='text')
        print(len(notags))
