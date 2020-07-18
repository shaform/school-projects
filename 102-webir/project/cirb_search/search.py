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

'''search.py: search the database'''

from multiprocessing import Pool

from . import config
from . import db
from . import query
from . import vsm

def gen_tasks(terms, search_db):
    l = len(terms)
    sk = int(l/config.PP) + 1
    for i in range(0, l, sk):
        yield terms[i:i+sk], search_db.copy()

def worker(params):
    terms, search_db = params
    doc_rank = vsm.rank_terms(terms, search_db)
    print('ranked {} terms...'.format(len(terms)))
    return doc_rank

class Search(object):
    def __init__(self, model_dir, doc_dir, db_path, rel_enabled):
        self.model_dir = model_dir
        self.doc_dir = doc_dir
        self.db_path = db_path
        self.rel_enabled = rel_enabled
        self.search_db = db.Database()
        self.search_db.open(model_dir, doc_dir, db_path)

    def search(self, text_ngrams):
        pool = Pool(config.PP)

        fb_num = 1
        if self.rel_enabled:
            print('== feedback enabled ==')
            fb_num = config.FB_IT

        print('rank documents...')
        terms = query.query_tngrams(text_ngrams, self.search_db)
        for it in range(0, fb_num):
            print('process terms...')

            ranked_list = vsm.ranked_list(pool.map(worker,
                gen_tasks(terms, self.search_db)))

            if self.rel_enabled and it+1 < fb_num:
                terms = vsm.qfeedback(ranked_list, terms, self.search_db)
                print('iteration {} done...'.format(it+1))


        pool.close()
        ranked_list = [x for x in ranked_list if x['value'] > 30]

        return [{'id': x['id'], 'value': x['value'],
            'path': self.search_db.file_list[x['id']]}
                for x in ranked_list]
