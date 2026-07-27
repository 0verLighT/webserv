project(
  'webserv', 
  'cpp',
  license: 'MIT',
  version: '0.0.1',
  meson_version: '>= 0.58.0'
)

sources = files(
  'src/main.cpp',
  'src/Server.cpp',
  'src/Client.cpp',
)

inc = include_directories('include')

executable(
  'webserv',
  sources,
  include_directories: inc,
  install: true,
)
