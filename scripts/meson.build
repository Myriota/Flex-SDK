objcopy = find_program('objcopy')

script_directory = meson.current_source_dir()
buildkey_script = find_program('buildkey.py', dirs: script_directory, required: false)
network_info_script = find_program('network_info.py', dirs: script_directory, required: false)
merge_binary_script = find_program('merge_binary.py', dirs: script_directory, required: false)

required_scripts = {
  'buildkey.py':  buildkey_script,
  'network_info.py': network_info_script,
  'merge_binary.py': merge_binary_script,
}

foreach name, script: required_scripts
  if not script.found()
    error('Requried script "@0@" is missing from directory: "@1@"'.format(name, script_directory))
  endif
endforeach

# Fetch network information
network_info_script = find_program('network_info.py')
network_info_cmd = [python, network_info_script, '@OUTPUT@']
if (get_option('cold_start'))
    network_info_cmd += ['--cold']
endif
network_info = configure_file(
  command: network_info_cmd,
  output: 'network_info',
)

# Create build key file
buildkey_xxd_file = configure_file(
    command: [python, buildkey_script, '@OUTPUT@'],
    output: 'buildkey.xxd',
)

# Write out the script to post process the User Application ELF
post_process_elf_conf = configuration_data()
post_process_elf_conf.set('objcopy', objcopy.full_path())
post_process_elf_conf.set('merge_binary', '@0@/merge_binary.py'.format(meson.current_source_dir()))
post_process_elf_conf.set('build_key_file', '@0@/buildkey.xxd'.format(meson.current_build_dir()))
post_process_elf_conf.set('network_info_file', '@0@/network_info'.format(meson.current_build_dir()))
post_process_elf = configure_file(
  input: 'post_process_elf.py.in',
  output: 'post_process_elf.py',
  configuration: post_process_elf_conf,
)
