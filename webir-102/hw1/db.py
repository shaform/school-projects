import os
import sqlite3

class Database(object):
    MEMORY = ':memory:'
    def open(self, model_dir, db_path=MEMORY):
        self.model_dir = model_dir
        self.conn = sqlite3.connect(db_path)
        self.stop_list = {}

    def close(self):
        self.conn.close()

    def ngram(self, text):
        c = self.conn.cursor()
        grams = []
        for ch in text:
            c.execute('select id from vocab where vocab = ?', ch)
            grams.append(str(c.fetchone()[0]))
        ngram = ','.join(grams)
        c.close()
        return ngram
    
    def build_index(self):
        c = self.conn.cursor()

        print('== build vocab dictionary...\n')
        c.execute('create table vocab (id INTEGER PRIMARY KEY, vocab TEXT UNIQUE)')
        with open(os.path.join(self.model_dir, 'vocab.all'), 'r') as f:
            next(f)
            c.executemany('insert into vocab(vocab) values (?)', 
                    ([l[:-1]] for l in f))

        print('== create doc mapping...\n')
        c.execute('create table doc (id INTEGER PRIMARY KEY, path TEXT, length INTEGER)')
        with open(os.path.join(self.model_dir, 'file-list'), 'r') as f:
            with open(os.path.join('data', 'docs_count'), 'r') as cf:
                c.executemany('insert into doc(path, length) values (?,?)', 
                        zip((l[len('./CIRB010/'):-1] for l in f), (int(l) for l in cf)))

        self.conn.commit()
        c.close()

    def build_doc_index(self, ngrams):
        c = self.conn.cursor()

        print('== build doc index...\n')
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
                        if ngram in ngrams and ngram not in self.stop_list:
                            yield ngram, int(doc_id)+1, int(count)
                        num -= 1
            c.executemany('insert into iindex(ngram, doc_id, count) values (?,?,?)', 
                    gen_index())

        print('== build additional index...\n')
        with open(os.path.join('data', 'words_index'), 'r') as f:
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

        self.conn.commit()
        c.close()
