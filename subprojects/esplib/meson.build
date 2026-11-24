project('esplib', 'c')

cc = meson.get_compiler('c')
lib_c_args = []

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
  c_args: lib_c_args,
  pic: false,
)

linker_script = files('linker.ld')

dep_link_args = ['-T', meson.current_source_dir() / 'linker.ld', '-e', 'call_start_cpu0']
dep_link_args += ['-lclang_rt.builtins']

esplib_dep = declare_dependency(
  link_whole: esplib,
  include_directories: esplib_inc,
  link_args: dep_link_args,
)
