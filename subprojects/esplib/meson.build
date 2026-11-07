project('esplib', 'c')

esplib_inc = include_directories('include')

sources = []
subdir('src/mem')
subdir('src/stdio')
subdir('src/uart')
subdir('src/wdt')

esplib = static_library('esplib',
  sources,
  include_directories: esplib_inc,
  implicit_include_directories: false,
  c_args: ['-fno-tree-loop-distribute-patterns'],
  pic: false,
)

esplib_dep = declare_dependency(
  link_with: esplib,
  include_directories: esplib_inc,
)
