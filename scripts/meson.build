script_directory = meson.current_source_dir()
buildkey_script = find_program('buildkey.py', dirs: script_directory, required: false)
network_info_script = find_program('network_info.py', dirs: script_directory, required: false)

required_scripts = {
  'buildkey.py':  buildkey_script,
  'network_info.py': network_info_script,
}

foreach name, script: required_scripts
  if not script.found()
    error('Requried script "@0@" is missing from directory: "@1@"'.format(name, script_directory))
  endif
endforeach

network_info = custom_target('network_info',
    output: ['network_info'],
    command: [network_info_script, '@OUTPUT0@'],
)

buildkey_xxd_file = custom_target('buildkey.xxd',
    output: ['buildkey.xxd'],
    command: [buildkey_script, '@OUTPUT0@'],
)
