"""vector space model"""
import math
import random

import config

def sim(v1, v2, db):
    """Compute similarity between vector v1 & v2"""

    if config.OKAPI_BM25:
        return okapi_sim(v1, v2, db)
    else:
        return cossim(db.doc_vec(v1['id']), v2['vector'])

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
