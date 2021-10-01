if build_machine.system() == 'linux'
  cmake = import('cmake')
  subdir('RakNet')
  subdir('luabind-0.7.1')
  subdir('SDL2_gfx-1.0.4')
  rml_options = cmake.subproject_options()
  rml_options.add_cmake_defines({'CMAKE_BUILD_TYPE': 'Debug'})
  rmlui_proj = cmake.subproject('RmlUi-4.2', options:rml_options)
  rmlui_lib = rmlui_proj.dependency('RmlCore')
  rmlui_debug = rmlui_proj.dependency('RmlDebugger')
  deps += [rmlui_lib, rmlui_debug]
endif
