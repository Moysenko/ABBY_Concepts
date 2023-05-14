from linq import LINQ


def fibonacci_sequence():
    last, current = 0, 1
    while True:
        yield current
        last, current = current, last + current


LINQ(fibonacci_sequence())\
    .where(lambda x: x % 3 == 0)\
    .select(lambda x: x**2 if x % 2 == 0 else x)\
    .take(5)\
    .to_stdout()
