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

import os.path
import urllib.parse
import xml.etree.ElementTree as etree

import requests
from bs4 import BeautifulSoup

import tools.indexer

def use_google_news(q):
    NEWS_URL = 'https://news.google.com/news/feeds?q={}&output=rss&ned=tw'

    results = []

    r = requests.get(NEWS_URL.format(urllib.parse.quote_plus(q)))
    if r.status_code == 200:
        root = etree.fromstring(r.text)
        channel = root.find('channel')
        for e in channel:
            if e.tag == 'item':
                text = e.find('title').text

                idx = text.rfind(' - ')
                title, source = text[:idx], text[idx+3:]
                html = e.find('description').text
                soup = BeautifulSoup(html)
                for link in soup.find_all('a'):
                    href = link.get('href')
                    if href is not None and href.find('news.google.com') != -1:
                        link.replace_with('')
                desc = soup.text.replace('\xa0', ' ')

                results.append({'title': title,
                    'source': source,
                    'text': desc})

    return results

def use_cirb(q, pysearch):
    # TODO: currently use path prefix to determine the source,
    # should use a more general approach
    SOURCE_DICT = {
            'APD': '蘋果日報',
            'LTN': '自由時報',
            'CTS': '中國時報',
            }
    ranked_list = pysearch.search(tools.indexer.text_ngrams(q))
    results = []

    for entry in ranked_list[:100]:
        with open(entry['path'], 'r') as f:
            doc = tools.indexer.parse_doc(f.read())
            results.append({'id': entry['id'],
                'title': doc['title'],
                'source': SOURCE_DICT[os.path.basename(entry['path'])[:3]],
                'text': doc['text']})

    return results
