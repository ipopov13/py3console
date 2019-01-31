#!/usr/bin/env python
#
# Setup script
# $Id: //modules/console/setup.py#4 $
#
# Usage: python setup.py install
#

from distutils.core import setup, Extension
from glob import glob

setup(
    name="py3console",
    version="0.1a-20190131",
    author="Ivan Popov (ported the original form Fredrik Lundh)",
    author_email="fredrik@pythonware.com",
    description="Console -- a console driver for Windows 9X/NT/2K/XP ported to Python3",
    py_modules = ["Console"],
    scripts = glob("demo*.py"),
    ext_modules = [
        Extension("_wincon", ["_wincon.c"])
        ]
    )
