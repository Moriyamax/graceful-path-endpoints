# Endpoint counts of graceful permutations of paths

Dataset accompanying the research note *"A divisibility-by-eight conjecture
for graceful permutations of paths"* and the technical note *"Endpoint
structure of graceful permutations of paths"* (M. Moriyama). It collects
exact counts of graceful permutations of the path graph P_n, resolved by
their endpoint pair.

## Definitions

A *graceful permutation* of P_n is a permutation (x_1, ..., x_n) of
{0, ..., n-1} whose consecutive absolute differences |x_{i+1} - x_i| are all
distinct (hence a permutation of {1, ..., n-1}). G(n) is the number of
graceful permutations of P_n (OEIS A006967). G(n; a, b) is the number of
graceful permutations with first term a and last term b, and m = n - 1.

## Contents

```
data/graceful_counts_n5_71.tsv   G(n) for n = 5..71 with per-value
                                 verification status and run statistics
data/graceful_endpoint_counts.csv
                                 all endpoint tables in one file, with
                                 header: n,a,b,count (121,806 rows,
                                 n = 5..71)
data/endpoints/endpoints_n*.csv  the same tables, one file per n
                                 (no header), n^2 rows per file
source/                          implementations (see Provenance)
checks/                          run outputs of the counting programs and
                                 verify_package.py, an independent checker
                                 for the packaged endpoint files
docs/                            predictions recorded before computation
                                 (n = 51..64, 67..71) with their hashes
MANIFEST.sha256                  SHA-256 hashes of the packaged files
```

Endpoint tables are included for every n = 5..71. In the Zenodo record,
the per-n tables are also bundled as `endpoints_per_n.zip` and the run
outputs and checker as `reproducibility_checks.zip`.

## Verification status

No third-party tabulation of the endpoint counts was found (OEIS A006967 and
references checked, October 2026). Verification is therefore layered, and
the layer covering each range of G(n) is recorded in the `verification`
column of `graceful_counts_n5_71.tsv`:

- **G(n), n <= 59 (category A):** agrees with the published b-file of
  OEIS A006967 (terms 0..40 from Adamaszek, term 41 from Knuth, terms
  42..59 as listed there). This includes an independent recomputation of
  a(41).
- **G(n), n = 60..68 (category C):** two builds of the counting program
  agree.
- **G(n), n = 69..71 (category C):** a rerun with a different thread count
  (10 instead of 8) reproduces G, the maximum state count, the expansion
  count and the maximum endpoint count exactly.
- **Endpoint runs, n = 67..71:** the totals of the endpoint tables (10
  threads) reproduce G(n) and the maximum state count of the G-only runs.
- **Endpoint counts, n <= 18:** produced by an independent complete
  enumeration (`source/graceful_enum.c`, `graceful_enum_shard.c`). The
  restored endpoint counts of the main program agree with this
  enumeration on every cell for n = 5..18.
- **Endpoint counts in alpha-labeling cells, N <= 55:** the sums over the
  cells that carry alpha-labelings (b - a = ceil(m/2) for even N;
  a + b = m/2 and a + b = 3m/2 for odd N, one orientation) agree with the
  independent counts of OEIS A395104 for all N = 5..55. This is a
  third-party check of those cells for 19 <= n <= 55.
- **Source rebuild:** the packaged `source/adam2_v3.cpp`, recompiled in a
  clean environment, reproduces the packaged endpoint tables for n = 18
  and n = 24 exactly (see `checks/rebuild_check.log`).
- **Endpoint counts, n >= 19 (other cells):** single implementation;
  internal checks only (row sums equal G(n); no violations of the
  endpoint-parity proposition or of Lemma 2 of the note; the zigzag value
  G(n; 0, ceil(m/2)) equals 1). The reversal and complement symmetries of
  the restored table are imposed by the restoration construction and are
  *not* independent checks.

`checks/verify_package.py` re-runs the total, parity, zigzag and
decomposition-parity checks on the packaged files; its output is in
`checks/checks_dataset.log` (rerun on all n = 5..71: ALL OK).

## How the counts were produced

The main program (`adam2`, Adamaszek-style layered dynamic programming)
counts graceful permutations level by level. Endpoint counts are restored
from the final layer, whose states carry an unordered endpoint pair {a, b}
and a complement-merged count M with M = 2 * G(n; a, b) when a + b != m and
M = G(n; a, b) when a + b = m. Source snapshot: `source/adam2_v3.cpp`
(SHA-256 1a1490c5e6d12e82e2d1d99dc02077dd61530c7c96f02be4b2e9891785e6e5fb);
the same code is developed in the linked GitHub repository.

Provenance by range: endpoint counts for n = 5..41 were computed on Linux
with 1 thread; n = 42..50 on Windows with 8 threads; n = 51..64 on Windows
with 8 threads; n = 65, 66 with 8 threads; n = 67..71 with
10 threads (Ryzen 5 5500GT, 128 GB; n = 71 took 3641 s, peak 815,079,482
states per level). See the run tables in `checks/`.

The programs were written with AI assistance (see the note).

## Limitations

- The endpoint counts for n >= 19 rest on a single implementation plus the
  internal checks listed above, except for the alpha-labeling cells with
  N <= 55; an independent reimplementation would strengthen them.
- Predictions recorded before computation were confirmed by the computed
  files: numbers of odd endpoint classes for n = 42..47; numbers,
  positions and parities of the exceptional classes for n = 65, 66; the
  full sets of odd classes and the complementary classes for n = 51..64
  and 67..71. The hashes of the prediction files were recorded in advance
  but not publicly timestamped. These are consistency evidence, not
  verification by a third party.

## Related records

- Research note (Zenodo): DOI to be inserted.
- Technical note (Zenodo): DOI to be inserted.
- Source code (GitHub): https://github.com/Moriyamax/graceful-path-endpoints
  (releases are archived to Zenodo; software DOI to be inserted).
- OEIS A006967: https://oeis.org/A006967 ; OEIS A395104:
  https://oeis.org/A395104

## License and author

Data and this README: CC BY 4.0. Author: M. Moriyama.
