#!/usr/bin/env python

# Copyright 2013, Sebastian Jeltsch (sjeltsch@kip.uni-heidelberg.de)
# Use, modification and distribution is subject to the Boost Software
# License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import os, sys, re, argparse
from subprocess import Popen, PIPE

class Color(object):
    ctrlseq = {
            'red' : '\033[31m',
            'green' : '\033[32m',
            'yellow' : '\033[33m',
            'blue' : '\033[34m',
            'reset' : '\033[0m',
        }

    def __init__(self, use_color=True):
        self.color = use_color

    def __call__(self, color):
        return self.ctrlseq[color] if self.color else ''


class TestRunner(object):

    @staticmethod
    def add_arguments(parser):
        parser.add_argument('--xml', action='store', default='build/xml',
                type=str, help='store xml test results in this directory')
        parser.add_argument('--color', default=1, type=int,
                help='colorize output')
        parser.add_argument('--quiet', action='store_true',
                help='write nothing to stdout')

    def __init__(self, args):
        self.args = args
        self.errorcnt = 0
        self.numtests = 0
        self.sink = file(os.devnull, 'wb') if args.quiet else sys.stdout
        self.color = Color(args.color)

        self.xmlout = args.xml
        if not os.path.exists(self.xmlout):
            os.makedirs(self.xmlout)

    def start(self, fname):
        self.numtests += 1

        proc = Popen([
            './bin/%s' % fname,
            '--log_format=xml',
            '--log_level=test_suite',
            '--log_sink=%s' % os.path.join(self.xmlout, '%s.xml'%fname)
            ], stdout=PIPE, stderr=PIPE)
        stderr = proc.stderr.read()

        success = re.compile('.*No errors detected.*', re.DOTALL)
        fail = not success.match(stderr)

        c = self.color
        self.sink.write('%(color)s%(prog)s%(reset)s :\n' % {
            'color' : c('red') if fail else c('green'),
            'prog' : fname,
            'reset' : c('reset')
            })

        if fail:
            self.errorcnt+=1
            for line in stderr.split('\n'):
                self.sink.write('\t%s\n' % line)

    def __enter__(self):
        return self

    def __exit__(self, type, value, traceback):
        e = self.errorcnt
        self.sink.write('%(color)s%(fail)s of %(num)s tests failed%(reset)s\n' % {
            'color' : self.color('red') if e else '',
            'fail' : e,
            'num' : self.numtests,
            'reset': self.color('reset'),
            })


def main():
    parser = argparse.ArgumentParser(formatter_class=\
            argparse.ArgumentDefaultsHelpFormatter)
    TestRunner.add_arguments(parser)
    args = parser.parse_args()

    with TestRunner(args) as runner:
        for test in os.listdir('bin'):
            runner.start(test)


if __name__ == '__main__':
    main()
