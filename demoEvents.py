#
# CONSOLE
# $Id$
#
# test event handling
#

import Console

c = Console.getconsole()

c.title("event codes")
c.text(0, 0, "CONSOLE -- event codes (click or type into window)")
c.pos(0, 5)

c.cursor(0)

while 1:
    e = c.get()
    d = {}
    for m in e.__members__:
        d[m] = getattr(e, m)
        print d
