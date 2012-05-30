from gen import getResult
import os
import pickle


LOAD_PATH = './gen_param'
SAVE_PATH = './result_param'

# check directory if not exists
if not os.path.exists(SAVE_PATH):
    os.makedirs(SAVE_PATH)

nextFId = 0
nextSId = 0

# read index files
with open(LOAD_PATH + '/bestFId', 'r') as f:
    nextFId = pickle.load(f)
with open(LOAD_PATH + '/bestSId', 'r') as f:
    nextSId = pickle.load(f)

f = open(SAVE_PATH + '/result', 'a')

def loadFirstParams(FId):
    with open(LOAD_PATH + '/bestF%d' % FId, 'r') as f:
        return pickle.load(f)

def loadSecondParams(FId):
    with open(LOAD_PATH + '/bestS%d' % FId, 'r') as f:
        return pickle.load(f)


def checkParam(aParam, bParam, hand):
    t = getResult(aParam, aParam,
            bParam, bParam, hand)
    return t

def checkPair(aId, aRole, bId, bRole, hand):
    aParam = []
    if aRole == 0:
        aParam = loadFirstParams(aId)
    else:
        aParam = loadSecondParams(aId)
    bParam = []
    if bRole == 0:
        bParam = loadFirstParams(bId)
    else:
        bParam = loadSecondParams(bId)

    return checkParam(aParam, bParam, hand)

def outputLine(line):
    print line
    f.write(line + '\n')
    f.flush()

ROUND_NUM = 5
cRound = 0


while cRound < nextFId or cRound < nextSId:
    outputLine('round %d' % cRound)

    maxIds = [min(cRound+ROUND_NUM, nextFId), min(cRound+ROUND_NUM, nextSId)]

    for p in range(2):
        for q in range(2):

            for i in range(cRound, maxIds[p]):
                r = [0, 0]
                for j in range(cRound, maxIds[q]):
                    t = checkPair(i, p, j, q, p)
                    if t > 0:
                        outputLine('[%d,%d] wins with %d' % (p, i, t))
                        r = [r[0]+1, r[1]+t]
                    elif t < 0:
                        outputLine('[%d,%d] loses with %d' % (p, i, t))
                    else:
                        outputLine('[%d,%d] error' % (p, i))
                outputLine('[%d,%d] wins %d with %d against %d' % (p, i, r[0], r[1], q))

    cRound = cRound + ROUND_NUM

f.close()
