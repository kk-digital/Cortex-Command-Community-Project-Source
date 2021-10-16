external_inc_dirs_common = include_directories(
  'include/common',
  'include/common/GL',
  'include/common/RakNet',
  'include/common/boost_1_75',
  'include/common/fmod',
  'include/common/glm',
  'include/common/luabind',)
if host_machine.system() == 'windows'
  external_inc_dirs_platform = include_directories(
    'include/windows',
    'include/windows/LZ4',)
endif
subdir('lib')
subdir('sources')
