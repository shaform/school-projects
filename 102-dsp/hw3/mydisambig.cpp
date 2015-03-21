#include <cstdio>
#include <cstring>
#include <cassert>

#include <algorithm>
#include <functional>
#include <map>
#include <queue>
#include <vector>

#include "Ngram.h"
#include "Vocab.h"
#include "VocabMap.h"

using std::exit;
using std::perror;
using std::printf;
using std::sscanf;
using std::strcmp;

using std::greater;
using std::map;
using std::pair;
using std::priority_queue;
using std::swap;
using std::vector;

const int MAX_ORDER = 4;
const LogP LogP_PseudoZero = -100;

class VNode {
    public:
        VocabIndex wid;
        LogP prob;

        VNode(const VocabIndex wid)
            : wid(wid), _prev(NULL), _ref_count(0), prob(0) {}

        VNode(const VocabIndex wid, VNode *prev)
            : wid(wid), _prev(prev), _ref_count(0), prob(0) { ++_prev->_ref_count; }

        ~VNode() {
            if (_ref_count) {
                perror("error: deleting a VNode with referencing objects\n");
            }
            if (_prev) {
                if (--_prev->_ref_count == 0) {
                    delete _prev;
                }
            }
        }
        bool unreferenced() const { return _ref_count == 0; }
        bool operator<(const VNode &rhs) const { return wid < rhs.wid; }
        VNode *prev() const { return _prev; }

    private:
        VNode *_prev;
        unsigned _ref_count;
};

class Front {
    public:
        map<VocabIndex, Front *> prefix;
        VNode *node;
        Front() : node(NULL) {}
        ~Front() { clear(); }
        /* clear() is used to delete all unreferenced VNodes,
         * and reinitialize the Front at the same time.
         */
        void clear() {
            for (map<VocabIndex, Front *>::iterator it=prefix.begin();
                    it!=prefix.end(); ++it) {
                delete it->second;
            }
            prefix.clear();
            if (node != NULL && node->unreferenced()) {
                delete node;
            }
            node = NULL;
        }
};

class ViterbiTree {
    public:
        ViterbiTree(Ngram &lm, int ngram_order, int prun_num)
            : _lm(lm), _ngram_order(ngram_order),
            _front(new Front()), _back(new Front()), _solution(NULL),
            _prun_num(prun_num)  //, _prun_mini(10.0)
        {
            assert(_ngram_order >= 1 && _ngram_order <= MAX_ORDER);

            Front *curr = _front;
            for (int i=1; i<_ngram_order; ++i) {
                curr = curr->prefix[_lm.vocab.ssIndex()] = new Front();
            }
            curr->node = new VNode(_lm.vocab.ssIndex());
        }
        ~ViterbiTree() {
            // destroy the frontends, this should also destroy the root
            delete _back;
            delete _front;
        }
        void step(const vector<VocabIndex> &next, bool end=false);
        void end() {
            vector<VocabIndex> next(1, _lm.vocab.seIndex());
            step(next, true);
        }
        void fillSentence(VocabString *sentence, int len) const;

    private:
        Ngram &_lm;
        const int _ngram_order;
        Front *_front, *_back;
        VNode *_solution;
        int _prun_num;
        // LogP _prun_mini;
        bool _step(Front *from, Front *to, VocabIndex wid, VocabIndex *context, int rdepth, bool end,
                priority_queue<LogP, vector<LogP>, greater<LogP> > &maxs);
};

void ViterbiTree::step(const vector<VocabIndex> &next, bool end) {
    assert(next.size() > 0);

    _back->clear();
    swap(_back, _front);
    // add new nodes
    VocabIndex context[MAX_ORDER] = { Vocab_None };
    if (_ngram_order == 1) {
        VocabIndex wid = next.front();
        LogP maxi = _lm.wordProb(wid, context);
        for (int i=1; i<next.size(); ++i) {
            LogP t = _lm.wordProb(next[i], context);
            if (t > maxi) {
                wid = next[i];
                maxi = t;
            }
        }
        _front->node = new VNode(wid, _back->node);
        if (end) {
            _solution = _front->node;
        }
    } else {
        context[MAX_ORDER-1] = Vocab_None;
        priority_queue<LogP, vector<LogP>, greater<LogP> > maxs;
        for (vector<VocabIndex>::const_iterator it=next.begin();
                it!=next.end(); ++it) {
            Front *curr = _front->prefix[*it] = new Front();
            _step(_back, curr, *it, &context[MAX_ORDER-_ngram_order], 0, end, maxs);
        }
        /*
        if (_prun_num && maxs.size() >= _prun_num) {
            _prun_mini = maxs.top();
        } else {
            _prun_mini = 10.0;
        }
        */
    }
}

bool ViterbiTree::_step(Front *from, Front *to, VocabIndex wid, VocabIndex *context,
        int rdepth, bool end, priority_queue<LogP, vector<LogP>, greater<LogP> > &maxs) {
    if (rdepth < _ngram_order-2) {
        for (map<VocabIndex, Front*>::iterator it=from->prefix.begin();
                it != from->prefix.end(); ++it) {
            context[rdepth] = it->first;
            Front *newf = new Front();
            bool pruned = _step(it->second, newf,
                    wid, context, rdepth+1, end, maxs);
            if (pruned) {
                delete newf;
            } else {
                to->prefix[it->first] = newf;
            }
        }
    } else {
        VNode *prefix = NULL;
        LogP maxi;
        for (map<VocabIndex, Front*>::iterator it=from->prefix.begin();
                it != from->prefix.end(); ++it) {
            context[rdepth] = it->first;
            VNode *curr = it->second->node;
            assert(curr != NULL);
            /*
            if (_prun_num && _prun_mini < 5.0 && curr->prob < _prun_mini) {
                continue;
            }
            */
            // calculate transition probability
            VocabIndex nctx[] = { Vocab_None };
            LogP pTrans = _lm.wordProb(wid, context), pUni = _lm.wordProb(wid, nctx);
            if (pTrans == LogP_Zero && pUni == LogP_Zero) {
                pTrans = LogP_PseudoZero;
            }
            // calculate current maximum probability
            LogP t = pTrans + curr->prob;
            if (prefix == NULL || t > maxi ||
                    (t == maxi &&
                     _lm.wordProb(curr->wid, nctx) > _lm.wordProb(prefix->wid, nctx))) {
                maxi = t;
                prefix = curr;
            }
        }
        if (_prun_num) {
            if (prefix == NULL || (maxs.size() >= _prun_num
                        && (maxi < maxs.top() || (maxi == maxs.top() && maxi == LogP_Zero)))) {
                return true;
            } else {
                maxs.push(maxi);
                while (maxs.size() > _prun_num) {
                    maxs.pop();
                }
            }
        }
        to->node = new VNode(wid, prefix);
        to->node->prob = maxi;
        if (end && (_solution == NULL || _solution->prob < maxi)) {
            _solution = to->node;
        }
    }
    return false;
}

void ViterbiTree::fillSentence(VocabString *sentence, int len) const {
    assert(_solution != NULL);
    VNode *back = _solution->prev();
    for (int i=len-1; i>=0 && back; --i) {
        if (back->wid != _lm.vocab.unkIndex() && back->wid != Vocab_None) {
            sentence[i] = _lm.vocab.getWord(back->wid);
        }
        back = back->prev();
    }
}

void disambigFile(File &tFile, VocabMap &map, Ngram &lm, int ngram_order, int prun_num);
void disambigSentence(VocabString *sentence, int len, VocabMap &map, Ngram &lm, int ngram_order, int prun_num);


static void usage(char *argv[]) {
    printf("Usage : %s TEXT MAP LM ORDER [-prun n]\n", argv[0]);
    exit(1);
}

int main(int argc, char *argv[]) {
    if (argc < 5) {
        usage(argv);
    }

    // load map
    Vocab vocab, hVocab;
    VocabMap map(vocab, hVocab);
    hVocab.unkIsWord() = true;

    File mFile(argv[2], "r");
    if (!map.read(mFile)) {
        perror("invalid map file\n");
        exit(1);
    }
    mFile.close();

    // decide order
    int ngram_order;
    if (sscanf(argv[4], "%d", &ngram_order) != 1 || ngram_order < 1 || ngram_order > MAX_ORDER) {
        perror("invalid order\n");
        ngram_order = 2;
    }

    // decide pruning number
    int prun_num;
    if (argc != 7 || strcmp(argv[5], "-prun") != 0 || sscanf(argv[6], "%d", &prun_num) != 1 || prun_num < 0) {
        prun_num = 0;
    }

    // load language model
    Ngram lm(hVocab, ngram_order);
    File lmFile(argv[3], "r");
    lm.read(lmFile);
    lmFile.close();

    // process text
    File tFile(argv[1], "r");
    disambigFile(tFile, map, lm, ngram_order, prun_num);
    tFile.close();

    return 0;
}

/**
 * disambigFile
 *
 * Break file into sentences and pass it to the disambigSentence function.
 * This function is wrritten with the help from the sourcecode "disambig.cc" in srilm.
 */
void disambigFile(File &tFile, VocabMap &map, Ngram &lm, int ngram_order, int prun_num) {
    char *line;
    VocabString sentence[maxWordsPerLine];

    while (line = tFile.getline()) {
        unsigned len = Vocab::parseWords(line, sentence, maxWordsPerLine);
        if (len >= maxWordsPerLine) {
            perror("too many words in a line\n");
        } else {
            disambigSentence(sentence, len, map, lm, ngram_order, prun_num);

            // print out the result
            printf("<s>");
            for (int i=0; i<len; ++i) {
                printf(" %s", sentence[i]);
            }
            printf(" </s>\n");
        }
    }
}

void disambigSentence(VocabString *sentence, int len, VocabMap &map, Ngram &lm, int ngram_order, int prun_num) {
    ViterbiTree vt(lm, ngram_order, prun_num);
    vector<VocabIndex> next;

    for (int i=0; i<len; ++i) {
        next.clear();

        VocabIndex wid = map.vocab1.getIndex(sentence[i]), twid;
        if (wid == Vocab_None) {  // OOV
            twid = lm.vocab.getIndex(sentence[i]);
            if (twid == Vocab_None) {
                twid = lm.vocab.unkIndex();
            }
            next.push_back(twid);
        } else {
            VocabMapIter iter(map, wid);
            Prob _;
            while (iter.next(twid, _)) {
                next.push_back(twid);
            }
        }
        vt.step(next);
    }

    vt.end();
    vt.fillSentence(sentence, len);
}
