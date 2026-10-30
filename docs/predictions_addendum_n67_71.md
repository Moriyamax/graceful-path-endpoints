# Pre-registered predictions, addendum: endpoint tables for n = 67..71

Registered: 2026-10-30 (JST).
Companion to `docs/predictions_n51_64_67_71.md`
(SHA-256 `0048fa0e02ece15936d36be1c3ac17baa251dc64a633bae27f378f26958cea50`).

**Status of the computations at registration.** The endpoint-table runs
for n = 67..71 were started on the author's machine before this file was
written and may be in progress or partly finished. No endpoint table for
any n in 67..71 has been inspected. The time order between this file's
commit and the completion of each run is not separately recorded; the
public commit time of this file is the only timestamp claimed.

## Notation

P_n is the path on n vertices; m = n - 1. G(n; a, b) is the number of
graceful permutations of P_n with first term a and last term b.
The Klein four-group V = {id, reversal R, complement C, RC} acts freely on
graceful permutations; on endpoint cells it acts by
(a, b) -> (b, a), (m - a, m - b), (m - b, m - a).
A *complementary cell* is a cell with a + b = m and a < b
(its V-orbit has size 2). All other non-diagonal cells lie in V-orbits of
size 4; diagonal cells are 0.

## New observation used here (data n = 5..66, all 62 values)

Let
- A(n) = number of size-4 orbits whose value G(n; a, b) is odd,
- B(n) = number of complementary cells with G(n; a, m - a) = 2 (mod 4).

Observed for every n in 5..66:
- (O1) every complementary cell has even value;
- (O2) A(n) = 2^{e(n)} (the odd-class law of the earlier registration);
- (O3) B(n) = 1 if n = 2^k + 1, B(n) = 2 if n = 2^k + 2, and B(n) = 0 otherwise.

Given (O1), the orbit decomposition gives the identity
G(n)/4 = A(n) + B(n) (mod 2); hence (O1)-(O3) together imply 8 | G(n).
None of (O1)-(O3) is proved. They are observations fitted on n = 5..66.

## Predictions for n = 67..71

| n | e(n) | A(n) | odd cells (with V-images) | B(n) | complementary cells odd |
|---|---|---|---|---|---|
| 67 | 2 | 4 | 16 | 0 | none |
| 68 | 2 | 4 | 16 | 0 | none |
| 69 | 2 | 4 | 16 | 0 | none |
| 70 | 3 | 8 | 32 | 0 | none |
| 71 | 4 | 16 | 64 | 0 | none |

Predicted odd-class representatives (the full odd set is the union of
their V-orbits; set equality is the test, not only the count):

- n = 67: (0,33), (1,32), (1,34), (2,33)
- n = 68: (0,34), (2,32), (1,35), (3,33)
- n = 69: (0,34), (2,32), (2,36), (4,34)
- n = 70: (0,35), (1,34), (2,33), (3,32), (2,37), (3,36), (4,35), (5,34)
- n = 71: (0,35), (1,34), (2,33), (3,32), (1,36), (2,35), (3,34), (4,33),
  (2,37), (3,36), (4,35), (5,34), (3,38), (4,37), (5,36), (6,35)

These representative sets are the same as those implied by the earlier
registration's laws (L0)-(L3); they are listed here explicitly.

Additional consistency predictions (already implied by the G(n) values
computed earlier, so they test the endpoint runs, not the laws):
- the sum of each table equals the previously computed G(n);
- 8 | G(n) for n = 67..71 (already known from the G(n) values).

## Falsification

Any of the following falsifies the corresponding statement as given:
- an odd cell outside the predicted set, or a predicted cell that is even
  (odd-class law);
- an odd complementary cell (O1);
- a complementary cell = 2 (mod 4) for any n in 67..71 (O3).

Results will be reported for every n, including any deviation.
