BLOCK_SIZE = 4


class Binary:
    def __init__(self, filename):
        with open(filename, "rb") as binary_code:
            self._data = bytearray(binary_code.read())

    def get_bytes(self, pos: int):
        block_start = pos * BLOCK_SIZE
        return self._data[block_start: block_start + BLOCK_SIZE]

    def set_bytes(self, pos: int, value):
        block_start = pos * BLOCK_SIZE
        self._data[block_start: block_start + BLOCK_SIZE] = value

    def get_int(self, pos: int):
        return int.from_bytes(self.get_bytes(pos), byteorder='big')

    def get_parsed_command(self, pos: int):
        command = self.get_bytes(pos)
        to_int = lambda x: int.from_bytes(x, byteorder='big')
        return list(map(to_int, [command[:1], command[1: 2], command[2: 4]]))

    @property
    def size(self):
        return len(self._data) // BLOCK_SIZE
