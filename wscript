#!/usr/bin/env python

# Copyright 2013, Sebastian Jeltsch (sjeltsch@kip.uni-heidelberg.de)
# Distributed under the terms of the LGPLv3 or newer.

import sys, os

APPNAME = 'boost-mongo'
VERSION = '1.0'

def options(opt):
    opt.load('g++')
    opt.load('boost')

def configure(cfg):
    cfg.load('g++')
    cfg.load('boost')

    cfg.check_boost(lib='serialization thread system filesystem',
        uselib_store='BOOST4MONGO')
    cfg.check_cxx(lib='mongoclient')
    cfg.check_cxx(lib='pthread')

def build(bld):
    bld(target          = 'boost-mongo_inc',
        export_includes = '.')

    # use either CXXFLAGS or fallback to defaults
    cxxflags = bld.env['CXXFLAGS']
    cxxflags = ['-O2', '-Wall', '-Wextra', '-pedantic', '-Wno-long-long',
            '-Wno-variadic-macros'] if not cxxflags else cxxflags

    bld.shlib(
        target          = 'boost-mongo',
        source          = bld.path.ant_glob('libs/**/*.cpp'),
        install_path    = None,
        use             = [
            'MONGOCLIENT',
            'BOOST4MONGO',
            'PTHREAD',
            'boost-mongo_inc'
            ],
        linkflags       = bld.env['LDFLAGS'] + [
            '-Wl,-soname,libboost-mongo.so.%s' % VERSION
            ],
        cxxflags        = cxxflags)

    # build shlib ldconfig conformingly
    bld.install_as('${PREFIX}/lib/libboost-mongo.so.' + VERSION,
        'libboost-mongo.so')
    bld.symlink_as('${PREFIX}/lib/libboost-mongo.so',
        'libboost-mongo.so.%s' % VERSION)

    # install headers
    for header in bld.path.ant_glob('boost/archive/**/*.(hpp|ipp)'):
        bld.install_as('${PREFIX}/include/%s' % header.path_from(bld.path), header)

def dist(dst):
    dst.base_name = '%s_%s' % (APPNAME, VERSION)
    dst.algo      = 'tar.gz'
    dst.excl      = ' **/waf-1.* **/.waf-1.* **/waf3-1.* **/.waf3-1.* ' \
            '**/*~ **/*.rej **/*.orig **/*.pyc  **/*.pyo **/*.bak ' \
            '**/*.swp **/.lock-w* **/.* **/build **/lib '
