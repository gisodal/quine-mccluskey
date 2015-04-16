#!/usr/bin/env python

"""
Copyright (c) 2012 George Prekas <prekgeo@yahoo.com>

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
"""

"""
This class implements the Quine-McCluskey algorithm for minimization of boolean
functions.

Based on code from Robert Dick <dickrp@eecs.umich.edu> and Pat Maupin
<pmaupin@gmail.com>. Most of the original code was re-written for performance
reasons.

>>> qm = QM(['A','B'])

>>> qm.get_function(qm.solve([],[])[1])
'0'
>>> qm.get_function(qm.solve([1,3],[0,2])[1])
'1'
>>> qm.get_function(qm.solve([0,1,2,3],[])[1])
'1'
>>> qm.get_function(qm.solve([3],[])[1])
'(A AND B)'
>>> qm.get_function(qm.solve([0],[])[1])
'((NOT A) AND (NOT B))'
>>> qm.get_function(qm.solve([1,3],[])[1])
'A'
>>> qm.get_function(qm.solve([1],[3])[1])
'A'
>>> qm.get_function(qm.solve([2,3],[])[1])
'B'
>>> qm.get_function(qm.solve([0,2],[])[1])
'(NOT A)'
>>> qm.get_function(qm.solve([0,1],[])[1])
'(NOT B)'
>>> qm.get_function(qm.solve([1,2,3],[])[1])
'(A OR B)'
>>> qm.get_function(qm.solve([0,1,2],[])[1])
'((NOT B) OR (NOT A))'
"""

class QM:
  def __init__(self, variables):
    """
    Initialize the Quine-McCluskey solver.

    variables: a list of strings that are the names of the variables used in
    the boolean functions
    """

    self.variables = variables
    self.numvars = len(variables)

  def solve(self, ones, dc):
    """
    Executes the Quine-McCluskey algorithm and returns its results.

    ones: a list of indices for the minterms for which the function evaluates
    to 1
    dc: a list of indices for the minterms for which we do not care about the
    function evaluation

    returns: a tuple a,b; a is the complexity of the result and b is a list of
    minterms which is the minified boolean function expressed as a sum of
    products
    """

    # Handle special case for functions that always evaluate to True or False.
    if len(ones) == 0:
      return 0,'0'
    if len(ones) + len(dc) == 1<<self.numvars:
      return 0,'1'

    primes = self.compute_primes(ones + dc)
    return self.unate_cover(list(primes), ones)

  def compute_primes(self, cubes):
    """
    Find all prime implicants of the function.

    cubes: a list of indices for the minterms for which the function evaluates
    to 1 or don't-care.
    """

    sigma = []
    for i in xrange(self.numvars+1):
      sigma.append(set())
    for i in cubes:
      sigma[bitcount(i)].add((i,0))

    primes = set()
    groups = self.numvars+1

    while sigma:
      nsigma = []
      redundant = set()
      for c1, c2 in zip(sigma[:-1], sigma[1:]):
        nc = set()
        for a in c1:
          for b in c2:
            m = merge(a, b)
            if m != None:
              nc.add(m)
              redundant |= set([a, b])
        nsigma.append(nc)
      primes |= set(c for cubes in sigma for c in cubes) - redundant
      groups = groups - 1

      debug = True;
      if debug:
        print "groups: "+str(groups+1)
        #print "sigma: "+str(sigma)
        for i,c in zip(xrange(-1,len(sigma)+2),sorted(sigma)):
            print " * "+str(i)+" ("+str(len(c))+") : " #+str(sorted(c))
            for m,dc in sorted(c):
                stdout.write("("+"{0:b}".format(m)+","+"{0:b}".format(dc)+") ")
            print

        #print "  redundant: "+str(sorted(redundant))
        print "  primes   : "+str(set(c for cubes in sigma for c in cubes) - redundant)+"\n"
      sigma = nsigma

    print "primes: "+str(primes)+"\n"
    if debug:
      print "primes: "+str(primes)+"\n"
    return primes

  def unate_cover(self, primes, ones):
    """
    Use the prime implicants to find the essential prime implicants of the
    function, as well as other prime implicants that are necessary to cover
    the function. This method uses the Petrick's method, which is a technique
    for determining all minimum sum-of-products solutions from a prime implicant
    chart.

    primes: the prime implicants that we want to minimize.
    ones: a list of indices for the minterms for which we want the function to
    evaluate to 1.
    """

    chart = []
    for one in ones:
      column = []
      for i in xrange(len(primes)):
        if (one & (~primes[i][1])) == primes[i][0]:
          column.append(i)
      chart.append(column)

    covers = []
    if len(chart) > 0:
      covers = [set([i]) for i in chart[0]]
    for i in xrange(1,len(chart)):
      new_covers = []
      for cover in covers:
        for prime_index in chart[i]:
          x = set(cover)
          x.add(prime_index)
          append = True
          for j in xrange(len(new_covers)-1,-1,-1):
            if x <= new_covers[j]:
              del new_covers[j]
            elif x > new_covers[j]:
              append = False
          if append:
            new_covers.append(x)
      covers = new_covers

    min_complexity = 99999999
    for cover in covers:
      primes_in_cover = [primes[prime_index] for prime_index in cover]
      complexity = self.calculate_complexity(primes_in_cover)
      if complexity < min_complexity:
        min_complexity = complexity
        result = [primes_in_cover]
      elif complexity == min_complexity:
        result.append(primes_in_cover)

    return min_complexity,result

  def compare_term(self, term1, term2):
    for t1,t2 in zip(term1,term2):
      if (t1 == None and t2 != None) or (t1 < t2):
        return term1
      elif (t1 != None and t2 == None) or (t2 < t1):
        return term2

    return term1


  def calculate_complexity_term(self,minterm):
      mask = (1<<self.numvars)-1
      masked = ~minterm[1] & mask
      term_complexity = bitcount(masked)
      if term_complexity == 1:
        term_complexity = 0
      return term_complexity + bitcount(~minterm[0] & masked)

  def calculate_complexity(self, minterms):
    """
    Calculate the complexity of the given function. The complexity is calculated
    based on the following rules:
      A NOT gate adds 1 to the complexity.
      A n-input AND or OR gate adds n to the complexity.

    minterms: a list of minterms that form the function

    returns: an integer that is the complexity of the function

    >>> qm = QM(['A','B','C'])

    >>> qm.calculate_complexity([(1,6)])
    0
    >>> qm.calculate_complexity([(0,6)])
    1
    >>> qm.calculate_complexity([(3,4)])
    2
    >>> qm.calculate_complexity([(7,0)])
    3
    >>> qm.calculate_complexity([(1,6),(2,5),(4,3)])
    3
    >>> qm.calculate_complexity([(0,6),(2,5),(4,3)])
    4
    >>> qm.calculate_complexity([(0,6),(0,5),(4,3)])
    5
    >>> qm.calculate_complexity([(0,6),(0,5),(0,3)])
    6
    >>> qm.calculate_complexity([(3,4),(7,0),(5,2)])
    10
    >>> qm.calculate_complexity([(1,4),(7,0),(5,2)])
    11
    >>> qm.calculate_complexity([(2,4),(7,0),(5,2)])
    11
    >>> qm.calculate_complexity([(0,4),(7,0),(5,2)])
    12
    >>> qm.calculate_complexity([(0,4),(0,0),(5,2)])
    15
    >>> qm.calculate_complexity([(0,4),(0,0),(0,2)])
    17
    """

    complexity = len(minterms)
    if complexity == 1:
      complexity = 0
    mask = (1<<self.numvars)-1
    for minterm in minterms:
      masked = ~minterm[1] & mask
      term_complexity = bitcount(masked)
      if term_complexity == 1:
        term_complexity = 0
      complexity += term_complexity
      complexity += bitcount(~minterm[0] & masked)

    return complexity

  def get_function(self, minterms):
    """
    Return in human readable form a sum of products function.

    minterms: a list of minterms that form the function

    returns: a string that represents the function using operators AND, OR and
    NOT.
    """

    if isinstance(minterms,str):
      return minterms

    def parentheses(glue, array):
      if len(array) > 1:
        return ''.join(['(',glue.join(array),')'])
      else:
        return glue.join(array)

    or_terms = []
    for minterm in sorted(list(minterms)):
      and_terms = []
      for j in xrange(len(self.variables)):
        if minterm[0] & 1<<j:
          and_terms.append(self.variables[j])
        elif not minterm[1] & 1<<j:
          and_terms.append(u'\u00AC%s' % self.variables[j])
          #and_terms.append(u'\u00AC%s' % self.variables[j])
      or_terms.append(parentheses(u' \u2227 ', and_terms))
    return parentheses(u'  \u2228  ', or_terms)

def bitcount(i):
  """ Count set bits of the input. """

  res = 0
  while i > 0:
    res += i&1
    i>>=1
  return res

def is_power_of_two_or_zero(x):
  """
  Determine if an input is zero or a power of two. Alternative, determine if an
  input has at most 1 bit set.
  """

  return (x & (~x + 1)) == x

def merge(i, j):
  """ Combine two minterms. """

  #print "merge: "+str(i)+","+str(j);
  if i[1] != j[1]:
    return None
  y = i[0] ^ j[0]
  if not is_power_of_two_or_zero(y):
    return None
  return (i[0] & j[0],i[1]|y)


from optparse import *
from sys import *
def main():

    options = [
        Option('-d', '--dontcares', dest='dc', default='', help='comma-separated don\'t-cares', metavar='D'),
        Option('-o', '--ones', dest='ones', default='', help='comma-separated ones', metavar='O'),
        Option('-v', '--variables', dest='variables', default='', help='number of variables', metavar='V')
    ]

    f = IndentedHelpFormatter()
    def raw_format(s): return s + '\n'
    f.format_description = raw_format

    optparser = OptionParser(description=__doc__, formatter=f)
    optparser.add_options(options)
    opts, args = optparser.parse_args()
    if len(argv) == 1 or args:
        optparser.print_help()
        exit()

    dc = [int(i) for i in opts.dc.split(',') if i]
    ones = [int(i) for i in opts.ones.split(',') if i]
    variables = int(opts.variables)

    qm = QM([chr(code) for code in xrange(ord('a'),ord('a')+variables)])
    #stdout.write("variables: "+str(qm.variables)+'\n')
    soln = qm.solve(ones, dc)
    if len(soln) == 0:
        stdout.write('contradiction\n')
    elif len(soln) == 1 and soln[0].count('X') == len(soln[0]):
        stdout.write('tautology\n')
    else:
        for cover in soln[1]:
            stdout.write(str(soln[0])+":")
            for prime in sorted(list(cover)):
                stdout.write(" "+str(qm.calculate_complexity_term(prime))+":"+str(prime))
            stdout.write("\n")

            f = qm.get_function(cover)
            stdout.write(f+"\n")

if __name__ == '__main__':
  main()

