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

"""create_db.py

Parse index files and create SQLite3 database.

Usage:
    python3 indexer.py SOURCE-DIR OUTPUT-DB
"""
import os
import sys

import sqlite3

if __name__ == '__main__':
    if len(sys.argv) != 3:
        print(__doc__)
        sys.exit()

    if not os.path.isdir(sys.argv[1]):
        print('`{}` is not a directory'.format(sys.argv[1]))
        sys.exit()
    if os.path.exists(sys.argv[2]):
        print('`{}` exists'.format(sys.argv[2]))
        sys.exit()

    input_dir = os.path.abspath(sys.argv[1])
    output_path = os.path.abspath(sys.argv[2])

    conn = sqlite3.connect(output_path)
    c = conn.cursor()

    print('== build vocab dictionary...')
    c.execute('create table vocab (id INTEGER PRIMARY KEY, vocab TEXT UNIQUE)')
    with open(os.path.join(input_dir, 'vocab.all'), 'r') as f:
        c.executemany('insert into vocab(vocab) values (?)',
                ([l[:-1]] for l in f))

    print('== create doc mapping...')
    c.execute('create table doc (id INTEGER PRIMARY KEY, path TEXT, length INTEGER)')
    with open(os.path.join(input_dir, 'file-list'), 'r') as flist:
        with open(os.path.join(input_dir, 'doc-length'), 'r') as dlen:
            c.executemany('insert into doc(path, length) values (?,?)', 
                    zip((l[:-1] for l in flist),
                        (int(l) for l in dlen)))

    print('== build doc index...')
    c.execute('create table iindex (ngram TEXT, doc_id INTEGER, count INTEGER)')
    c.execute('create index if not exists iidx_idx_ngram on iindex(ngram)')
    c.execute('create index if not exists iidx_idx_doc on iindex(doc_id)')
    with open(os.path.join(input_dir, 'inverted-index'), 'r') as f:
        def gen_index():
            num = 0
            for l in f:
                if num <= 0:
                    ngram, num = l.split()
                    num = int(num)
                    ngram = ngram[1:]
                else:
                    doc_id, count = l.split()
                    yield ngram, int(doc_id), int(count)
                    num -= 1
        c.executemany('insert into iindex(ngram, doc_id, count) values (?,?,?)', 
                gen_index())

    print('== build df table...')
    c.execute('create table df (ngram TEXT PRIMARY KEY, count INTEGER)')
    c.execute('insert into df(ngram, count) select ngram, count(count) from iindex group by ngram')

    c.close()
    conn.commit()
    conn.close()
