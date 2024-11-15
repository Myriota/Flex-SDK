project('libflex',
  'c',
  version: '1.4.0',
  meson_version: '>=0.61.2'
)

flex_options_c = configure_file(
  input: 'config/flex_options.c.in',
  output: 'flex_options.c',
  configuration : {
    'skip_gnss' : get_option('skip_gnss').to_string()
  },
)

ldscript = '@0@/ldscripts/APP.ld'.format(meson.current_source_dir())
ldscript_args = '-T@0@'.format(ldscript)
fs = import('fs')
if not fs.exists(ldscript)
  error('Missing linker script: "@0@". Maybe you deleted it?'.format(ldscript))
endif

system_image_version = '2.0.3-FlexSense'
system_image = '@0@/system_image/system_image-@1@.bin'.format(meson.current_source_dir(), system_image_version)
libflex = '@0@/lib/libflex-v@1@.a'.format(meson.current_source_dir(), meson.project_version())
foreach file: [system_image, libflex]
  if not fs.exists(file)
    error('Missing "@0@". Please run `python scripts/download_binaries.py` to download it.'.format(file))
  endif
endforeach

includes = include_directories('include')
libflex_dep = declare_dependency(
    sources: [
        flex_options_c,
    ],
    include_directories: includes,
    link_args: [
      ldscript_args,
      '-Wl,--whole-archive',
      libflex,
      '-Wl,--no-whole-archive',
    ],
)
