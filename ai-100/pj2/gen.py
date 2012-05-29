import random
import time
import pickle
import re
import os
import subprocess
from subprocess import Popen
import sys
import signal
 

MAX = 1000000
SAVE_PATH = './gen_param'

nextFId = 0
nextSId = 0

bestResults = [-12, -12]
bestParams = [
        [0, 0, 0, 0],
        [0, 0, 0, 0],
        ]


def saveFirst(params):
    global nextFId
    global bestParams
    with open(SAVE_PATH + '/bestF%d' % nextFId, 'w') as f:
        pickle.dump(params, f)

    nextFId = nextFId + 1
    with open(SAVE_PATH + '/bestFId', 'w') as f:
        pickle.dump(nextFId, f)

    bestParams[0] = list(params)

def saveSecond(params):
    global nextSId
    global bestParams
    with open(SAVE_PATH + '/bestS%d' % nextSId, 'w') as f:
        pickle.dump(params, f)

    nextSId = nextSId + 1
    with open(SAVE_PATH + '/bestSId', 'w') as f:
        pickle.dump(nextSId, f)

    bestParams[1] = list(params)

def genRandomParams(params):
    gened = []
    for p in params:
        gened.append(random.randint(0, MAX))
    return gened

def genNewParams(params):
    gened = list(params)
    idx = random.randint(0, len(params)-1)
    p = gened[idx]
    gened[idx] = random.randint(max(0, p/2), min(p+p/2, MAX))
    if random.randint(0, 100) > 80:
        gened[idx] = random.randint(0, MAX)
    return gened

def writeParams(params, player, fname):
    with open('./player%d/%s' % (player, fname), 'w') as f:
        for p in params:
            f.write('%d ' % p)

def getResult(fp1, sp1, fp2, sp2, hand):
    time.sleep(1)
    writeParams(fp1, 1, 'first')
    writeParams(sp1, 1, 'second')
    writeParams(fp2, 2, 'first')
    writeParams(sp2, 2, 'second')

    subprocess.call(["./clear_files"], shell=True)
    time.sleep(1)

    if hand == 0:
        fPlayer = '2'
    else:
        fPlayer = '1'

    b = Popen(['java', 'Umpire', fPlayer, 'false', '600'], stdout=subprocess.PIPE)
    c = Popen(['./proj2_client', '1', 'first', 'second'],
            cwd=os.path.abspath('./player1/'), stdout=subprocess.PIPE)
    d = Popen(['./proj2_client', '2', 'first', 'second'],
            cwd=os.path.abspath('./player2/'), stdout=subprocess.PIPE)

    class TimeoutException(Exception): 
        pass 

    def timeout_handler(signum, frame):
        print 'time out!'
        raise TimeoutException()

    old_handler = signal.signal(signal.SIGALRM, timeout_handler)

    try:
        while True:
            signal.alarm(60)
            line = b.stdout.readline()
            signal.alarm(0)

            m = re.match('player(\d) is winner, remain (\d+)', line)
            if m is not None:
                result = int(m.group(2))
                if int(m.group(1)) == 1:
                    result = -result
                break

    except:
        result = 0

    finally:
        signal.alarm(0)
        signal.signal(signal.SIGALRM, old_handler) 

        c.kill()
        d.kill()
        b.kill()

        c.wait()
        d.wait()
        b.wait()

    time.sleep(3)
    return result

# set up environments

# check directory if not exists
if not os.path.exists(SAVE_PATH):
    os.makedirs(SAVE_PATH)

# check index files
if not os.path.exists(SAVE_PATH + '/bestFId'):
    with open(SAVE_PATH + '/bestFId', 'w') as f:
        pickle.dump(0, f)

if not os.path.exists(SAVE_PATH + '/bestSId'):
    with open(SAVE_PATH + '/bestSId', 'w') as f:
        pickle.dump(0, f)

# read index files
with open(SAVE_PATH + '/bestFId', 'r') as f:
    nextFId = pickle.load(f)
with open(SAVE_PATH + '/bestSId', 'r') as f:
    nextSId = pickle.load(f)


def calcBestResults():
    global bestResults
    for h in [0, 1]:
        bestResults[h] = getResult(bestParams[0], bestParams[1],
                bestParams[0], bestParams[1], h)

def checkSame(params, h):
    return False
    print 'check same'
    a = getResult(bestParams[h], bestParams[h],
            params[h], params[h], 0)
    b = getResult(bestParams[h], bestParams[h],
            params[h], params[h], 1)
    if a+b > 0:
        print 'better'
        return True
    else:
        print 'worse'
        return False


if nextFId > 0 and nextSId > 0:
    with open(SAVE_PATH + '/bestF%d' % (nextFId-1), 'r') as f:
        bestParams[0] = pickle.load(f)
    with open(SAVE_PATH + '/bestS%d' % (nextSId-1), 'r') as f:
        bestParams[1] = pickle.load(f)
    calcBestResults()

print bestResults

# main loop

tryNum = 0
switchNum = 0
testHand = 1

bParams = list(bestParams)
cParams = list(bParams)
bResults = list(bestResults)
while True:
    # gen new result
    cParams[testHand] = genNewParams(bParams[testHand])
    # random restart
    if tryNum > 10:
        if random.randint(0, 100) > 60:
            cParams[testHand] = genRandomParams(cParams[testHand])
            # reset try number
            tryNum = 0
            # always accept new result
            bResults[testHand] = -12

    print 'gen param'
    print cParams[testHand]
    print 'for hand%d' % testHand
    print 'now best'
    print bestResults
    print 'now current best'
    print bResults

    result = getResult(bestParams[0], bestParams[1],
        cParams[0], cParams[1], testHand)
    print 'Result %d by' % result
    print cParams[testHand]
    
    if result != 0 and result > bResults[testHand]:

        if result > bResults[testHand]:
            tryNum = 0

        bResults[testHand] = result
        bParams[testHand] = list(cParams[testHand])

        if (bResults[testHand] > bestResults[testHand]
                or (result == bestResults[testHand]
                    and checkSame(cParams, testHand))):
            print 'New Result'
            if testHand == 0:
                saveFirst(cParams[0])
            else:
                saveSecond(cParams[1])

            # force switch
            switchNum = switchNum + 100
            calcBestResults()
            # start from best
            bParams = list(bestParams)
            cParams = list(bParams)
            bResults = list(bestResults)

    else:
        tryNum = tryNum + 1
        switchNum = switchNum + 1

    if switchNum > 100:
        if bResults[testHand] > 0 or switchNum > 200:
            testHand = (testHand + 1) % 2
