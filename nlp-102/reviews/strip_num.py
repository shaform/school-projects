import fileinput


flip = False
for l in fileinput.input():
    flip = not flip
    if flip:
        continue
    else:
        print(l[l.find('|')+1:-1])
