/*
 *  Nearpi, a C program to exhibit large floating-point numbers
 *
 *                      Z = m * 2 ^ L 
 *
 *  very close to integer multiples of  pi / 2 ,
 *
 *     where  m  =   m0 / e ,  e  <=  m0  <=  f ,  and
 *            e  =  2 ^ (D - 1)
 *            f  =  2 ^ D - 1
 *            L  =  the binade
 *            D  =  significant figures in floating-point numbers.
 *
 *  Adapted by S. McDonald from a Basic program written by
 *  W. Kahan.
 *                                              -S.McD  6/21/83
 *                                              revised 6/ 7/84
 *
 *  One should consult the handout "Minimizing  q * m - n" below
 *  by W. Kahan, March 1983, for background material regarding
 *  this program.
 *
 *  This program was written for VAX double precision ( D = 56 ),
 *  but may be modified for VAX single or quad precision by appropriate
 *  modification of  D ,  MAX_EXP  and the output section.
 *
 *  With the exception of  p , q , x  and  z , all floating-point
 *  variables in this program are used as D-bit integers.
 *
 *  If you are implementing this algorithm on another machine,
 *  you won't need to use any floating-point numbers (except
 *  for  p , q , x  and  z ) if your machine has an integer type
 *  with at least D bits.  That is to say, this program deals almost
 *  entirely with integers (and will be slightly easier if you 
 *  use them); I'm using floating-point numbers because my machine's
 *  integers aren't wide enough.
 *
 *  In any case, it is remarkable that the same precision as the
 *  code you are trying to test, argument reduction for trigonometric
 *  functions, is sufficient to test the code!
 *    
 *  Simply stated, what happens is this:  a continued fraction 
 *  related to  pi  is repeatedly evaluated and doubled;  each
 *  evaluation produces all the integer multiples of  pi / 2
 *  between the current doubled value and the next.
 *
 *  For those of you with base sixteen machines, you are in luck.  
 *  Instead of doubling once after each evaluation, you double four
 *  times.  Decimal machines, alas, are another matter.  Indeed,
 *  it is possible to produce the continued fraction for five
 *  times a continued fraction, but it isn't as easy as doubling.
 *  Therefore, only the modifications necessary for base sixteen
 *  machines are noted throughout the program.  Of course "binade"
 *  should be read "B_ade" when the base, B , isn't binary.
 *
 *  In background article below, each doubled continued fraction
 *  was calculated afresh the hard way, i.e. for each binade
 *  a multi-precision number was doubled, then converted to a
 *  continued fraction.
 *
 *  Prof. Kahan soon discovered, though, that the doubling needn't
 *  use any multi-precision floating-point arithmetic.  A method
 *  for doubling due to A. Hurwitz (1891) can be used instead.
 *                                                              -S.McD
 *
 *
 *  Minimizing  q * m  -  n                W. Kahan, March 1983 
 *  -------------------------
 *  
 *  Notation:   Letters  i, j, k, l, m, n  denote integer
 *              variables.
 *              B  =  radix of floating-point arithmetic with
 *              d  =  no. of sig.  B-digits
 *  
 *  
 *  Problem:  Given an irrational real number  q,  we seek the several
 *  closest rational approximations  n / m  to  q  with  m  confined
 *  to a prescribed interval.  For example, to compute trigonometric
 *  functions in floating-point we must perform "Argument Reduction";
 *  given a floating-point number  B ^ l * m  with 
 *  
 *               d - 1              d
 *              B       =<  m  =<  B  - 1
 *  
 *  we seek an integer  n  such that 
 *  
 *               l
 *              B  * m  =  (n + x) pi / 2    with  |x|  =<  1 / 2  ,
 *  
 *  and for full accuracy we must compute the reduced argument
 *  
 *                        l
 *              x  =  (2 B  / pi) m - n
 *  
 *  to full precision (d sig. digits) or better despite cancellation.
 *  For what values  m  in the allowed range will x be unusually tiny
 *  and hence hard to compute accurately?  In this example
 *  
 *                       l
 *              q  =  2 B  / pi
 *  
 *  is closely approximated by
 *  
 *              n / m  =  q -  x / m
 *  
 *  just when  x  is very tiny.
 *  
 *      To solve this problem we shall use the continued fraction
 *
 *                        1
 *      q  =  i  +  -------------
 *             0                1
 *                  i  +  -------------
 *                   1                1
 *                        i  +  -------------
 *                         2                1
 *                              i  +  -------------
 *                               3                1
 *                                    i  +  -------------
 *                                     4        . . .
 *  
 *  
 *  where either        every   i    >=  1      except possibly  i  ,
 *                               k                                0
 *  
 *      or              every  |i |  >=  2      except possibly  i  ,
 *                               k                                0
 *  
 *  though usually  i  will turn out to be enormous.
 *                   0
 *  
 *  Associated with  q  are certain sequences calculated by
 *  recurrences:
 *  
 *  Let  q  := q ,  p  := l  := j  := 1 ,  j  := l  := 0 ,  ( l  := i  ,)
 *        0          0     0     -1         0     1           -1     0
 *  
 *  and for  k = 0, 1, 2, 3, ...        in turn
 *  
 *  i   := ( q  chopped or rounded to an integer ),     ( l  := i  ,)
 *   k        k                                            -1    0
 *  
 *  q   := 1 / ( q  - i  )      , ... so  q  = i  + 1 / q    ,
 *   k+1          k    k                   k    k        k+1
 *                                                                k + 1
 *  p   := -q   p               , ...  =  q   q  q  ...  q  * (-1)  ,
 *   k+1     k+1 k                         k+1 k  k-1     1
 *  
 *  j   := j  - i  j            (so  j  = 1 )
 *   k+1    k-1  k  k                 1
 *  
 *  l   := l  - i  l            (so  l  = 0 )   [ not normally
 *   k+1    k-1  k  k                 1           calculated ]
 *  
 *  
 *  Note that, since all  |q |  >  1  for  k  >  0, so  |p | --> oo  as k --> oo
 *                          k                             k
 *                                                  k/2
 *                              (In fact  |p |  >  2    .)
 *                                          k
 *  
 *
 *  Lemma  0 :  j  - j  / q   =  p
 *  ---------    k+1  k    k+1    k
 *  
 *  Proof:      At  k = 0  we find  j  - j  / q   =  1 - 0 / q   =  1  =  p 
 *                                   1    0    1               1           0
 *  
 *  as claimed, and by induction for  k  >  0  we find, assuming
 *  
 *  j  - j  / q   =  p , that also  j  - j  / q =  j  - ( i  + 1 / q  ) j 
 *   k    k-1  k      k-1            k+1  k    k+1  k-1    k        k+1  k
 *  
 *                                              =  j  - q  j
 *                                                  k-1  k  k
 *  
 *                                              =  -q  ( j  - j  / q )
 *                                                   k    k    k-1  k
 *  
 *                                              =  -q  p
 *                                                   k  k-1
 *  
 *                                              =  p  .                       []
 *                                                  k
 *  
 *
 *  Next we relate  m, n  and  x  to recurrences:
 *  
 *      m  := n ,  m  := m ,  x  := x ,
 *       -1         0          0
 *  
 *      and for  k = 0, 1, 2, 3, ...    in turn
 *  
 *      m   := m  - i  m                        m   =  m  + i  m
 *       k+1    k-1  k  k       invertible       k-1    k+1  k  k
 *                                 via
 *      x   := m   q  -  m                      x   =  m  q - m
 *       k+1    k+1 k+1   k                      k      k  k   k-1
 *  
 *
 *                       m           j     l       m
 *                        k+1         k+1   k+1     1
 *  Lemma  1 :         (     )  =  (           ) (    )         and
 *  ---------            m           j     l       m
 *                        k           k     k       0
 *  
 *                       m                 l    -l       m
 *                        1            k    k     k+1     k+1
 *                     (     )  =  (-1)  (           ) (     )
 *                       m                -j     j       m
 *                        0                 k     k+1     k
 *
 *                                      j     l
 *                                       k+1   k+1       1  0
 *  Proof:      When  k = 0  we find  (           )  =  (    )  as expected.
 *                                      j     l          0  1
 *                                       k     k
 *  
 *                   m              -i         m
 *                    k+1             k   1     k
 *  For  k  >  0,  (     )      =  (       ) (     )
 *                   m               1    0    m
 *                    k                         k-1
 *  
 *                          -i         j     l      m
 *                            k   1     k     k      1
 *                      =  (       ) (           ) (  ) etc. by induction.
 *                           1    0    j     l      m
 *                                      k-1   k-1    0
 *  
 *                    j     l                       j     l
 *                     k+1   k+1          k          k+1   k+1  -1
 *  Similarly,  det (           )  =  (-1)  , so  (           )
 *                    j     l                       j     l
 *                     k     k                       k     k
 *
 *                                                         l    -l
 *                                                     k    k     k+1
 *                                              =  (-1)  (           )  .     []
 *                                                        -j     j
 *                                                          k     k+1
 *
 *
 *  Lemma  2 :  x   =  -q   x   =  p   x
 *  ---------    k+1     k+1 k      k+1 0
 *
 *  Proof:      x       =  q   ( m  - m  / q   )
 *               k+1        k+1   k+1  k    k+1
 *
 *                      =  q   ( m  - m  ( i  + 1 / q   ) )
 *                          k+1   k-1  k    k        k+1
 *
 *                      =  q   ( m  - m  q )
 *                          k+1   k-1  k  k
 *
 *                      =  -q   x       =  (p  / p ) x  =  p   (x  / p )
 *                           k+1 k           k+1  k   k     k+1  0    0
 *
 *                      =  p   x        by induction.                         []
 *                          k+1 0
 *
 *
 *                                                      k+1
 *  Lemma  3 :  m   =  -x  + m  / q     =  -j   x  + (-1)  m  / p 
 *  ---------    k+1     k    k    k+1       k+1 0          0    k+1
 *
 *  Proof:      m   =  ( m  + x  ) / q  =  -x  + m  / q    by lemma 2, so
 *               k+1      k    k+1    k+1    k    k    k+1
 *
 *    m             1 / q           m
 *     k+1               k+1   -1    k
 *  (      )    =  (             ) (  )
 *    x              0       -q     x
 *     k+1                     k+1   k
 *                                         k
 *                  -p / p             (-1)  / p     -j     m
 *                    k   k+1     -1            k      k     0
 *              =  (                ) (                  ) (  )
 *                   0       p  / p     0             p     x
 *                            k+1  k                   k     0
 *
 *                      k+1
 *                  (-1)  / p      j  p / p  - p    m
 *                           k+1    k  k   k+1  k    0
 *              =  (                             ) (  )
 *                   0                    p         x
 *                                         k+1       0
 *
 *  provided this lemma is true for some  k  >=  0  [ as it is for
 *  k = 0  since  m     =  -j  x  - m  / p      =  -x  + m  / q
 *                 1         1  0    0    1          0    0    1
 *
 *                      =  ( x  + m  ) / q      . ]
 *                            1    0      1
 *
 *  And the proof will be complete by induction if
 *      j  p  / p  - p  =  -j   ; this follows from lemma 0 .                 []
 *       k  k    k+1  k      k+1
 *
 *
 *                                      k
 *  Corollary :      x  =  x    =  ((-1)  m  / p  - m ) / j     =  x  / p
 *  ----------              0              0    k    k     k        k    k
 *
 *                              =  ( m  q  - m   ) / p
 *                                    k  k    k-1     k
 *
 *                                     k
 *              and so     m    =  (-1)  ( ( p  + q  j  ) m  - j  m   )
 *                          0                 k    k  k    k    k  k-1
 *                                     k
 *                              =  (-1)  ( j   m  - j  m   )
 *                                          k-1 k    k  k-1
 *
 *  [ By lemma 0,  p  +  q  j   =  j   +  j  ( q  - 1 / q   )
 *                  k     k  k      k+1    k    k        k+1
 *
 *                              =  j  + i  j    =  j    alright. ]            []
 *                                  k+1  k  k       k-1
 *
 *
 *  Application to the Problem
 *  ---------------------------
 *
 *  Since  m  =  m  is confined to some interval, namely
 *          0
 *                       d - 1               d
 *                      B       =<  m   =<  B  - 1  , yet  |p | --> oo ,
 *                                                           k
 *  so there must be some  k  =  K  for which the interval
 *  containing  m / p  contains just a few integers,
 *                   k
 *      i.e.               d    d - 1
 *                      ( B  - B      - 1 ) / |p |  =<  a few.
 *                                              k
 *                                         k
 *  For that  k  =  K  we find  x  =  ((-1) m / p  - m  ) / j
 *                                               k    k      k
 *
 *  is smallest when  m  is chosen to be one of the few integers
 *                     k
 *                  k
 *  over which  (-1) m / p   may range, i.e.  roughly between
 *                        k
 *          k  d - 1                        k    d
 *      (-1)  B      / p        and     (-1)  ( B  - 1 ) / p  .
 *                      k                                   k
 *  Having chosen  m   to be one of those few integers, we
 *                  k
 *
 *  observe that  x  =  ( m  q  - m   ) / p   is tiniest when  m
 *                         k  k    k-1     k                    k-1
 *
 *  is one of the integers nearest  m  q  , and then
 *                                   k  k
 *
 *                                       d    d - 1
 *  |x|  <  1 / |p |    =<  (a few) / ( B  - B      - 1 )  .
 *                k
 *
 *  Indeed,  |x|  may be much tinier than  1 / |p |  if  m  q  is
 *                                               k        k  k
 *
 *  very nearly an integer.  Finally, having chosen  m   and  m
 *                                                    k        k-1
 *                        k
 *  we obtain  m   =  (-1)  ( j   m  - j  m   )  ; provided this  m
 *              0              k-1 k    k  k-1                     0
 *
 *  lies in the range allowed for  m  we have a solution.
 *  Moreover, all solutions must be obtained in this way.
 *  
 *
 *  Example
 *  --------
 *              B  =  10,  d  =  4      so  1000  =<  m  =<  9999  .
 *
 *                      q       =  200 / pi     =  63.66197723 ...
 *
 *
 *              q          i            p          j
 *   k  |        k      |   k   |        k      |   k   |
 *  ----+---------------+-------+---------------+-------+
 *   0  |  63.66197723  |  64   |       1       |   0   |
 *      |               |       |               |       |
 *   1  | -2.958380585  |  -3   |  2.958380585  |   1   |
 *      |               |       |               |       |
 *   2  |  24.02724786  |  24   | -71.08174358  |   3   |
 *      |               |       |               |       |
 *   3  |  36.70012985  |  37   | -2608.709219  |  -71  |
 *      |               |       |               |       |
 *   4  | -3.334776736  |  -3   |  8699.462815  |  2630 |
 *  ----+---------------+-------+---------------+-------+
 *
 *  K = 4 puts                                  k
 *              0.115   =  1000 / p     =<  (-1)  m  / p
 *                                 k                    k
 *                                      =<  9999 / p
 *                                                  k
 *                                      =   1.149  .
 *
 *  Thus
 *              m       =       0       or      1  ,
 *               k
 *  and so
 *              m  q    =       0       or      -3.3347  .
 *               k  k
 *  Hence
 *              m       =       1       or      {       -3      or      -4 }  .
 *               k-1
 *
 *  Therefore
 *              m       =       j    m  - j  m
 *               0               k-1  k    k  k-1
 *
 *                      =       -2630   or      {       7819    or      10449 }
 *                                                                      -----
 *  And finally
 *              m  q    =       -167431.0001    497772.9999     or   665204.001
 *               0                                                   ----------
 *                      =       n + x  ,
 *
 *  yielding two values since the third is slightly out of range.
 */

/*
 * Global macro definitions.
 */

# define hex( double )  *(1 + ((long *) &double)), *((long *) &double)
# define sgn(a)         (a >= 0 ? 1 : -1)
# define MAX_k          2500
# define D              56
# define MAX_EXP        127
# define THRESHOLD      2.22e-16

/*
 *  Global Variables
 */

int     CFlength,               /* length of CF including terminator */
        binade;
double  e,
        f;                      /* [e,f] range of D-bit unsigned int of f;
                                   form 1X...X */


/*
 *  This is the start of the main program.
 */

main ()
{
    int     k;                  /* subscript variable */
    double  i[MAX_k],
            j[MAX_k];           /* i and j are continued fractions
                                   (coeffs) */


/*
 *  Compute global variables e and f, where
 *
 *      e = 2 ^ (D-1), i.e. the D bit number 10...0
 *  and
 *      f = 2 ^ D - 1, i.e. the D bit number 11...1  .
 */

    e = 1;
    for (k = 2; k <= D; k = k + 1)
        e = 2 * e;
    f = 2 * e - 1;

 /* 
  *  Compute the continued fraction for  (2/e)/(pi/2)  , i.e.
  *  q's starting value for the first binade, given the continued
  *  fraction for  pi  as input; set the global variable CFlength
  *  to the length of the resulting continued fraction (including
  *  its negative valued terminator).  One should use as many 
  *  partial coefficients of  pi  as necessary to resolve numbers
  *  of the width of the underflow plus the overflow threshold.
  *  A rule of thumb is 0.97 partial coefficients are generated
  *  for every decimal digit of  pi .
  *
  *  Note: for radix B machines, subroutine  input  should compute
  *  the continued fraction for  (B/e)/(pi/2)  where  e = B ^ (D - 1).
  */

    input (i);

/*
 *  Begin main loop over all binades:
 *  For each binade, find the nearest multiples of pi/2 in that binade.
 *
 *  [ Note: for hexadecimal machines ( B = 16 ), the rest of the main
 *  program simplifies(!) to
 *
 *                      B_ade = 1;
 *                      while (B_ade < MAX_EXP)
 *                      {
 *                          dbleCF (i, j);
 *                          dbleCF (j, i);
 *                          dbleCF (i, j);
 *                          CFlength = dbleCF (j, i);
 *                          B_ade = B_ade + 1;
 *                      }
 *                  }
 *
 *  because the alternation of source & destination are no longer necessary. ]
 */

    binade = 1;
    while (binade < MAX_EXP)
    {

/*
 *  For the current (odd) binade, find the nearest multiples of pi/2.
 */

        nearPiOver2 (i);

/*
 *  Double the continued fraction to get to the next (even) binade.
 *  To save copying arrays, i and j will alternate as the source
 *  and destination for the continued fractions.
 */

        CFlength = dbleCF (i, j);
        binade = binade + 1;

/*
 *  Check for main loop termination again because of the
 *  alternation.
 */

        if (binade >= MAX_EXP)
            break;

/*
 *  For the current (even) binade, find the nearest multiples of pi/2.
 */

        nearPiOver2 (j);

/*
 *  Double the continued fraction to get to the next (odd) binade.
 */

        CFlength = dbleCF (j, i);
        binade = binade + 1;
    }
}                               /* end of Main Program */

/*
 *  Subroutine  DbleCF  doubles a continued fraction whose partial
 *  coefficients are i[] into a continued fraction j[], where both
 *  arrays are of a type sufficient to do D-bit integer arithmetic.
 *
 *  In my case ( D = 56 ) , I am forced to treat integers as double
 *  precision reals because my machine does not have integers of
 *  sufficient width to handle D-bit integer arithmetic.
 *
 *  Adapted from a Basic program written by W. Kahan.
 *
 *  Algorithm based on Hurwitz's method of doubling continued
 *  fractions (see Knuth Vol. 3, p.360).
 *
 *  A negative value terminates the last partial quotient.
 *
 *  Note:  for the non-C programmers, the statement  break
 *  exits a loop and the statement  continue  skips to the next
 *  case in the same loop.
 *
 *  The call  modf ( l / 2, &l0 )  assigns the integer portion of
 *  half of L to L0.
 */

dbleCF (i, j)
double  i[],
        j[];
{
    register double k,
                    l,
                    l0,
                    j0;
    register int    n,
                    m;
    n = 1;
    m = 0;
    j0 = i[0] + i[0];
    l = i[n];
    while (1)
    {
        if (l < 0)
        {
            j[m] = j0;
            break;
        };
        modf (l / 2, &l0);
        l = l - l0 - l0;
        k = i[n + 1];
        if (l0 > 0)
        {
            j[m] = j0;
            j[m + 1] = l0;
            j0 = 0;
            m = m + 2;
        };
        if (l == 0)
/*
 *  Even case.
 */
            if (k < 0)
            {
                m = m - 1;
                break;
            }
            else
            {
                j0 = j0 + k + k;
                n = n + 2;
                l = i[n];
                continue;
            };
/*
 *  Odd case.
 */
        if (k < 0)
        {
            j[m] = j0 + 2;
            break;
        };
        if (k == 0)
        {
            n = n + 2;
            l = l + i[n];
            continue;
        };
        j[m] = j0 + 1;
        m = m + 1;
        j0 = 1;
        l = k - 1;
        n = n + 1;
        continue;
    };
    m = m + 1;
    j[m] = -99999;
    return (m);
}

/*
 *  Subroutine  input  computes the continued fraction for
 *  (2/e) / (pi/2) , where  e = 2 ^ (D-1) , given  pi 's
 *  continued fraction as input.  That is, double the continued 
 *  fraction of  pi   D-3  times and place a zero at the front.
 *
 *  One should use as many partial coefficients of  pi  as
 *  necessary to resolve numbers of the width of the underflow
 *  plus the overflow threshold.  A rule of thumb is  0.97
 *  partial coefficients are generated for every decimal digit
 *  of  pi .  The last coefficient of  pi  is terminated by a
 *  negative number.
 *
 *  I'll be happy to supply anyone with the partial coefficients
 *  of  pi .  My ARPA address is  mcdonald@ucbdali.BERKELEY.ARPA .
 *
 *  I computed the partial coefficients of  pi  using a method of
 *  Bill Gosper's.  I need only compute with integers, albeit
 *  large ones.  After writing the program in  bc  and  Vaxima  ,
 *  Prof. Fateman suggested  FranzLisp .  To my surprise, FranzLisp
 *  ran the fastest!  the reason?   FranzLisp's  Bignum  package is
 *  hand coded in assembler.  Also,  FranzLisp  can be compiled.
 *
 *
 *  Note: for radix B machines, subroutine  input  should compute
 *  the continued fraction for  (B/e)/(pi/2)  where  e = B ^ (D - 1).
 *  In the case of hexadecimal ( B = 16 ), this is done by repeated
 *  doubling the appropriate number of times.
 */

input (i)
double  i[];
{
    int     k;
    double  j[MAX_k];

/*
 *  Read in the partial coefficients of  pi  from a precalculated file
 *  until a negative value is encountered.
 */

    k = -1;
    do
    {
        k = k + 1;
        scanf ("%E", &i[k]);
    } while (i[k] >= 0);

/*
 *  Double the continued fraction for  pi  D-3  times using
 *  i  and  j  alternately as source and destination.  On my
 *  machine  D = 56  so  D-3  is odd; hence the following code:
 *
 *  Double twice  (D-3)/2  times,
 */
    for (k = 1; k <= (D - 3) / 2; k = k + 1)
    {
        dbleCF (i, j);
        dbleCF (j, i);
    };
/*
 *  then double once more.
 */
    dbleCF (i, j);

/*
 *  Now append a zero on the front (reciprocate the continued
 *  fraction) and the return the coefficients in  i .
 */

    i[0] = 0;
    k = -1;
    do
    {
        k = k + 1;
        i[k + 1] = j[k];
    } while (j[k] >= 0);

/*
 *  Return the length of the continued fraction, including its
 *  terminator and initial zero, in the global variable CFlength.
 */

    CFlength = k;
}

/*
 *  Given a continued fraction's coefficients in an array  i ,
 *  subroutine  nearPiOver2  finds all machine representable
 *  values near a integer multiple of  pi/2  in the current binade.
 */

nearPiOver2 (i)
double  i[];
{
    int     k,                  /* subscript for recurrences    (see
                                   handout) */
            K;                  /* like  k , but used during cancel. elim.
                                   */
    double  p[MAX_k],           /* product of the q's           (see
                                   handout) */
            q[MAX_k],           /* successive tail evals of CF  (see
                                   handout) */
            j[MAX_k],           /* like convergent numerators   (see
                                   handout) */
            tmp,                /* temporary used during cancellation
                                   elim. */
            mk0,                /* m[k - 1]                     (see
                                   handout) */
            mk,                 /* m[k] is one of the few ints  (see
                                   handout) */
            mkAbs,              /* absolute value of m sub k              
                                */
            mK0,                /* like  mk0 , but used during cancel.
                                   elim. */
            mK,                 /* like  mk  , but used during cancel.
                                   elim. */
            z,                  /* the object of our quest (the argument) 
                                */
            m0,                 /* the mantissa of z as a D-bit integer   
                                */
            x,                  /* the reduced argument         (see
                                   handout) */
            ldexp (),           /* sys routine to multiply by a power of
                                   two  */
            fabs (),            /* sys routine to compute FP absolute
                                   value   */
            floor (),           /* sys routine to compute greatest int <=
                                   value   */
            ceil ();            /* sys routine to compute least int >=
                                   value   */

 /* 
  *  Compute the q's by evaluating the continued fraction from
  *  bottom up.
  *
  *  Start evaluation with a big number in the terminator position.
  */

    q[CFlength] = 1.0E + 30;

    for (k = CFlength - 1; k >= 0; k = k - 1)
        q[k] = i[k] + 1 / q[k + 1];

/*
 *  Let  THRESHOLD  be the biggest  | x |  that we are interesed in
 *  seeing.
 *
 *  Compute the p's and j's by the recurrences from the top down.
 *
 *  Stop when
 *
 *        1                          1
 *      -----   >=  THRESHOLD  >   ------    .
 *      2 |j |                     2 |j  |
 *          k                          k+1
 */

    p[0] = 1;
    j[0] = 0;
    j[1] = 1;
    k = 0;
    do
    {
        p[k + 1] = -q[k + 1] * p[k];
        if (k > 0)
            j[1 + k] = j[k - 1] - i[k] * j[k];
        k = k + 1;
    } while (1 / (2 * fabs (j[k])) >= THRESHOLD);

/*
 *  Then  mk  runs through the integers between
 *
 *                  k        +                   k        +
 *              (-1)  e / p  -  1/2     &    (-1)  f / p  -  1/2  .
 *                         k                            k 
 */

    for (mkAbs = floor (e / fabs (p[k]));
            mkAbs <= ceil (f / fabs (p[k])); mkAbs = mkAbs + 1)
    {

        mk = mkAbs * sgn (p[k]);

/*
 *  For each  mk ,  mk0  runs through integers between
 *
 *                    +
 *              m  q  -  p  THRESHOLD  .
 *               k  k     k
 */

        for (mk0 = floor (mk * q[k] - fabs (p[k]) * THRESHOLD);
                mk0 <= ceil (mk * q[k] + fabs (p[k]) * THRESHOLD);
                mk0 = mk0 + 1)
        {

/*
 *  For each pair  { mk ,  mk0 } , check that
 *
 *                             k
 *              m       =  (-1)  ( j   m  - j  m   )
 *               0                  k-1 k    k  k-1
 */
            m0 = (k & 1 ? -1 : 1) * (j[k - 1] * mk - j[k] * mk0);

/*
 *  lies between  e  and  f .
 */
            if (e <= fabs (m0) && fabs (m0) <= f)
            {

/*
 *  If so, then we have found an
 *
 *                              k
 *              x       =  ((-1)  m  / p  - m ) / j
 *                                 0    k    k     k
 *
 *                      =  ( m  q  - m   ) / p  .
 *                            k  k    k-1     k
 *
 *  But this later formula can suffer cancellation.  Therefore,
 *  run the recurrence for the  mk 's  to get  mK  with minimal
 *   | mK | + | mK0 |  in the hope  mK  is  0  .
 */
                K = k;
                mK = mk;
                mK0 = mk0;
                while (fabs (mK) > 0)
                {
                    p[K + 1] = -q[K + 1] * p[K];
                    tmp = mK0 - i[K] * mK;
                    if (fabs (tmp) > fabs (mK0))
                        break;
                    mK0 = mK;
                    mK = tmp;
                    K = K + 1;
                };

/*
 *  Then
 *              x       =  ( m  q  - m   ) / p
 *                            K  K    K-1     K
 *
 *  as accurately as one could hope.
 */
                x = (mK * q[K] - mK0) / p[K];

/*
 *  To return  z  and  m0  as positive numbers,
 *   x  must take the sign of  m0  .
 */
                x = x * sgn (m0);
                m0 = fabs (m0);

/*
 *  Set  z = m0 * 2 ^ (binade+1-D) .
 */
                z = ldexp (m0, binade + 1 - D);

/*
 *  Print  z (hex),  z (dec),  m0 (dec),  binade+1-D,  x (hex), x (dec).
 */
                printf ("\
%08x %08x    Z=%22.16E    M=%17.17G    L+1-%d=%3d    %08x %08x    x=%23.16E\n",
                        hex (z), z, m0, D, binade + 1 - D, hex (x), x);

            }
        }
    }
}
