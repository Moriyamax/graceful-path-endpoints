# Endpoint counts of graceful permutations of paths

Dataset accompanying the research note *"Conjecture: 8 | G(n) for
every n ≥ 5, checked for all n ≤ 71"* and the technical note *"Endpoint
structure of graceful permutations of paths"* (M. Moriyama). It collects
exact counts of graceful permutations of the path graph P_n, resolved by
their endpoint pair.

This directory mirrors the dataset archived on Zenodo as
doi:10.5281/zenodo.23801688 (*"Endpoint-resolved counts of graceful
permutations of paths, n = 5 to 71"*). The Zenodo record is the version
of record; this copy is kept identical to it.

## Definitions

A *graceful permutation* of P_n is a permutation (x_1, ..., x_n) of
{0, ..., n-1} whose consecutive absolute differences |x_{i+1} - x_i| are all
distinct (hence a permutation of {1, ..., n-1}). G(n) is the number of
graceful permutations of P_n (OEIS A006967). G(n; a, b) is the number of
graceful permutations with first term a and last term b, and m = n - 1.

## Contents (files of this directory)

```
README.md                        this file
graceful_counts_n5_71.tsv        G(n) for n = 5..71 with per-value
                                 verification status and run statistics
graceful_endpoint_counts.csv     all endpoint tables in one file, with
                                 header: n,a,b,count (121,806 rows,
                                 n = 5..71)
endpoints_per_n.zip              the same tables, one file per n
                                 (endpoints_n<n>.csv, no header,
                                 n^2 rows per file; 67 files, n = 5..71)
reproducibility_checks.zip       a checks/ folder: run outputs of the
                                 counting programs, verify_package.py
                                 and its log for n = 5..71
MANIFEST.sha256                  SHA-256 hashes of the files above
```

The pre-registered predictions for n = 51..64 and 67..71 are files of
the Zenodo record (`predictions_n51_64_67_71.md`,
`predictions_addendum_n67_71.md`; SHA-256 0048fa0e... and d673ff47...);
in this repository they are kept in `docs/`.

Endpoint tables are included for every n = 5..71. To run
`verify_package.py`, work from the repository root: unpack
`endpoints_per_n.zip` into `data/endpoints/`, unpack
`reproducibility_checks.zip` at the root (it creates `checks/`), and
run `python3 checks/verify_package.py`.

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
  enumeration (`graceful_enum.c`, `graceful_enum_shard.c` at the
  repository root). The restored endpoint counts of the main program
  agree with this enumeration on every cell for n = 5..18.
- **Endpoint counts in alpha-labeling cells, N <= 55:** the sums over the
  cells that carry alpha-labelings (b - a = ceil(m/2) for even N;
  a + b = m/2 and a + b = 3m/2 for odd N, one orientation) agree with the
  independent counts of OEIS A395104 for all N = 5..55. This is a
  third-party check of those cells for 19 <= n <= 55.
- **Source rebuild:** `adam2_v3.cpp`, recompiled in a clean environment,
  reproduces the packaged endpoint tables for n = 18 and n = 24 exactly
  (see `checks/rebuild_check.log` inside `reproducibility_checks.zip`).
- **Endpoint counts, n >= 19 (other cells):** single implementation;
  internal checks only (row sums equal G(n); no violations of the
  endpoint-parity proposition or of Lemma 2 of the note; the zigzag value
  G(n; 0, ceil(m/2)) equals 1). The reversal and complement symmetries of
  the restored table are imposed by the restoration construction and are
  *not* independent checks.

`verify_package.py` (inside `reproducibility_checks.zip`) re-runs the
total, parity, zigzag and decomposition-parity checks on the packaged
files; its output is in `checks/checks_dataset.log` (rerun on all
n = 5..71: ALL OK).

## How the counts were produced

The main program (`adam2`, Adamaszek-style layered dynamic programming)
counts graceful permutations level by level. Endpoint counts are restored
from the final layer, whose states carry an unordered endpoint pair {a, b}
and a complement-merged count M with M = 2 * G(n; a, b) when a + b != m and
M = G(n; a, b) when a + b = m. Source snapshot: `adam2_v3.cpp` at the
repository root (SHA-256
1a1490c5e6d12e82e2d1d99dc02077dd61530c7c96f02be4b2e9891785e6e5fb);
the same code is archived with the Zenodo dataset record.

Provenance by range: endpoint counts for n = 5..41 were computed on Linux
with 1 thread; n = 42..50 on Windows with 8 threads; n = 51..64 on Windows
with 8 threads; n = 65, 66 with 8 threads; n = 67..71 with
10 threads (Ryzen 5 5500GT, 128 GB; n = 71 took 3641 s, peak 815,079,482
states per level). See the run tables in `reproducibility_checks.zip`.

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

- This dataset (Zenodo): doi:10.5281/zenodo.23801688 (all versions:
  doi:10.5281/zenodo.23801687)
- Research note (Zenodo): doi:10.5281/zenodo.23634015 (all versions)
- Technical note (Zenodo): doi:10.5281/zenodo.23802065 (all versions)
- Source code (GitHub): https://github.com/Moriyamax/graceful-path-endpoints
  (development repository; the code archived with the dataset record is
  the version of record).
- OEIS A006967: https://oeis.org/A006967 ; OEIS A395104:
  https://oeis.org/A395104

## License and author

Data and this README: CC BY 4.0. The programs at the repository root are
MIT-licensed. Author: M. Moriyama.
