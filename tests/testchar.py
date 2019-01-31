#
# CONSOLE
# $Id$
#
# test character input handling
#

import Console

c = Console.getconsole()

c.title("character input")
c.text(0, 0, "CONSOLE -- character codes")
c.pos(0, 5)

c.cursor(0)

while 1:
    print repr(c.getchar()),
