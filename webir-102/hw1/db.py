import os
import sqlite3
import xml.etree.ElementTree as etree

import config

class Database(object):
    MEMORY = ':memory:'
    def open_simple(self, db_path):
        self.conn = sqlite3.connect(db_path)

    def open(self, model_dir, doc_dir, db_path=MEMORY):
        self.model_dir = model_dir
        self.doc_dir = doc_dir
        self.conn = sqlite3.connect(db_path)
        self.stop_list = set()
        with open(os.path.join(config.P_STOP_LIST), 'r') as f:
            for e in f.readlines()[:config.STOP_LIST]:
                self.stop_list.add(e.split('|')[0])

    def close(self):
        self.conn.close()

    def ngram(self, text):
        c = self.conn.cursor()
        grams = []
        for ch in text:
            c.execute('select id from vocab where vocab = ?', (ch,))
            grams.append(str(c.fetchone()[0]))
        ngram = ','.join(grams)
        c.close()
        return ngram

    def retrieve_docs(self, vec):
        c = self.conn.cursor()
        t_docs = set()
        for term in vec.keys():
            if term not in self.stop_list:
                c.execute('select doc_id from iindex where ngram = ?', (term,))
                t_docs.update(c.fetchall())
        docs = []
        for x in t_docs:
            c.execute('select length from doc where id = ?', x)
            dlen = c.fetchone()
            docs.append({'id': x[0],
                'length': dlen[0],
                })
        c.close()
        return docs

    def doc_id(self, doc):
        c = self.conn.cursor()
        c.execute('select path from doc where id = ?', (doc,))
        xml_path = os.path.join(self.doc_dir, c.fetchone()[0])
        ret = etree.parse(xml_path).getroot().find('doc').find('id').text.lower()
        c.close()
        return ret

    def doc_vec(self, doc):
        c = self.conn.cursor()
        c.execute('select ngram, count from iindex where doc_id = ?', (doc,))
        vec = {}
        for x in c.fetchall():
            vec[x[0]] = x[1]
        c.close()
        return vec

    def ngram_df(self, ngram):
        c = self.conn.cursor()
        c.execute('select count from df where ngram = ?', (ngram,))
        ret = c.fetchone()
        c.close()
        return ret[0]
    
    def build_index(self):
        c = self.conn.cursor()

        print('== build vocab dictionary...')
        c.execute('create table vocab (id INTEGER PRIMARY KEY, vocab TEXT UNIQUE)')
        with open(os.path.join(self.model_dir, 'vocab.all'), 'r') as f:
            next(f)
            c.executemany('insert into vocab(vocab) values (?)', 
                    ([l[:-1]] for l in f))

        print('== create doc mapping...')
        c.execute('create table doc (id INTEGER PRIMARY KEY, path TEXT, length INTEGER)')
        with open(os.path.join(self.model_dir, 'file-list'), 'r') as f:
            with open(os.path.join(config.P_DOCS_COUNT), 'r') as cf:
                c.executemany('insert into doc(path, length) values (?,?)', 
                        zip((l[len('./CIRB010/'):-1] for l in f), (int(l) for l in cf)))

        self.conn.commit()
        c.close()

    def build_doc_index(self, ngrams):
        c = self.conn.cursor()

        print('== build doc index...')
        c.execute('create table iindex (ngram TEXT, doc_id INTEGER, count INTEGER)')
        c.execute('create index if not exists iidx_idx_ngram on iindex(ngram)')
        c.execute('create index if not exists iidx_idx_doc on iindex(doc_id)')
        with open(os.path.join(self.model_dir, 'inverted-index'), 'r') as f:
            def gen_index():
                num = 0
                for l in f:
                    if num <= 0:
                        ng1, ng2, num = l.split()
                        num = int(num)
                        if ng2 == '-1':
                            ngram = ng1
                        else:
                            ngram = ng1 + ',' + ng2
                    else:
                        doc_id, count = l.split()
                        if config.NOT_SKIP_NG or (ngram in ngrams and len(ngram) > 0):
                            yield ngram, int(doc_id)+1, int(count)
                        num -= 1
            c.executemany('insert into iindex(ngram, doc_id, count) values (?,?,?)', 
                    gen_index())

        print('== build additional index...')
        with open(os.path.join(config.P_WORDS_INDEX), 'r') as f:
            def gen_index():
                gc = self.conn.cursor()
                for l in f:
                    items = l.split()
                    ngram = self.ngram(items[0])
                    for item in items[1:]:
                        doc_id, count = item.split(':')
                        yield ngram, int(doc_id), int(count)
                gc.close()
            c.executemany('insert into iindex(ngram, doc_id, count) values (?,?,?)', 
                    gen_index())

        print('== build df table...')
        c.execute('create table df (ngram TEXT PRIMARY KEY, count INTEGER)')
        c.execute('insert into df(ngram, count) select ngram, count(count) from iindex group by ngram')

        self.conn.commit()
        c.close()
