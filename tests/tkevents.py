from Tkinter import *

c = Canvas()
c.pack()

def echo(event):
    i = vars(event).items()
    i.sort()
    print i

c.focus_set()

c.bind("<KeyPress>", echo)
c.bind("<KeyRelease>", echo)
c.bind("<ButtonPress>", echo)
c.bind("<ButtonRelease>", echo)
c.bind("<Motion>", echo)

mainloop()
