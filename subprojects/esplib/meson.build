project('esplib', 'c')

esplib_inc = include_directories('include')
private_inc = include_directories('src/include')
xtensa_inc = include_directories('src/include/xtensa')


sources = [
  'src/entry.c',
]
subdir('src/mem')
subdir('src/stdio')
subdir('src/uart')
subdir('src/task')

esplib = static_library('esplib',
  sources,
  include_directories: [esplib_inc, private_inc, xtensa_inc],
  implicit_include_directories: false,
  c_args: ['-fno-tree-loop-distribute-patterns'],
  pic: false,
)

linker_script = files('linker.ld')

esplib_dep = declare_dependency(
  link_whole: esplib,
  include_directories: esplib_inc,
  link_args: ['-T', meson.current_source_dir() / 'linker.ld', '-lgcc', '-e', 'call_start_cpu0'],
)
