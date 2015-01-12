import math
import os
import sqlite3
import xml.etree.ElementTree as etree

from . import config

class Database(object):

    def copy(self):
        new_db = Database()
        new_db.stop_list = self.stop_list
        new_db.db_path = self.db_path
        new_db.avg_length = self.avg_length
        new_db.total_docs = self.total_docs
        new_db.file_list = self.file_list
        return new_db

    def open_simple(self):
        self.conn = sqlite3.connect(self.db_path)
        return self.conn.cursor()

    def close_simple(self, c):
        c.close()
        self.conn.close()

    def open(self, model_dir, doc_dir, db_path):
        self.model_dir = model_dir
        self.doc_dir = doc_dir
        self.db_path = db_path

        self.init()

    def init(self):
        self.stop_list = set()
        with open(os.path.join(self.model_dir, 'stop-list'), 'r') as f:
            for e in f.readlines()[:config.STOP_LIST]:
                self.stop_list.add(e.split()[0])

        self.file_list = {}
        with open(os.path.join(self.model_dir, 'file-list'), 'r') as f:
            fnum = 0
            for l in f:
                self.file_list[fnum] = os.path.join(self.doc_dir, l.strip())
                fnum += 1

        c = self.open_simple()

        c.execute('SELECT AVG(length), COUNT(length) FROM doc')
        self.avg_length, self.total_docs = c.fetchone()

        self.close_simple(c)

    def ngram(self, text):
        c = self.open_simple()
        grams = []
        for ch in text:
            c.execute('select id from vocab where vocab = ?', (ch,))
            ret = c.fetchone()
            if ret is not None:
                grams.append(str(ret[0]))
        ngram = ','.join(grams)
        self.close_simple(c)
        if len(ngram) > 0:
            return ngram
        else:
            return None

    def doc_by_ngram(self, ngram):
        c = self.open_simple()
        c.execute('select doc_id, length, count from iindex, doc where ngram = ?'
                ' and doc_id = doc.id', (ngram,))
        ret = c.fetchall()
        self.close_simple(c)
        return ret

    def doc_vec(self, doc_id):
        c = self.open_simple()
        print('let try')
        c.execute('select ngram, count from iindex where doc_id = ?', (doc_id,))
        print('let try done')
        vec = {}
        for ngram, count in c.fetchall():
            vec[ngram] = count
        self.close_simple(c)
        return vec

    def ngram_df(self, ngram):
        c = self.open_simple()
        c.execute('select count from df where ngram = ?', (ngram,))
        ret = c.fetchone()
        self.close_simple(c)
        if ret is None:
            return 0.000000001
        return ret[0]

    def ngram_idf(self, ngram):
        return math.log(self.total_docs/self.ngram_df(ngram))
