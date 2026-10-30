# Pre-registered predictions: odd endpoint classes for n = 51..64 and 67..71

Registered: 2026-10-30 (before any endpoint table for these n has been
computed; endpoint tables currently exist only for n = 5..50, 65, 66).
This file's SHA-256 is recorded outside the file (assistant's memory and
goal log) at registration time.

## The laws used (fitted on n = 5..50, 65, 66 — all 48 values)

Notation: m = n - 1; 2^k is the largest power of two with 2^k <= n - 1;
u = n - 2^k - 1 (so 0 <= u <= 2^k - 1); s2(x) = number of 1 bits of x.

(L0) Origin. The smallest diagonal carrying odd classes is c0 = ceil(m/2),
     and the smallest first component on it is A(c0) = 0.
     (Holds for all 48 known n. The class (0, ceil(m/2)) is the zigzag
     class, proved odd with value 1; the content is that no diagonal
     below ceil(m/2) carries odd classes.)
(L1) Diagonals. The diagonal set is c0 + subset-sums(Jc) with
     Jc = { 2^(i+1) : bit i of floor(u/2) is 1 }.
     (Setwise match, 48/48.)
(L2) First components. On diagonal c, the set of first components is
     (c - c0)/2 + subset-sums(Ja), with
     Ja = binary digit set of ceil(u/2),
     EXCEPT when n is a power of two (n = 2^j, j >= 3), where Ja = {}
     instead of { 2^(j-2) }.
     (Setwise match on 45/48; the three exceptions 8, 16, 32 are all of
     this single type.)
(L3) Count. e(n) = s2(floor(u/2)) + s2(ceil(u/2)), minus 1 when n is a
     power of two. The number of odd classes is 2^e(n).

## Predictions

n   Jc            Ja            e   #odd classes
51  {2,16}        {1,8}         4   16
52  {2,16}        {2,8}         4   16
53  {4,16}        {2,8}         4   16
54  {4,16}        {1,2,8}       5   32
55  {2,4,16}      {1,2,8}       6   64
56  {2,4,16}      {4,8}         5   32
57  {8,16}        {4,8}         4   16
58  {8,16}        {1,4,8}       5   32
59  {2,8,16}      {1,4,8}       6   64
60  {2,8,16}      {2,4,8}       6   64
61  {4,8,16}      {2,4,8}       6   64
62  {4,8,16}      {1,2,4,8}     7   128
63  {2,4,8,16}    {1,2,4,8}     8   256
64  {2,4,8,16}    {16} or {}    5 or 4   32 or 16   <-- fork, see below
67  {2}           {1}           2   4
68  {2}           {2}           2   4
69  {4}           {2}           2   4
70  {4}           {1,2}         3   8
71  {2,4}         {1,2}         4   16

By (L0)-(L2) these determine the complete predicted odd-class set for
each n (diagonals from c0 = ceil(m/2), first components shifted by
(c - c0)/2), not merely the count.

## The fork at n = 64

n = 64 is a power of two. If the exception in (L2) is a general rule
for n = 2^j, then Ja = {} and e = 4 (16 odd classes). If the exceptions
at 8, 16, 32 were small-n accidents, the formula gives Ja = {16} and
e = 5 (32 odd classes). This is the single sharpest test in the table.

## Falsification

Any deviation in any row — a diagonal outside the predicted set, a
missing or extra first component on any diagonal, a wrong count —
falsifies the corresponding law as stated. Partial credit rules are
not fixed in advance; deviations will be reported as found.

Status: laws are fitted, not proved. The registration exists precisely
to convert the fit into a prediction before the n = 51..64 and 67..71
endpoint tables are computed.
