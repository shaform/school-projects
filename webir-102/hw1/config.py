import os

# parameters
STOP_LIST = 404
Q_TITLE = True
Q_QUESTION = True
Q_NARRATIVE = True
Q_CONCEPTS = True
PP = 12

# feedback
FB_IT = 2
FB_REL = 10
FB_NREL = 5000
FB_CUT = 10000
FB_A = 1
FB_B = 0.8

# OKAPI_BM25
OKAPI_BM25 = True
OK_AVG_L = 771.77
OK_K1 = 1.6
OK_K3 = 1.6
OK_B = 0.75
OK_N = 97445

# file paths
P_DATA_DIR = os.path.join(os.path.dirname(
    os.path.realpath(__file__)),
    'data')
P_STOP_LIST = os.path.join(P_DATA_DIR, 'stoplist.zh_TW.u8')
P_DOCS_COUNT = os.path.join(P_DATA_DIR, 'docs_count')
P_WORDS_INDEX = os.path.join(P_DATA_DIR, 'words_index')
