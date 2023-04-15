import logging

logger = logging.getLogger(__name__)


STACK_SIZE = 3


class SectionType:
    VARIABLES = 'VARIABLES'
    CODE = 'CODE'
_section_types = (SectionType.VARIABLES, SectionType.CODE)

class ArgumentType:
    REG = 'reg'
    CONST = 'const'
    FUNC = 'func'

_command_meta = {
    'MOV_REG': {
        'code': 0x01,
        'args': (ArgumentType.REG, ArgumentType.REG),
    },
    'MOV_CONST': {
        'code': 0x02,
        'args': (ArgumentType.REG, ArgumentType.CONST),
    },

    'ADD_REG': {
        'code': 0x10,
        'args': (ArgumentType.REG, ArgumentType.REG),
    },
    'ADD_CONST': {
        'code': 0x11,
        'args': (ArgumentType.REG, ArgumentType.CONST),
    },
    'SUB_REG': {
        'code': 0x12,
        'args': (ArgumentType.REG, ArgumentType.REG),
    },
    'SUB_CONST': {
        'code': 0x13,
        'args': (ArgumentType.REG, ArgumentType.CONST),
    },
    'MUL_REG': {
        'code': 0x14,
        'args': (ArgumentType.REG, ArgumentType.REG),
    },
    'MUL_CONST': {
        'code': 0x15,
        'args': (ArgumentType.REG, ArgumentType.CONST),
    },

    'PRINT_INT': {
        'code': 0x20,
        'args': (ArgumentType.REG,),
    },
    'PRINT_STR': {
        'code': 0x21,
        'args': (ArgumentType.REG,),
    },
    'PRINT_HELLOWORLD': {
        'code': 0x22
    },

    'CALL': {
        'code': 0x30,
        'args': (ArgumentType.FUNC,),
    },
    'RET': {
        'code': 0x31,
    },
    'EXIT': {
        'code': 0x32,
    },
    'JMP': {
        'code': 0x33,
        'args': (ArgumentType.FUNC,),
    },

    'PUSH': {
        'code': 0x40,
        'args': (ArgumentType.REG,),
    },
    'POP': {
        'code': 0x41,
        'args': (ArgumentType.REG,),
    },
}
def _get_command_meta(command):
    meta = _command_meta.get(command)
    if meta is None:
        logger.error(f"There is no command \"{command}\"")
        raise KeyError("Unknown command")
    return meta


class Encoder:
    def __init__(self, code_filename):
        with open(code_filename, "r") as code_file:
            self._code = code_file.readlines()

        self._binary = bytearray(4)
        self._var_pos = {}
        self._var_values = list()
        self._func_pos = {}

        current_section = SectionType.VARIABLES
        for line in self._code:
            formatted_line = line.strip()
            if formatted_line == "":
                continue

            if formatted_line in _section_types:
                current_section = formatted_line
                continue

            if current_section == SectionType.VARIABLES:
                self._add_variable(formatted_line)
            else:
                self._process_code(formatted_line)

        self._process_code('JMP main')
        self._binary[0:4] = self._binary[-4:]
        del self._binary[-4:]

        self._encode_variables(self._var_values)

    def save_to(self, output_filename):
        log_string = " ".join([
            ("\n" if i % 4 == 0 else "") + hex(x) 
            for i, x in enumerate(self._binary)
        ])
        logger.info(f'Saving program: {log_string}')
        with open(output_filename, "wb") as out_file:
            out_file.write(self._binary)

    def _add_variable(self, command):
        if '=' in command:
            name, val = command.split('=', 1)
            name = name.strip()
            val = val.strip()
        else:
            name, val = command, None
        logger.info(f'Add variable {name}={val}')
        self._var_pos[name] = STACK_SIZE + len(self._var_values)
        self._var_values.append(val)

    def _process_code(self, command):
        if command == "":
            return
        
        if command[-1] == ':':
            self._add_function(command[:-1], len(self._binary) // 4)
            return
 
        if ' ' not in command:
            args = None
        else:
            command, args = command.split(' ', 1)
            args = args.split()
        self._process_command(command, args)

    def _add_function(self, function_name, pos):
        pos = STACK_SIZE + len(self._var_values) + pos
        logger.info(f'Adding function {function_name} at pos {pos}')
        self._func_pos[function_name] = pos
        self._binary += bytearray(4)

    def _process_command(self, command, args):
        byte_command = bytearray(4)
        meta = _get_command_meta(command)
        arg_types = meta.get('args')
        if (args is None) != (arg_types is None) or\
                args and len(args) != len(arg_types):
            logger.error(f'Command "{command}" must have arguments: '
                         f'{meta["args"]}; actual: {args}')
            raise KeyError('Command schema missmatched')
        
        byte_command[0] = meta['code']
        logger.debug(f'Command meta: {meta}')
        if args:
            byte_command[1: 3] = (
                self._get_argument_bytes(args[0], arg_types[0])
                    .to_bytes(2, byteorder='big')
                )
            if len(args) > 1:
                byte_command[3] = self._get_argument_bytes(args[1], arg_types[1])

        logger.info(f'Adding command {command} with args {args}')
        logger.debug(f'{byte_command}')
        self._binary += byte_command
            
    def _get_argument_bytes(self, value, _type):
        if _type == ArgumentType.CONST:
            return int(value)
        elif _type == ArgumentType.FUNC:
            print(value, _type)
            return int(self._func_pos[value])
        elif value in self._var_pos:
            return int(self._var_pos[value])
        else:
            logger.error(f'Unknown variable "{value}"')
            raise KeyError("Unknown variable")
        
    def _encode_variables(self, values):
        encoded = bytearray(len(values) * 4)
        for i, val in enumerate(values):
            if val is None:
                continue
            elif val.isdigit():
                left = i * 4
                encoded[left: left + 4] = _int_to_bytes(int(val))
            else:
                logger.warning('Unknown variable value type')
        self._binary = bytearray(STACK_SIZE * 4) + encoded + self._binary


def _int_to_bytes(x):
    return x.to_bytes(4, byteorder='big', signed=True)