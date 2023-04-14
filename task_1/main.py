#!/usr/local/bin/python3

from binary import Binary
from encoder import Encoder
from processor import Processor
import coloredlogs

coloredlogs.install()


def run():
    mode = input("Mode: ")
    if mode == "encode":
        enc = Encoder(input("Code source filename: "))
        enc.save_to(input("Binary code destination filename: "))
    elif mode == "process":
        binary_code = Binary(input("Binary code source filename: "))
        proc = Processor(binary_code)
        proc.process()
    else:
        print("Unknown mode")

if __name__ == '__main__':
	run()