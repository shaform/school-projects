import fileinput

NN = set()
VA = set()

for l in fileinput.input():
    ts = l.split()
    for t in ts:
        if t.find('#NN') != -1:
            NN.add(t)
        if t.find('#VA') != -1:
            VA.add(t)

NN = list(NN)
VA = list(VA)

for i in range(0, len(NN), 10):
    print(' '.join(NN[i:i+10]))

for i in range(0, len(VA), 10):
    print(' '.join(VA[i:i+10]))
