#
# taken from Demo/scripts
#
# adapted for console display by Fredrik Lundh, November 2000

"""N queens problem.

The (well-known) problem is due to Niklaus Wirth.

This solution is inspired by Dijkstra (Structured Programming).  It is
a classic recursive backtracking approach.

"""

N = 8                                   # Default; command line overrides

import Console
console = Console.getconsole(0)

class Queens:

    def __init__(self, n=N):
        self.n = n
        self.reset()

    def reset(self):
        n = self.n
        self.y = [None]*n               # Where is the queen in column x
        self.row = [0]*n                # Is row[y] safe?
        self.up = [0] * (2*n-1)         # Is upward diagonal[x-y] safe?
        self.down = [0] * (2*n-1)       # Is downward diagonal[x+y] safe?
        self.nfound = 0                 # Instrumentation

    def solve(self, x=0):               # Recursive solver
        for y in range(self.n):
            if self.safe(x, y):
                self.place(x, y)
                if x+1 == self.n:
                    self.display()
                else:
                    self.solve(x+1)
                self.remove(x, y)

    def safe(self, x, y):
        return not self.row[y] and not self.up[x-y] and not self.down[x+y]

    def place(self, x, y):
        self.y[x] = y
        self.row[y] = 1
        self.up[x-y] = 1
        self.down[x+y] = 1

    def remove(self, x, y):
        self.y[x] = None
        self.row[y] = 0
        self.up[x-y] = 0
        self.down[x+y] = 0

    def display(self):
        self.nfound = self.nfound + 1
        if self.nfound == 1:
            console.page(0x20)
        else:
            console.home()
        print '+-' + '--'*self.n + '+'
        for y in range(self.n-1, -1, -1):
            print '|',
            for x in range(self.n):
                if self.y[x] == y:
                    print "Q",
                else:
                    print ".",
            print '|'
        print '+-' + '--'*self.n + '+'

def main():
    import sys
    n = N
    if sys.argv[1:]:
        n = int(sys.argv[1])
    q = Queens(n)
    q.solve()
    print("Found", q.nfound, "solutions.")

if __name__ == "__main__":
    main()
