#
# CONSOLE
# $Id$
#
# show available colors
#

import Console

c = Console.getconsole()

INK = 0x1f

c.title("Console Attributes")

c.page(INK)

c.text(0, 0, chr(0xfe) + " Console Attributes " + chr(0xfe), INK)
c.text(0, 1, chr(0xcd) * 80, INK)

#
# attribute masks:
#
# 0x01 = blue foreground
# 0x02 = green foreground
# 0x04 = red foreground
# 0x08 = highlight foreground
# 0x10 = blue background
# 0x20 = green background
# 0x40 = red background
# 0x80 = highlight background

for x in range(16):
    for y in range(16):
        attribute = x*16+y
        c.text(x*5, y+2, " %02x " % attribute, attribute)

c.pos(0, -2)

