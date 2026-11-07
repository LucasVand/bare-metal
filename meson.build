
project('baremetal', 'c', version: '1.0')

esplib_dep = subproject('esplib').get_variable('esplib_dep')

# This handles: xtensa-esp32-elf-gcc -nostdlib -T linker.ld main.c -o main.elf
my_elf = executable('main.elf',
  sources: ['src/main.c'],
  dependencies: esplib_dep,
  implicit_include_directories: false,
  link_args: ['-T', meson.current_source_dir() / 'linker.ld', '-lgcc'],
  # Ensure Meson tracks the linker script as a dependency
  link_depends: 'linker.ld' 
)

# Extract the esptool binary we defined in cross.ini
esptool = find_program('esptool')

# Convert the ELF to a firmware BIN image
# This handles: esptool --chip esp32 elf2image main.elf
my_bin = custom_target('firmware_image',
  input: my_elf,
  output: 'main.bin', 
  command: [
    esptool, 
    '--chip', 'esp32', 
    'elf2image', 
    '--flash-mode', 'dio',
    '--flash-freq', '40m',
    '--flash-size',  '4MB' ,
    '@INPUT@', 
    '--output', '@OUTPUT@' 
  ],
  build_by_default: true # Ensures this runs automatically when you type 'ninja'
)


# Add the Flash Target (Only runs when explicitly called)
# This mimics: esptool --chip esp32 write_flash 0x10000 builddir/main.bin
run_target('flash',
  command: [
    esptool,
    '--chip', 'esp32',
    'write-flash',
    '0x1000',      # The standard bare-metal factory app offset for ESP32
    my_bin          # Meson automatically resolves this to the compiled main.bin file path
  ]
)

run_target('image-info',
  command: [
    esptool,
    '--chip', 'esp32',
    'image-info',
    my_bin
]
)
