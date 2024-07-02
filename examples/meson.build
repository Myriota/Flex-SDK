examples = [
  'analog',
  'blinky',
  'event',
  'gnss',
  'hwtest',
  'message',
  'pulse_counter',
  'rs485_rs232',
]

objcopy = find_program('objcopy')
xxd = find_program('xxd')

fs = import('fs')
foreach example: examples
  if fs.is_dir(example)
    c_args = []
    c_link_args = []
    c_files = []
    subdir(example)

    example_elf = executable(example,
        c_files,
        name_suffix: 'elf',
        c_args: c_args,
        link_args: c_link_args,
        dependencies: [ libflex_dep, ],
    )

    example_bin_no_build_key = custom_target(example,
        output : example + '.bin',
        build_by_default : true,
        command : [objcopy, ['-O', 'binary', '-S', example_elf.full_path(), '@OUTPUT@']],
        depends : example_elf
    )

    example_bin = custom_target(example + '.bin',
        output : example,
        input: buildkey_xxd_file,
        feed: true,
        build_by_default : true,
        command : [xxd, ['-r', '-', example_bin_no_build_key.full_path()]],
        depends : example_bin_no_build_key
    )
  endif
endforeach
