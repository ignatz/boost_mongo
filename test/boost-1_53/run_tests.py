#!/usr/bin/env python

# Copyright 2013, Sebastian Jeltsch (sjeltsch@kip.uni-heidelberg.de)
# Use, modification and distribution is subject to the Boost Software
# License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import os, sys, re
from subprocess import Popen, PIPE

success = re.compile('.*No errors detected.*', re.DOTALL)
color = {
        'red' : '\033[31m',
        'green' : '\033[32m',
        'yellow' : '\033[33m',
        'blue' : '\033[34m',
        'reset' : '\033[0m',
    }
xmlout = './build/xml'

def main():
    if not os.path.exists(xmlout):
        os.makedirs(xmlout)
    tests = os.listdir('./bin')

    errorcnt = 0
    for file in tests:
        proc = Popen([
            './bin/%s' % file, '--log_format=xml', '--log_level=test_suite',
            '--log_sink=%s' % os.path.join(xmlout, '%s.xml'%file)],
            stdout=PIPE, stderr=PIPE)
        stderr = proc.stderr.read()

        if stderr and not success.match(stderr):
            print '%s%s%s :' % ( color['blue'],
                    file.upper(), color['reset'])
            print '%s\n' % stderr
            errorcnt+=1

    print '(%d/%d) %s%d%s tests failed' % (
            len(tests)-errorcnt, len(tests),
            color['red' if errorcnt else 'reset'],
            errorcnt, color['reset'])


if __name__ == '__main__':
    main()
