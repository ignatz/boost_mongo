#!/usr/bin/env python

# Copyright 2013, Sebastian Jeltsch (sjeltsch@kip.uni-heidelberg.de)
# Use, modification and distribution is subject to the Boost Software
# License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import os, sys, re
from subprocess import Popen, PIPE

success = re.compile('.*No errors detected\..*')
color = {
        'red' : '\033[31m',
        'green' : '\033[32m',
        'yellow' : '\033[33m',
        'blue' : '\033[34m',
        'reset' : '\033[0m',
    }

def main():
    tests = os.listdir('./bin')
    err = 0
    for file in tests:
        proc = Popen('./bin/%s' % file,
                stdout=PIPE, stderr=PIPE)
        proc.wait()
        stderr = proc.stderr.read()

        if stderr and not success.match(stderr):
            print '%s%s%s :' % ( color['blue'],
                    file.upper(), color['reset'])
            print '%s\n' % stderr
            err+=1

    print '\n(%d/%d) %s%d%s tests failed' % (
            len(tests)-err, len(tests),
            color['red' if err else 'reset'],
            err, color['reset'])

    if err:
        sys.exit(1)


if __name__ == '__main__':
    main()
