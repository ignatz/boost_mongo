#!/usr/bin/env python
import sys, os

def options(opt):
    opt.load('g++')
    opt.load('boost')

def configure(cfg):
    cfg.load('g++')
    cfg.load('boost')

    cfg.check_boost(lib='serialization', uselib_store='BOOST_SERIALIZATION')

def build(bld):
    bld (
            target          = 'boost_mongo',
            use             = ['BOOST_SERIALIZATION'],
            export_includes = '.',
    )
