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
        return int.from_bytes(self.get_bytes(pos), byteorder='big', signed=True)
    
    def set_int(self, pos: int, value: int):
        self.set_bytes(pos, value.to_bytes(4, byteorder='big', signed=True))

    def get_parsed_command(self, pos: int):
        command = self.get_bytes(pos)
        to_int = lambda x: int.from_bytes(x, byteorder='big', signed=True)
        return list(map(to_int, [command[:1], command[1: 3], command[3: 4]]))

    @property
    def size(self):
        return len(self._data) // BLOCK_SIZE
