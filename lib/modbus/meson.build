modbus_includes = include_directories('include')

modbus_files = files(
  'src/modbus.c',
)

modbus_lib = static_library('modbus',
  modbus_files,
  include_directories: modbus_includes,
)

modbus_dep = declare_dependency(
  include_directories: modbus_includes,
  link_with: modbus_lib,
)

compiler = meson.get_compiler('c', native: true)
cmocka_lib = compiler.find_library('cmocka', required: false)
if cmocka_lib.found()
    modbus_unit_tests = executable('modbus_unit_tests',
      modbus_files,
      native: true,
      c_args: [
        '-DMYRIOTA_MODBUS_UNIT_TESTS',
      ],
      include_directories: modbus_includes,
      dependencies: cmocka_lib,
    )

    test('modbus unit tests', modbus_unit_tests)
endif

flex_sdk_lib_deps += modbus_dep
