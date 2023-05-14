from linq import LINQ


def file_lines(filename):
    with open(filename) as input:
        for line in input:
            yield line


LINQ(file_lines('word_count.txt'))\
    .select(lambda x: x.strip())\
    .select(lambda x: x.split())\
    .flatten()\
    .group_by(lambda x: x)\
    .select(lambda x: (x[0], len(x[1])))\
    .order_by(lambda x: -x[1])\
    .to_stdout()