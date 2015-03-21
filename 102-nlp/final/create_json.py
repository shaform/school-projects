"""
create_json.py 

create a json file for latter use

Usage:
    create_json.py -i INPUT -a ASPECT-FILE -o OUTPUT-FILE

json format:
[
{
    "id": 1,
    "tw": "zh_tw text",
    "tw_ap": ["zh_tw aspect", ...],
    "cn": "zh_cn text",
    "cn_ap": ["zh_cn aspect", ...],
}, ...
]
"""
import argparse
import json

import opencc

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('-i', type=argparse.FileType('r'), required=True)
    parser.add_argument('-a', type=argparse.FileType('r'), required=True)
    parser.add_argument('-o', type=argparse.FileType('w'), required=True)
    args = parser.parse_args()
    
    aps = dict()
    for l in args.a:
        tw_ap = l.strip()
        if len(tw_ap) == 0:
            continue
        cn_ap = opencc.t2s(tw_ap)
        aps[tw_ap] = cn_ap

    elements = []
    for l in args.i:
        l = l.strip()
        if len(l) == 0:
            continue
        text_id, tw_text = l.split('|', 1)
        cn_text = opencc.t2s(tw_text)

        e = {'id': int(text_id),
                'tw': tw_text,
                'tw_ap': [],
                'cn': cn_text,
                'cn_ap': [],
                }
        for tw, cn in aps.items():
            if tw_text.find(tw) != -1:
                e['tw_ap'].append(tw)
                e['cn_ap'].append(aps[tw])
        elements.append(e)

    json.dump(elements, args.o, indent=2, ensure_ascii=True)

