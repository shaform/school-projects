import argparse
import json
import re

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--data', type=argparse.FileType('r'), required=True)
    parser.add_argument('--base', type=argparse.FileType('r'), required=True)
    parser.add_argument('--opdict', type=argparse.FileType('r'), required=True)
    parser.add_argument('-o', type=argparse.FileType('w'), required=True)
    args = parser.parse_args()

    op_dict = json.load(args.opdict)
    data = json.load(args.data)

    base = {}
    while True:
        text_id = args.base.readline()
        if not text_id:
            break
        text_id = int(text_id)
        pos_ops = set(args.base.readline().strip().split('\t'))
        neg_ops = set(args.base.readline().strip().split('\t'))
        for ops in (pos_ops, neg_ops):
            if '' in ops:
                ops.remove('')
        score = int(args.base.readline())
        base[text_id] = {
                'pos': pos_ops,
                'neg': neg_ops,
                'score': score
                }

    # detect implicit
    for item in data:
        if base[item['id']]['score'] == 1:
            pos_imply = op_dict['pos_imply']
            for k, ap in pos_imply.items():
                if re.search('[^不]' + k, item['tw']) is not None:
                    if ap not in base[item['id']]['neg']:
                        base[item['id']]['pos'].add(ap)
        elif base[item['id']]['score'] == 2:
            neg_imply = op_dict['neg_imply']
            for k, ap in neg_imply.items():
                if re.search('[^不]' + k, item['tw']) is not None:
                    if ap not in base[item['id']]['pos']:
                        base[item['id']]['neg'].add(ap)

    # detect same
    for text_id, item in base.items():
        for aps in op_dict['same']:
            for ap in aps:
                if ap in item['pos']:
                    for t in aps:
                        if t not in item['neg'] and t not in item['pos']:
                            item['pos'].add(t)
                if ap in item['neg']:
                    for t in aps:
                        if t not in item['pos'] and t not in item['neg']:
                            item['neg'].add(t)
    # detect conflict
    for item in data:
        bitem = base[item['id']]
        if bitem['score'] == 1:
            for ap in bitem['pos']:
                if ap in bitem['neg']:
                    bitem['neg'].remove(ap)
        elif bitem['score'] == 2:
            for ap in bitem['neg']:
                if ap in bitem['pos']:
                    bitem['pos'].remove(ap)

    
    # output
    for text_id in sorted(base.keys()):
        item = base[text_id]
        args.o.write('{}\n{}\n{}\n{}\n'.format(text_id,
            '\t'.join(list(item['pos'])),
            '\t'.join(list(item['neg'])),
            item['score']))
