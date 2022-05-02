project('getopt', 'c', default_options : ['c_std=c99'])

libgetopt_include = include_directories('include')
libgetopt = library('getopt', 'src/getopt.c', include_directories: libgetopt_include, install: true)
install_headers('include/getopt/getopt.h', subdir: 'getopt')

pkg = import('pkgconfig')
pkg.generate(libgetopt,
    filebase : 'getopt',
    name : 'getopt',
    description : 'C library to parse command-line options',
    url : 'github.com/wc-duck/getopt',
)
