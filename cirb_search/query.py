#
# Copyright 2014 Yong-Siang Shih.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 3,
# as published by the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

'''query.py: parse query terms'''

import sys
from collections import defaultdict

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
