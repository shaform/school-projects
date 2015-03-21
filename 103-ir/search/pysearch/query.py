"""query.py
"""
from collections import defaultdict
import sys

def ngrams(text_ngrams, db):
    ngram_list = [ngram(text, db) for text in text_ngrams]
    return [x for x in ngram_list if x]

def ngram(text_ngram, db):
    t = []
    for x in text_ngram:
        v_id = db.ngram(x)
        if v_id is None:
            return None
        else:
            t.append(v_id)

    return ','.join(t)

def query_terms(tfs, db):
    terms = []
    for gram, tf in tfs.items():
        terms.append({
            'ngram': gram,
            'tf': tf,
            'idf': db.ngram_idf(gram),
            })
    return terms

def query_tngrams(text_ngrams, db):
    tfs = defaultdict(int)
    for gram in ngrams(text_ngrams, db):
        tfs[gram] += 1

    return query_terms(tfs, db)
