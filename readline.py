# *** THIS IS WORK IN PROGRESS ***
#
# CONSOLE
# $Id$
#
# Readline for Windows 9X/NT
#
# This module provides a simple command history and editing
# implementation based on the windows console driver.
#
# History:
# 99-01-06 fl  Created
#
# Written by Fredrik Lundh, January 1999.
#
# Copyright (c) 1999 by Secret Labs AB.
# Copyright (c) 1999 by Fredrik Lundh.
#
# fredrik@pythonware.com
# http://www.pythonware.com
#

# FIXME: the completer is not finished
# FIXME: the screen update breaks down if you exceed one line

import Console, sys

_completer = None

def set_completer(completer):
    global _completer
    _completer = completer

class Readline:

    def __init__(self):
        self.console = Console.getconsole(0)
        sys.stdout = sys.stderr = self.console
        self.console.cursor(0)
        self.history = []

    def write(self, string):
        self.console.write(string, 0x0E)

    def readline(self):

        s = ""
        t = ""

        i = 0

        c = self.console

        x, y = c.pos()
        w, h = c.size()

        while 1:

            # sanity
            i = min(i, len(s))

            # redraw the edit field
            c.pos(x, y)
            c.write(s, 0x0F)
            c.rectangle((x+len(s), y, x+w, y+1))
            c.pos(x+i, y)

            # FIXME: should handle line wrap here!

            # get event
            c.cursor(1)
            e = c.get()
            c.cursor(0)

            if e.type != "KeyPress":
                continue

            # interpret keyboard event
            if e.keysym == "Return":
                break
            elif e.keysym == "Escape":
                s = ""
            elif e.keysym == "BackSpace":
                if s:
                    s = s[:i-1] + s[i:]
                    i = i - 1
            elif e.keysym == "Delete":
                if s:
                    s = s[:i] + s[i+1:]
            elif e.keysym == "Home" or e.char == "\001":
                i = 0
            elif e.keysym == "End" or e.char == "\005":
                i = len(s)
            elif e.keysym == "Right" or e.char == "\006":
                i = min(i+1, len(s))
            elif e.keysym == "Left" or e.char == "\002":
                i = max(i-1, 0)
            elif e.keysym == "Tab":
                pass # FIXME: implement completion
            elif e.char == "\013":
                s = s[:i]
            elif e.char == "\003":
                return None # force caller to generate KeyboardInterrupt
            elif e.char == "\f":
                c.page()
                return "\n"
            elif e.char and e.char >= " ":
                s = s[:i] + e.char + s[i:]
                i = i + len(e.char)

        c.write("\r\n")

        if s != "\n":
            self.history.append(s)

        return s + "\n"

#
# install readline utility

handler = Readline()

Console.install_readline(handler)

try:
    import rlcompleter
except:
    pass
