#
# CONSOLE
# $Id$
#
# Simple console interface
#
# This module provides a simple "console" interface for
# a number of platforms.
#
# History:
# 1999-01-05 fl  Created
# 2000-11-18 fl  Added support for Python 2.0
# 2001-01-03 fl  Minor fixes
#
# Written by Fredrik Lundh, January 1999.
#
# Copyright (c) 1999-2001 by Secret Labs AB.
# Copyright (c) 1999-2001 by Fredrik Lundh.
#
# fredrik@pythonware.com
# http://www.pythonware.com
#

import sys

try:
    if sys.platform == "win32":
        import _wincon
        Console = _wincon.Console
        install_readline = _wincon.install_readline
    else:
        from _cursescon import Console
except ImportError:
    raise ImportError("installation error: cannot find a console driver")

#
# console factory (if you don't want to redirect stdout/stderr,
# simply instantiate the Console class yourself)

def getconsole(buffer=1):
    """Get a console handle.

    If buffer is non-zero, a new console buffer is allocated and
    installed.  Otherwise, this returns a handle to the current
    console buffer"""
    c = Console(buffer)
    print("Init class done...")
    # try to redirect stdout and stderr
    try:
        print("Trying stdout...",sys.stdout.isatty())
#        if sys.stdout.isatty():
#            sys.stdout = c
    except:
        print("Failed stdout!")
        raise
    try:
        print("Trying stderr...", sys.stderr.isatty())
#        if sys.stderr.isatty():
#            sys.stderr = c
    except:
        print("Failed stderr!")
        raise

    return c
