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

import math
import os
import random
from collections import defaultdict
from statistics import mean

# all scores are in range [-1, 1]

def sigmoid(x):
    return 1 / (1 + math.exp(-x))

def norm(x):
    return (sigmoid(x)-0.5)*2

class RandomJudger(object):
    def judge(self, evt):
        return random.uniform(-1, 1)

class Judger(object):
    def __init__(self, path, name):
        with open(os.path.join(path, name), 'r') as f:
            self.score_list = self.load_list(f)

    def load_list(self, f):
        raw_list = []
        pos, neg = [], []
        for l in f:
            score = float(l)
            raw_list.append(score)
            if score >= 0:
                pos.append(score)
            else:
                neg.append(-score)
        mp, mn = mean(pos), mean(neg)

        score_list = []
        for x in raw_list:
            if x >= 0:
                score_list.append(x/mp*2)
            else:
                score_list.append(x/mn*2)
        return [ norm(x) for x in score_list ]

    def judge(self, evt):
        if 'id' in evt:
            text_id = evt['id']
            if text_id <= len(self.score_list):
                return self.score_list[text_id-1]
        return RandomJudger().judge(evt)

def event_by_source(events):
    by_source = defaultdict(list)
    for evt in events:
        by_source[evt['source']].append(evt)
    return by_source

def random_semantic(events):
    return semantic_simple_sum(event_by_source(events), RandomJudger())

def score_semantic(events, judger):
    return semantic_simple_sum(event_by_source(events), judger)

def semantic_simple_sum(by_source, semantic_judge):
    results = []

    for k in by_source:
        total = 0.0
        positive, negative = [], []
        for evt in by_source[k]:
            score = semantic_judge.judge(evt)
            if score < 0:
                negative.append((score, evt))
            else:
                positive.append((score, evt))
            total += score

        total /= len(by_source[k])
        results.append({'score': total,
            'source': k,
            'positive': positive,
            'negative': negative,
            })

    random.shuffle(results)
    return results
