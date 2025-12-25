project('cameracli', 'cpp')

cmake = import('cmake')

inc = include_directories('external')

ccap_proj = cmake.subproject('ccap')

ccap_dep = ccap_proj.dependency('ccap')
notcurses_raw = dependency('notcurses')

notcurses_inc = include_directories('/usr/include/notcurses')

notcurses_dep = declare_dependency(
    dependencies: notcurses_raw,
    include_directories: notcurses_inc
)

notcurses_cpp_dep = dependency('notcurses++')
spdlog_dep = dependency('spdlog')

deps = [ccap_dep, notcurses_dep, notcurses_cpp_dep, spdlog_dep]

executable('cameracli', 'src/cameracli.cpp',
    include_directories: inc,
    dependencies: deps
)
