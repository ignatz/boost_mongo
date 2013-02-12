#!/usr/bin/env python
import sys, os

def options(opt):
    opt.load('g++')
    opt.load('boost')

def configure(cfg):
    cfg.load('g++')
    cfg.load('boost')

    cfg.check_boost(
            lib='serialization system thread filesystem',
            uselib_store='BOOST4MONGO')

    cfg.check_cxx(
            lib='mongoclient',
            uselib_store='MONGO')

def build(bld):
    bld(
            target          = 'boost_mongo_inc',
            export_includes = '.',
    )

    bld.shlib(
            target          = 'boost_mongo',
            source          = bld.path.ant_glob('libs/**/*.cpp'),
            install_path    = 'lib',
            use             = [
                'boost_mongo_inc',
                'BOOST4MONGO',
                'MONGO',
                ],
            cxxflags        = [
                '-O2',
                '-Wall',
                '-Wextra',
                '-pedantic',
                '-Wno-long-long',
                '-Wno-variadic-macros',
                ],
    )
