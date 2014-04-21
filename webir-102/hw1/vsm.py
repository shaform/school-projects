"""vector space model"""
from collections import defaultdict

import math
import random

import config
import db

def ranked_list(doc_ranks):
    doc_rank = defaultdict(float)
    for t_rank in doc_ranks:
        for d in t_rank.keys():
            doc_rank[d] += t_rank[d]
    rlist = []
    for d in doc_rank:
        rlist.append({
            'id': d,
            'value': doc_rank[d]
            })
    rlist.sort(key=lambda x: x['value'], reverse=True)
    return rlist

def okapi_rank_terms(ts, db):
    doc_rank = defaultdict(float)
    for t in ts:
        for doc_id, tfd, ld in db.doc_by_term(t['ngram']):
            lavg = config.OK_AVG_L
            k1 = config.OK_K1
            k3 = config.OK_K3
            b = config.OK_B
            N = config.OK_N
            tfq = t['tf']
            idf = t['idf']
            doc_rank[doc_id] += (idf
                    * ( ((k1 + 1) * tfd) / (k1*((1-b)+b*(ld/lavg))+tfd) )
                    * ( ((k3+1)*tfq) / (k3+tfq) )
                    )
    return doc_rank

def cos_rank_terms(ts, db):
    doc_rank = defaultdict(float)
    for t in ts:
        for doc_id, tfd, ld in db.doc_by_term(t['ngram']):
            doc_rank[doc_id] += t['tf']*t['idf']*tfd
    return doc_rank

def rank_terms(ts, db):
    if config.OKAPI_BM25:
        return okapi_rank_terms(ts, db)
    else:
        return cos_rank_terms(ts, db)

def rank_docs(e):
    ranked_list, q, search_db = e
    clean_db = False
    if not isinstance(search_db, db.Database):
        new_db = search_db[0]
        new_db.open_simple(search_db[1])
        search_db = new_db
        clean_db = True
    r_count = 0
    for d in ranked_list:
        val = sim(d, q, search_db)
        d['value'] = val
        r_count += 1
        if r_count % 10000 == 0:
            print('ranked {} docs.'.format(r_count))
            r_count = 0
    if r_count != 0:
        print('ranked {} docs.'.format(r_count))

    if clean_db:
        search_db.close()

    return ranked_list

def sim(d, q, db):
    """Compute similarity between vector v1 & v2"""

    if config.OKAPI_BM25:
        return okapi_sim(d, q, db)
    else:
        return cos_sim(db.doc_vec(d['id']), q['vector'])

def qfeedback(doc_list, q, db):
#        VR = defaultdict(int)
#        VR_len = config.FB_REL
#        VNR = defaultdict(int)
#
#        for d in doc_list[:VR_len]:
#            dvec = db.doc_vec(d['id'])
#            for t in q['vector'].keys():
#                if t in dvec:
#                    VR[t] += 1
#
#        for d in doc_list[-config.FB_NREL:]:
#            dvec = db.doc_vec(d['id'])
#            for t in q['vector'].keys():
#                if t in dvec:
#                    VNR[t] += 1
#
#        return (VR, VR_len, VNR), q
#    else:
#        return None, q

    if config.FB_A != 1:
        for k in q['vector'].keys():
            q['vector'][k] *= config.FB_A

    vt = defaultdict(float)
    for d in doc_list[:config.FB_REL]:
        dvec = db.doc_vec(d['id'])
        for t in dvec.keys():
            if t not in db.stop_list:
                vt[t] += dvec[t]
    for t in vt:
        q['vector'][t] += vt[t] * config.FB_B / config.FB_REL

    vt = defaultdict(float)
    for d in doc_list[-config.FB_NREL:]:
        dvec = db.doc_vec(d['id'])
        for t in dvec.keys():
            if t not in db.stop_list:
                vt[t] -= dvec[t]
    for t in vt:
        q['vector'][t] += vt[t] * config.FB_C / config.FB_NREL

#    return None, q
    return q



#def sim_feedback(fb, d, q, db):
#    if config.OKAPI_BM25:
#        VR, VR_len, VNR = fb
#        return okapi_sim_feedback(d, q, VR, VR_len, VNR, db)
#    else:
#        return sim(d, q, db)

def okapi_sim(d, q, db):
    lavg = config.OK_AVG_L
    k1 = config.OK_K1
    k3 = config.OK_K3
    b = config.OK_B
    N = config.OK_N
    ld = d['length']
    dvec = db.doc_vec(d['id'])
    total = 0.0
    for key in q['vector'].keys():
        if key in dvec:
            tfd = dvec[key]
            tfq = q['vector'][key]
            df = db.ngram_df(key)
            total += (math.log(N/df)
                    * ( ((k1 + 1) * tfd) / (k1*((1-b)+b*(ld/lavg))+tfd) )
                    * ( ((k3+1)*tfq) / (k3+tfq) )
                    )
    return total

#def okapi_sim_feedback(d, q, VR, VR_len, VNR, db):
#    lavg, N = config.OK_AVG_L, config.OK_N
#    k1, k3, b = config.OK_K1, config.OK_K3, config.OK_B
#    ld = d['length']
#
#    dvec = db.doc_vec(d['id'])
#    qvec = q['vector']
#
#    total = 0.0
#    for t in qvec.keys():
#        if t in dvec:
#            tfd = dvec[t]
#            tfq = qvec[t]
#            df = db.ngram_df(t)
#
#            total += (math.log( ((VR[t]+0.5)/(VNR[t]+0.5)) / ((df-VR[t]+0.5)/(N-df-VR_len+VR[t]+0.5)) )
#                    + math.log( ((k1 + 1) * tfd) / (k1*((1-b)+b*(ld/lavg))+tfd) )
#                    + math.log( ((k3+1)*tfq) / (k3+tfq) )
#                    )
#    return total

def cos_sim(v1, v2):
    """Compute cosine similarity between vector v1 & v2"""
    total = 0.0
    width_v1 = 0.0
    width_v2 = 0.0

    for key in v1.keys():
        width_v1 += v1[key] * v1[key]
        if key in v2:
            total += v1[key] * v2[key]

    for key in v2.keys():
        width_v2 += v2[key] * v2[key]

    return total/(math.sqrt(width_v1)*math.sqrt(width_v2))
