import argparse


def process_commands():
    parser = argparse.ArgumentParser()
    parser.add_argument('--student_id',
                        help='student id')
    parser.add_argument('--log',
                        help='log file')
    parser.add_argument('--student', required=True,
                        help='answer file from student')
    parser.add_argument('--teacher', required=True,
                        help='answer file from teacher')

    return parser.parse_args()


def load(path):
    d = {}
    with open(path) as f:
        for l in f:
            label, truth = l.strip().split()
            d[label] = truth
    return d


def eval_f1(teacher, student):
    total = correct = predicted = 0
    same = 0
    for k, v in teacher.items():
        if v == '1':
            total += 1
            if k in student and student[k] == '1':
                correct += 1
        if k in student and student[k] == v:
            same += 1

    for v in student.values():
        if v == '1':
            predicted += 1

    prec = correct / predicted if predicted != 0 else 1
    recall = correct / total
    f1 = 2 * prec * recall / (prec + recall)
    accuracy = same / len(teacher)
    print('prec\trecall\tf1\taccuracy')
    print('{}\t{}\t{}\t{}'.format(
        prec,
        recall,
        f1,
        accuracy
    ))

    return prec, recall, f1, accuracy


def append(path, *items):
    text = '\t'.join('{}'.format(item) for item in items)
    with open(path, 'a') as f:
        f.write(text + '\n')


def main():
    args = process_commands()

    teacher = load(args.teacher)
    student = load(args.student)

    prec, recall, f1, accuracy = eval_f1(teacher, student)

    if args.student_id is not None and args.log is not None:
        append(args.log, args.student_id, prec, recall, f1, accuracy)

if __name__ == '__main__':
    main()
