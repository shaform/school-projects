"""
doc_size.py DB_PATH
"""
import math
import sqlite3
import sys

conn = sqlite3.connect(sys.argv[1])
c = conn.cursor()
c.execute('select id from doc order by id asc')
for doc_id in c.fetchall():
    c.execute('select ngram, count from iindex where doc_id = ?', doc_id)
    size = 0.0
    for pair in c.fetchall():
        size += pair[1] * pair[1]
    size = math.sqrt(size)
    print(size) 

c.close()
conn.close()
