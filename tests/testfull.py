#
# CONSOLE
# $Id$
#
# various tests
#

import Console

c = Console.getconsole()

INK = 0x1f

w, h = c.size()

c.title("testing")

c.rectangle((0, 0, w, h), INK)

c.text(0, 0, chr(0xfe) + " Windows Console Driver", INK)
c.text(0, 1, chr(0xcd) * 80, INK)

i = 0
for text in ("File", "Edit", "Image", "Help"):
    c.text(4, 4+i, "%d. "%i + text, INK)
    i = i + 1

c.pos(0, 20)
print c.pos()

# print "hello, world", w, h
