python = find_program('python3')

examples = [
  { 'name': 'analog', 'dir': 'analog', 'option': [], 'deps': []},
  { 'name': 'blinky', 'dir': 'blinky', 'option': [], 'deps': []},
  { 'name': 'digital', 'dir': 'digital', 'option': [], 'deps': []},
  { 'name': 'event', 'dir': 'event', 'option': [], 'deps': []},
  { 'name': 'gnss', 'dir': 'gnss', 'option': [], 'deps': []},
  { 'name': 'hwtest', 'dir': 'hwtest', 'option': [], 'deps': []},
  { 'name': 'message', 'dir': 'message', 'option': [], 'deps': []},
  { 'name': 'pulse_counter', 'dir': 'pulse_counter', 'option': [], 'deps': []},
  { 'name': 'rs232', 'dir': 'rs485_rs232', 'option': ['-DSERIAL_INTERFACE=@0@'.format(0)], 'deps': []},
  { 'name': 'rs485', 'dir': 'rs485_rs232', 'option': ['-DSERIAL_INTERFACE=@0@'.format(1)], 'deps': []},
  { 'name': 'modbus', 'dir': 'modbus', 'option': [], 'deps': [ modbus_dep ]},
]

fs = import('fs')
foreach example: examples
  if fs.is_dir(example['dir'])
    c_link_args = []
    c_args = example['option']
    c_files = [example['dir'] + '/main.c']

    example_elf = executable(example['name'],
        c_files,
        name_suffix: 'elf',
        c_args: c_args,
        link_args: c_link_args,
        dependencies: [ libflex_dep, ] + example['deps'],
    )

    example_bin = custom_target('@0@.bin'.format(example['name']),
        output: [
          '@0@_raw.bin'.format(example['name']),
          '@0@.app.bin'.format(example['name']),
          '@0@.bin'.format(example['name']),
        ],
        input: example_elf,
        build_by_default: true,
        command: [python, post_process_elf, '@INPUT@', '@OUTPUT0@', '@OUTPUT1@', '@OUTPUT2@' ],
    )
  endif
endforeach
