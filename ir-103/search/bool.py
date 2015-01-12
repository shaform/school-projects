import argparse
import os

def process_commands():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('source_dir')
    parser.add_argument('query_path')
    return parser.parse_args()


if __name__ == '__main__':
    args = process_commands()

    input_dir = os.path.abspath(args.source_dir)

    queries = []
    with open(args.query_path, 'r') as f:
        for l in f:
            num, terms = l.strip().split(' ', 1)
            terms = terms.split()
            queries.append((num, terms))


    results = [[] for _ in range(len(queries))]
    for root, dirs, files in os.walk(input_dir):
        for f_name in sorted(files, key=lambda x: int(x)):
                
            file_path = os.path.join(root, f_name)
            with open(file_path, 'r') as f:
                doc = f.read()

            for i in range(len(queries)):
                all_found = True
                for x in queries[i][1]:
                    if x not in doc:
                        all_found = False
                        break
                if all_found:
                    results[i].append(f_name)

    for query, result in zip(queries, results):
        print(query[0])
        print(' '.join(result))
