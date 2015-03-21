"""vector space model"""
from . import config
from . import db
from . import query

from collections import defaultdict

def ranked_list(doc_ranks):
    doc_rank = defaultdict(float)
    for t_rank in doc_ranks:
        for d, v in t_rank.items():
            doc_rank[d] += v
    rlist = []
    for d in doc_rank:
        rlist.append({
            'id': d,
            'value': doc_rank[d]
            })
    rlist.sort(key=lambda x: x['value'], reverse=True)
    return rlist

def rank_terms(ts, db):
    doc_rank = defaultdict(float)
    for t in ts:
        for doc_id, ld, tfd in db.doc_by_ngram(t['ngram']):
            lavg = db.avg_length
            k1 = config.OK_K1
            k3 = config.OK_K3
            b = config.OK_B
            N = db.total_docs
            tfq = t['tf']
            idf = t['idf']
            doc_rank[doc_id] += (idf
                    * ( ((k1 + 1) * tfd) / (k1*((1-b)+b*(ld/lavg))+tfd) )
                    * ( ((k3+1)*tfq) / (k3+tfq) )
                    )
    return doc_rank

def qfeedback(doc_list, q, db):
    if config.FB_A != 1:
        for k in q:
            k['tf'] *= config.FB_A

    tt = defaultdict(float)

    vt = defaultdict(float)
    if config.FB_REL > 0:
        for d in doc_list[:config.FB_REL]:
            dvec = db.doc_vec(d['id'])
            for t in dvec.keys():
                if t not in db.stop_list:
                    vt[t] += dvec[t]
    for t in vt:
        tt[t] += vt[t] * config.FB_B / config.FB_REL

    vt = defaultdict(float)
    if config.FB_NREL > 0:
        for d in doc_list[-config.FB_NREL:]:
            dvec = db.doc_vec(d['id'])
            for t in dvec.keys():
                if t not in db.stop_list:
                    vt[t] -= dvec[t]
    for t in vt:
        tt[t] += vt[t] * config.FB_C / config.FB_NREL

    for k in q:
        tt[k['ngram']] += k['tf']
    return query.query_terms({key: value for (key, value) in tt.items() if value > config.FB_THRESHOLD}, db)
