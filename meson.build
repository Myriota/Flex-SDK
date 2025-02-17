project(
  'flex_sdk',
  'c',
  version: '2.2.4',
  meson_version: '>=0.61.2',
  default_options: [
    'c_std=gnu99',
    'warning_level=2',
    'werror=true',
    'buildtype=minsize',
 ]
)

if not meson.is_cross_build()
  error('Native builds not supported. Please ensure you have provided a cross file via \'--cross-file\'.')
endif

####  Begin Flex SDK Project Boilerplate ####

libflex_proj = subproject('libflex', required: false)
if not libflex_proj.found()
  error('Missing libflex subproject. Maybe you deleted it?')
endif

libflex_dep = libflex_proj.get_variable('libflex_dep')
system_image = libflex_proj.get_variable('system_image')

python = find_program('python3')
subdir('scripts')

flex_sdk_lib_deps = []
subdir('lib')

####  End Flex SDK Project Boilerplate ####

# If the project is being included as a subproject create the necessary
# flex-sdk dependency. Otherwise the project is being used as a project
# template so build the user_application in place.
if meson.is_subproject()
  flex_sdk_dep = declare_dependency(
    dependencies: [
      libflex_dep
    ] + flex_sdk_lib_deps,
  )
else
  # Add your projects source file to this array
  c_files = [
    'src/main.c'
  ]

  # Create the user_application as an ELF
  user_application_elf = executable('user_application',
    c_files,
    name_suffix: 'elf',
    dependencies: [
      libflex_dep
    ] + flex_sdk_lib_deps,
  )

  # Post process the ELF into Myriota desired bin format
  user_application_bin = custom_target('user_application.bin',
      output: [
        'user_application_raw.bin',
        'user_application_build_key.bin',
        'user_application.bin',
      ],
      input: user_application_elf,
      build_by_default: true,
      command: [python, post_process_elf, '@INPUT@', '@OUTPUT0@', '@OUTPUT1@', '@OUTPUT2@' ],
  )

  if get_option('examples')
    subdir('examples')
  endif
endif
