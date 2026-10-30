# Structure of the odd endpoint classes (S region) — verification record

Computed from the dataset endpoint tables (n = 5..50, 65, 66; all 48 values
of n for which endpoint tables exist). Verified 2026-10-30.

## Statement that holds in the full computed range

Let the odd classes be the classes (a, b) with a < b, a + b < m and
G(n; a, b) odd, grouped by the diagonal c = a + b. Then:

1. The set of diagonals that occur is a binary digit sumset
   C = c0 + subset-sums(Jc), where Jc is a set of distinct powers of two,
   none of them equal to 1 (the diagonal parity is fixed by Proposition 4,
   so the unit digit never occurs).
2. On every occurring diagonal, the set of first components is
   A(c) + subset-sums(Ja), with the SAME digit set Ja for all diagonals.
3. (Coupling) A(c) = A(c0) + (c - c0)/2 for every occurring diagonal c.
   In sheared coordinates (a, (c - c0)/2) the odd classes therefore form
   an exact Cartesian product of two digit sumsets.
4. Consequently the number of odd classes is
   2^(|Jc| + |Ja|), i.e. e(n) = |Jc| + |Ja| — this is the mechanism
   behind observation (O1) of the note, and it strengthens (O1):
   the odd-class set is not merely counted by a power of two, it IS
   a coupled binary subcube in digit coordinates.

## Origin law (fits every computed n)

The smallest diagonal carrying odd classes is c0 = ceil(m/2), and the
smallest first component on it is A(c0) = 0. So the position of the
whole configuration is also determined, not only its shape: diagonal
c0 is the zigzag diagonal (the class (0, ceil(m/2)) is proved odd,
value 1), and empirically no diagonal below it carries odd classes.

## The laws for Jc and Ja (fit every computed n, with one exception type)

Let 2^k be the largest power of two with 2^k <= n - 1, put
u = n - 2^k - 1 (0 <= u <= 2^k - 1) and t = floor(u/2). Then

    Jc(n) = { 2^(i+1) : bit i of t is 1 },
    Ja(n) = binary digit set of ceil(u/2),

with a single exception: if n is a power of two (n = 2^j, j >= 3),
Ja = {} instead of { 2^(j-2) }. (Setwise: Jc matches 48/48; Ja matches
45/48, the exceptions being exactly n = 8, 16, 32, all of this type.)
Jc is constant on each pair of consecutive n and enumerates the subsets
of {2, 4, ..., 2^(k-1)} in binary counting order across a block; its
largest possible digit is 2^(k-1) (e.g. 16 in the block 33..64, where
2^k = 32).

Consequently e(n) = s2(floor(u/2)) + s2(ceil(u/2)), minus 1 when n is a
power of two, where s2 is the binary digit sum.

## Caveat on evidence strength

For two-element sets the digit-sumset property is automatic, so the
small-n fits alone would be weak evidence. The evidential weight is in
the cross-block repetition (the same Ja recurring across blocks, the
Jc law fitting 48/48 as sets) and, decisively, in the pre-registered
predictions for n = 51..64 and 67..71 (predictions_n51_64_67_71.md),
which stand or fall with the first computation of those tables.

## Table of (Jc, Ja)

n: Jc / Ja (digit scales)
5: [] / []        6: [] / [1]       7: [2] / [1]      8: [2] / []
9: [] / []       10: [] / [1]      11: [2] / [1]     12: [2] / [2]
13: [4] / [2]    14: [4] / [1,2]   15: [2,4] / [1,2] 16: [2,4] / []
17: [] / []      18: [] / [1]      19: [2] / [1]     20: [2] / [2]
21: [4] / [2]    22: [4] / [1,2]   23: [2,4] / [1,2] 24: [2,4] / [4]
25: [8] / [4]    26: [8] / [1,4]   27: [2,8] / [1,4] 28: [2,8] / [2,4]
29: [4,8] / [2,4] 30: [4,8] / [1,2,4] 31: [2,4,8] / [1,2,4] 32: [2,4,8] / []
33: [] / []      34: [] / [1]      35: [2] / [1]     36: [2] / [2]
37: [4] / [2]    38: [4] / [1,2]   39: [2,4] / [1,2] 40: [2,4] / [4]
41: [8] / [4]    42: [8] / [1,4]   43: [2,8] / [1,4] 44: [2,8] / [2,4]
45: [4,8] / [2,4] 46: [4,8] / [1,2,4] 47: [2,4,8] / [1,2,4] 48: [2,4,8] / [8]
49: [16] / [8]   50: [16] / [1,8]
65: [] / []      66: [] / [1]

## Figures

- odd_classes_block17_32.png — (a, c) plots for the block n = 17..32.
- odd_classes_n31_n48.png — n = 31 (a full sheared rectangle) and
  n = 48 (two strands: Ja = [8]).

## Status

All statements above are [observed] over the computed range (endpoint
tables for n = 5..50, 65, 66); none is proved. The verification script
logic: extract odd classes in the S region from
graceful-endpoints-dataset/data/endpoints/, group by c = a + b, test
that the diagonal set and each diagonal's a-set are digit sumsets
(base + subset sums of distinct powers of two), that Ja is the same on
every diagonal, and the coupling A(c) - A(c0) = (c - c0)/2.
