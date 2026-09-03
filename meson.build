project(
  'webserv',
  'cpp',
  license: 'MIT',
  version: '0.0.1',
  meson_version: '>= 0.58.0'
)

sources = files(
  'src/CommonGatewayInterface.cpp',
  'src/Client.cpp',
  'src/http/HttpRequest.cpp',
  'src/http/HttpResponse.cpp',
  'src/http/HttpException.cpp',
  'src/http/httpUtils.cpp',
  'src/Logger.cpp',
  'src/RequestHandler.cpp',
  'src/Server.cpp',
  'src/TomlParser.cpp',
  'src/main.cpp',
  'src/utils.cpp',
)

inc = include_directories('include')

executable(
  'webserv',
  sources,
  include_directories: inc,
  install: true,
)
