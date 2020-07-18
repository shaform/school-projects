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

import re
import xml.etree.ElementTree as etree

def parse_doc(text):
    doc = etree.fromstring(text).find('doc')
    return {'doc_id': doc.find('id').text.strip().lower(),
            'date': doc.find('date').text.strip(),
            'title': doc.find('title').text.strip(),
            'text': etree.tostring(doc.find('text'), encoding='unicode',
                method='text').strip()
            }

def tokenize(text):
    rENG = re.compile('([a-zA-Z0-9._+*/-]+)')
    tokens = []
    for each in text.split():
        each = each.strip()
        for t in rENG.split(each):
            m = rENG.match(t)
            if m is not None:
                tokens.append(t)
            else:
                for u in re.split('\W+', t):
                    for c in u:
                        tokens.append(c)
    return tokens

def text_ngrams(text, ngram_num=2):
    tokens = tokenize(text)
    ngram_list = []
    for gnum in range(1, ngram_num+1):
        for i in range(len(tokens)-gnum+1):
            ngram_list.append(tuple(tokens[i:i+gnum]))
    return ngram_list
