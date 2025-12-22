project('cameracli', 'cpp')

cmake = import('cmake')

inc = include_directories('external')

ccap_proj = cmake.subproject('ccap')

ccap_dep = ccap_proj.dependency('ccap')
ncurses_dep = dependency('ncurses')
spdlog_dep = dependency('spdlog')

deps = [ccap_dep, ncurses_dep, spdlog_dep]

executable('cameracli', 'src/cameracli.cpp',
    include_directories: inc,
    dependencies: deps
)
