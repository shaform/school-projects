"""query.py
Parse query xml files.
"""
import xml.etree.ElementTree as etree
import sys

PARSE_TITLE = True
PARSE_QUESTION = True
PARSE_NARRATIVE = True
PARSE_CONCEPTS = True

def parse_xml(xml_path):
    tree = etree.parse(xml_path)
    root = tree.getroot()
    queries = []
    for child in root:
        query = {
                'number': child.find('number').text.strip('\n'),
                'title': child.find('title').text.strip('\n'),
                'question': child.find('question').text.strip('\n'),
                'narrative': child.find('narrative').text.strip('\n'),
                'concepts': child.find('concepts').text.strip('\n'),
                }
        queries.append(query)
    return queries

def parse_queries(queries):
    processed = []
    for query in queries:
        if PARSE_TITLE:
            pass
    return processed

def collect_words(queries):
    words = set()
    for query in queries:
        for w in  query['concepts'].strip('。 \t\n').split('、'):
            if '、' not in w and '。' not in w and len(w) > 2:
                words.add(w)
    return words

if __name__ == '__main__':
    if len(sys.argv) == 2:
        queries = parse_query(sys.argv[1])
        words = collect_words(queries)
