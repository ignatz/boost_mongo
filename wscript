#!/usr/bin/env python

# Copyright 2013, Sebastian Jeltsch (sjeltsch@kip.uni-heidelberg.de)
# Distributed under the terms of the LGPLv3 or newer.

import sys, os

APPNAME = 'boost-mongo'
VERSION = '0.1.0'

def options(opt):
    opt.load('compiler_cxx')
    opt.load('boost')

def configure(cfg):
    cfg.load('compiler_cxx')
    cfg.load('boost')

    cfg.check_boost(lib='serialization thread system filesystem',
        uselib_store='BOOST4MONGO')
    cfg.check_cxx(lib='mongoclient')
    cfg.check_cxx(lib='pthread')

    # This non-standard installation behaviour is a HACK for some transitional
    # time. Note that, if you want to install the package at the system level
    # you can still explicitly specify the PREFIX var. For most unix style
    # systems the prefix should be '/usr/local'.
    cfg.env.PREFIX = cfg.env.PREFIX if os.getenv('PREFIX') else '.'

def build(bld):
    bld(target = 'boost-mongo_inc',
        export_includes = '.')

    # use either system CXXFLAGS, LDFLAGS or fall back to defaults
    cxxflags = bld.env['CXXFLAGS'] if bld.env['CXXFLAGS'] else [
            '-O2', '-Wall', '-Wextra', '-pedantic',
            '-Wno-long-long', '-Wno-variadic-macros']
    ldflags  = bld.env['LDFLAGS'] if bld.env['LDFLAGS'] else [ '-zdefs' ]

    bld.shlib(
        target = 'boost-mongo',
        source = bld.path.ant_glob('libs/**/*.cpp'),
        install_path = '${PREFIX}/lib',
        use = [
            'MONGOCLIENT',
            'BOOST4MONGO',
            'PTHREAD',
            'boost-mongo_inc'
            ],
        linkflags = ldflags + ['-Wl,-soname,libboost-mongo.so.%s' % VERSION],
        cxxflags = cxxflags,
        vnum = VERSION)

    # install headers
    for header in bld.path.ant_glob('boost/archive/**/*.(hpp|ipp)'):
        bld.install_as('${PREFIX}/include/%s' % header.path_from(bld.path), header)

def dist(dst):
    dst.base_name = '%s_%s' % (APPNAME, VERSION)
    dst.algo      = 'tar.gz'
    dst.excl      = ' **/waf-1.* **/.waf-1.* **/waf3-1.* **/.waf3-1.* ' \
            '**/*~ **/*.rej **/*.orig **/*.pyc  **/*.pyo **/*.bak ' \
            '**/*.swp **/.lock-w* **/.* **/build **/lib **/test'
