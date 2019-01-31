#
# CONSOLE
# $Id$
#
# show character set
#

import Console

c = Console.getconsole()

c.title("Console Character Set")

INK = 0x1f

c.page(INK)

c.text(0, 0, chr(0xfe) + " Console Character Set " + chr(0xfe), INK)
c.text(0, 1, chr(0xcd) * 80, INK)

for x in range(16):
    for y in range(16):
	char = x*16+y
	c.text(x*5, y+2, "%02x=%c" % (char, char), INK)

c.pos(0, -2)
