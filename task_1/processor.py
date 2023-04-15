from binary import Binary
import logging

logger = logging.getLogger(__name__)


class Processor:
    def __init__(self, binary: Binary):
        self._ip = 0
        self._sp = 1
        self._binary = binary
    
    def process(self):
        logger.info('Start processing')

        is_finished = False
        while not is_finished:
            is_finished = self._execute_command()

        logger.info('Finish processing')

    def _execute_command(self):
        command_code, first, second = self._binary.get_parsed_command(self._ip)
        is_finished = self._apply_command(command_code, first, second)
        is_finished |= self._ip >= self._binary.size
        return is_finished

    def _apply_command(self, command_code, first, second):
        command_func = Processor._func_by_command_code.get(command_code)
        if command_func is None:
            self._ip += 1
            return False

        logger.info(f'(ip={self._ip}, sp={self._sp})\t'
                    f'Executing command {command_func.__name__}\t'
                    f'first={first}, second={second}')
        result = command_func(self, first, second)
        self._ip += 1
        logger.debug(f'(ip={self._ip}, sp={self._sp})\t'
                    f'Finished command {command_func.__name__}')
        return result if result is not None else False

    def _mov_reg(self, first, second):
        second_value = self._binary.get_bytes(second)
        self._binary.set_bytes(first, second_value)

    def _mov_const(self, first, second):
        self._binary.set_int(first, second)
    
    def _add_reg(self, first, second):
        first_value = self._binary.get_int(first)
        second_value = self._binary.get_int(second)
        self._binary.set_int(first, first_value + second_value)

    def _add_const(self, first, second):
        first_value = self._binary.get_int(first)
        self._binary.set_int(first, first_value + second)

    def _sub_reg(self, first, second):
        first_value = self._binary.get_int(first)
        second_value = self._binary.get_int(second)
        self._binary.set_int(first, first_value - second_value)

    def _sub_const(self, first, second):
        first_value = self._binary.get_int(first)
        print(first_value - second)
        self._binary.set_int(first, first_value - second)
    
    def _mul_reg(self, first, second):
        first_value = self._binary.get_int(first)
        second_value = self._binary.get_int(second)
        self._binary.set_int(first, first_value * second_value)

    def _mul_const(self, first, second):
        first_value = self._binary.get_int(first)
        self._binary.set_int(first, first_value * second)
    
    def _print_int(self, first, second):
        first_value = self._binary.get_int(first)
        print(first_value)

    def _print_str(self, first, second):
        string = self._binary.get_bytes(first).decode(encoding="utf32")
        print(string, end="")

    def _print_helloworld(self, first, second):
        print('Привет, мир!')

    def _push(self, first, second):
        val = self._binary.get_int(first)
        logger.info(f'Pushing {first} on stack, val = {val}')
        self._binary.set_int(self._sp, val)
        self._sp += 1

    def _pop(self, first, second):
        self._sp -= 1
        top_val = self._binary.get_int(self._sp)
        logger.info(f'Popping from stack to {first}, val = {top_val}')
        self._binary.set_int(first, top_val)

    def _call(self, first, second):
        self._binary.set_int(0, self._ip)
        self._push(0, None)
        self._jmp(first, None)
    
    def _ret(self, first, second):
        self._pop(0, None)
        self._ip = self._binary.get_int(0)

    def _exit(self, first, second):
        return True
    
    def _jmp(self, first, second):
        self._ip = first

    _func_by_command_code = {
        0x01: _mov_reg,
        0x02: _mov_const,

        0x10: _add_reg,
        0x11: _add_const,
        0x12: _sub_reg,
        0x13: _sub_const,
        0x14: _mul_reg,
        0x15: _mul_const,

        0x20: _print_int,
        0x21: _print_str,
        0x22: _print_helloworld,

        0x30: _call,
        0x31: _ret,
        0x32: _exit,
        0x33: _jmp,

        0x40: _push,
        0x41: _pop,
    }
        