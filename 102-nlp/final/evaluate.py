import argparse
if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--ans', type=argparse.FileType('r', encoding='utf-8'), required=True)
    parser.add_argument('--my', type=argparse.FileType('r', encoding='utf-8'), required=True)
    parser.add_argument('-o', action='store_true')

    args = parser.parse_args()

    ans_dict = {}
    
    if args.o:
        while True:
            line = args.ans.readline()
            if not line: break
            polarity = int(line)
            text_id, text = args.ans.readline().split('|', 1)
            ans_dict[int(text_id)] = {
                'polarity': int(polarity),
                'pos': [],
                'neg': [],
                }
    else:
        while True:
            line = args.ans.readline()
            if not line: break
            text_id = int(line)
            pos = args.ans.readline().strip().split('\t')
            neg = args.ans.readline().strip().split('\t')
            polarity = int(args.ans.readline())
            ans_dict[int(text_id)] = {
                'polarity': int(polarity),
                'pos': pos,
                'neg': neg,
                }


    my_dict = {}
    while True:
        line = args.my.readline()
        if not line: break
        text_id = int(line)
        pos = args.my.readline().strip().split('\t')
        neg = args.my.readline().strip().split('\t')
        polarity = int(args.my.readline())
        my_dict[text_id] = {
                'polarity': polarity,
                'pos': pos,
                'neg': neg,
                }


    total_ans = len(ans_dict)
    total_my = len(my_dict)
    tp = 0
    for k, item in ans_dict.items():
        if k in my_dict:
            my_item = my_dict[k]
            if item['polarity'] == my_item['polarity']:
                tp += 1

    print('-- overall --')
    print('total: {}, accuracy: {}'.format(total_ans, tp/total_ans))

    if not args.o:
        tans = 0
        tmy = 0
        ttp = 0
        for pol in ('pos', 'neg'):
            print('-- {} aspect --'.format(pol))
            total_ans = 0
            total_my = 0
            tp = 0
            for k, item in ans_dict.items():
                if k in my_dict:
                    my_item = my_dict[k]
                    total_ans += len(item[pol])
                    total_my += len(my_item[pol])
                    for ap in item[pol]:
                        if ap in my_item[pol]:
                            tp += 1
            print('total ans/my: {}/{}, prec: {}, recall: {}'.format(total_ans, total_my, tp/total_my, tp/total_ans))
            tans += total_ans
            tmy += total_my
            ttp += tp

        recall = ttp/tans
        prec = ttp/tmy
        print('total recall: {}, prec: {}, F1: {}'.format(recall, prec, 2*(recall*prec)/(recall+prec)))
