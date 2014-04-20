import os

# parameters
STOP_LIST = 104
Q_TITLE = True
Q_QUESTION = True
Q_NARRATIVE = True
Q_CONCEPTS = True

# feedback
FB_IT = 5
FB_REL = 5
FB_NREL = 20
FB_CUT = 10000

# OKAPI_BM25
OKAPI_BM25 = True
OK_AVG_L = 771.77
OK_K1 = 1.2
OK_K3 = 1.2
OK_B = 0.75
OK_N = 97445

# file paths
P_DATA_DIR = os.path.join(os.path.dirname(
    os.path.realpath(__file__)),
    'data')
P_STOP_LIST = os.path.join(P_DATA_DIR, 'stoplist.zh_TW.u8')
P_DOCS_COUNT = os.path.join(P_DATA_DIR, 'docs_count')
P_WORDS_INDEX = os.path.join(P_DATA_DIR, 'words_index')
