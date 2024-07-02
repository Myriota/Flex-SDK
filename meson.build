project(
  'flex_sdk',
  'c',
  version: '1.0.0',
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

subdir('scripts')

####  End Flex SDK Project Boilerplate ####

# If the project is being included as a subproject create the necessary
# flex-sdk dependency. Otherwise the project is being used as a project
# template so build the user_application in palce.
if meson.is_subproject()
  flex_sdk_dep = declare_dependency(
    dependencies: [
      libflex_dep
    ],
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
    ],
  )

  # Strip the ELF into it's raw binary format
  objcopy = find_program('objcopy')
  user_application_no_buildkey = custom_target('user_application_no_buildkey',
    output : 'user_application.bin',
    build_by_default : true,
    command : [objcopy, ['-O', 'binary', '-S', user_application_elf.full_path(), '@OUTPUT@']],
    depends : user_application_elf
  )

  # Append a build key to the raw user_application
  # WARNING: This step is required and removing the build key will cause
  # undefined behaviour.
  xxd = find_program('xxd')
  user_application_bin = custom_target('user_application.bin',
    output : 'user_application',
    input: buildkey_xxd_file,
    feed: true,
    build_by_default : true,
    command : [xxd, ['-r', '-', user_application_no_buildkey.full_path()]],
    depends : user_application_no_buildkey
  )

  if get_option('examples')
    subdir('examples')
  endif
endif
