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
            if x not in db.stop_list:
                t.append(db.ngram(x))
        for i in range(len(t)-1):
            x = ','.join(t[i:i+2])
            if x not in db.stop_list:
                ngrams.append(x)
        ngrams.extend(t)
    return ngrams

def parse_queries(queries, db):
    processed = []
    for query in queries:
        p_query = dict(query)
        q = defaultdict(int)

        if config.Q_TITLE:
            for entry in ngram(query['title'], db):
                q[entry] += 1

        if config.Q_QUESTION:
            for entry in ngram(query['question'][2:], db):
                q[entry] += 1

        if config.Q_NARRATIVE:
            texts = query['narrative'].replace(
                    '相關文件內容', '').replace(
                    '應說明', '').replace(
                    '應包括', '').replace(
                    '應列舉', '').replace(
                    '應敘述', '').replace(
                    '包括', '').replace(
                    '主要應', '').split('。')
            texts = [x for x in texts if x]
            if len(texts) > 1 and texts[-1].find('不相關') != -1:
                text = '。'.join(texts[:-1])
            else:
                text = '。'.join(texts)

            for entry in ngram(text, db):
                q[entry] += 1

        if config.Q_CONCEPTS:
            for w in strip_concepts(query['concepts']):
                if len(w) > 2:
                    q[db.ngram(w)] += config.Q_C_W
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

def collect_ngrams(queries):
    ngrams = set()
    for query in queries:
        for w in query['vector'].keys():
            ngrams.add(w)
    return ngrams

def process_terms(ts, db):
    terms = []
    for t in ts.keys():
        if t not in db.idf:
            db.collect_idfs([t])
        terms.append({
            'ngram': t,
            'tf': ts[t],
            'idf': db.idf[t],
            })
    return terms


if __name__ == '__main__':
    if len(sys.argv) == 2:
        queries = parse_query(sys.argv[1])
        words = collect_words(queries)
