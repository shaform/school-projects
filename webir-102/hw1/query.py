"""query.py
Parse query xml files.
"""
from collections import defaultdict
import re
import sys
import xml.etree.ElementTree as etree

import config

def parse_xml(xml_path):
    tree = etree.parse(xml_path)
    root = tree.getroot()
    queries = []
    for child in root:
        query = {
                'number': child.find('number').text.strip('\n'),
                'title': child.find('title').text.strip('\n'),
                'question': child.find('question').text.strip('\n'),
                'narrative': child.find('narrative').text.strip('\n'),
                'concepts': child.find('concepts').text.strip('\n'),
                }
        queries.append(query)
    return queries

def ngram(text, db):
    words = re.split('\W', text)
    ngrams = []
    for w in words:
        t = []
        for x in w:
            t.append(db.ngram(x))
        for i in range(len(t)-1):
            ngrams.append(','.join(t[i:i+2]))
        ngrams.extend(t)
    return ngrams

def parse_queries(queries, db):
    processed = []
    for query in queries:
        p_query = dict(query)
        q = defaultdict(int)

        if config.Q_CONCEPTS:
            for w in strip_concepts(query['concepts']):
                if len(w) > 2:
                    q[db.ngram(w)] += 1
                for entry in ngram(w, db):
                    q[entry] += 1

        p_query['vector'] = q
        processed.append(p_query)

    return processed

def strip_concepts(text):
    for w in text.strip('。 \t\n').split('、'):
        if '、' not in w and '。' not in w:
            yield w

def collect_words(queries):
    words = set()
    for query in queries:
        for w in strip_concepts(query['concepts']):
            if len(w) > 2:
                words.add(w)
    return words

if __name__ == '__main__':
    if len(sys.argv) == 2:
        queries = parse_query(sys.argv[1])
        words = collect_words(queries)
