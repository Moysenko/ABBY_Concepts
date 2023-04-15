#!/usr/local/bin/python3

import argparse
import coloredlogs

from src.binary import Binary
from src.encoder import Encoder
from src.processor import Processor

coloredlogs.install()


def run():
    parser = argparse.ArgumentParser(prog='Neyman-Moysenko')
    parser.add_argument('mode', type=str)
    parser.add_argument('source_filename', type=str)
    parser.add_argument('--destination_filename', type=str, required=False)
    args = parser.parse_args()

    if args.mode == "encode":
        if args.destination_filename is None:
            print('Error: --destination_filename must be specified for mode "encode"')
            return
        enc = Encoder(args.source_filename)
        enc.save_to(args.destination_filename)
    elif args.mode == "process":
        binary_code = Binary(args.source_filename)
        proc = Processor(binary_code)
        proc.process()
    else:
        print("Error: unknown mode")

if __name__ == '__main__':
	run()