if build_machine.system() == 'linux'
  cmake = import('cmake')
  subdir('RakNet')
  subdir('luabind-0.7.1')
  subdir('SDL2_gfx-1.0.4')
  rmlui_proj = cmake.subproject('RmlUi-4.2')
  rmlui_lib = rmlui_proj.dependency('RmlCore')
endif
