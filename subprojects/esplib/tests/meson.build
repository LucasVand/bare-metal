unity_dep = dependency('unity', native: true)

unity_sub = subproject('unity')

unity_gen = unity_sub.get_variable('gen_test_runner')

# Map test suite names to their C source files
tests = {
  'Heap': {'src': 'heap_test.c', 'suite': 'collections'},
  'Fixed Block Allocator': {'src': 'fb_test.c', 'suite': 'alloc'}
}

foreach name, info : tests
  runner_src = unity_gen.process(info['src'])

  exe = executable(
    name.underscorify(),
    [info['src'], runner_src],
    dependencies: [esplib_dep_native, unity_dep],
    native: true,
  )
  test(name, exe, suite: info.get('suite', 'default'))
endforeach

