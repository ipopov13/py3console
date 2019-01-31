#
# CONSOLE
# $Id$
#
# test scrolling
#

import Console
import time

c = Console.getconsole()

t0 = time.time()

w, h = c.size()

for i in range(h):
    c.text(0, i, chr(97+i)*w)

for ink in range(h-10):
    c.title("ink %d" % ink)
    c.scroll((5, 5, w-5, h-5), 0, -1, ink, "*")
    time.sleep(0.1)
