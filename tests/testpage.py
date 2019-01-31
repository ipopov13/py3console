#
# CONSOLE
# $Id$
#
# blank the screen over and over again
#

import Console
import time

c = Console.getconsole(0)

t0 = time.time()

ink = 0

for char in range(256):
    c.title("ink = %d, char = %d" % (ink, char))
    c.page(ink, chr(char))
    ink = ink + 1
    if ink >= 31:
        ink = 0

print round((time.time() - t0) / (ink*char) * 1000, 3), "milliseconds per page"
