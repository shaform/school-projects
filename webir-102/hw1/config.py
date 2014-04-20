import os

STOP_LIST = 100
OKAPI_BM25 = True
Q_TITLE = True
Q_QUESTION = True
Q_NARRATIVE = True
Q_CONCEPTS = True

# file paths
P_DATA_DIR = os.path.join(os.path.dirname(
    os.path.realpath(__file__)),
    'data')
P_STOP_LIST = os.path.join(P_DATA_DIR, 'stoplist.zh_TW.u8')
P_DOCS_COUNT = os.path.join(P_DATA_DIR, 'docs_count')
P_WORDS_INDEX = os.path.join(P_DATA_DIR, 'words_index')
