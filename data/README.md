# Endpoint counts of graceful permutations of paths

Dataset accompanying the research note *"A divisibility-by-eight conjecture
for graceful permutations of paths"* (M. Moriyama). It collects exact counts
of graceful permutations of the path graph P_n, resolved by their endpoint
pair.

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
                                 header: n,a,b,count (51,476 rows)
data/endpoints/endpoints_n*.csv  the same tables, one file per n
                                 (no header), n^2 rows per file
source/                          implementations (see Provenance)
checks/                          run outputs of the counting programs and
                                 verify_package.py, an independent checker
                                 for the packaged endpoint files
MANIFEST.sha256                  SHA-256 hashes of the packaged files
```

Endpoint tables are included for n = 5..50, 65 and 66 (endpoint counts
were not computed for n = 51..64 or 67..71). In the Zenodo record, the
per-n tables are also bundled as `endpoints_per_n.zip` and the run
outputs and checker as `reproducibility_checks.zip`.

## Verification status

No third-party tabulation of the endpoint counts was found (OEIS A006967 and
references checked, October 2026); the endpoint-resolved values are new.
Verification is therefore layered, and the layer covering each range is
recorded in the `verification` column of `graceful_counts_n5_71.tsv`:

- **G(n), n <= 59 (category A):** agrees with the published b-file of
  OEIS A006967 (terms 0..40 from Adamaszek, term 41 from Knuth, terms
  42..59 as listed there).
- **G(n), n = 60..68 (category C):** two builds of the counting program
  agree.
- **G(n), n = 69..71 (category C):** a rerun with a different thread count
  (10 instead of 8) reproduces G, the maximum state count, the expansion
  count and the maximum endpoint count exactly.
- **Endpoint counts, n <= 18:** produced by an independent complete
  enumeration (`source/graceful_enum.c`, `graceful_enum_shard.c`). The
  restored endpoint counts of the main program agree with this
  enumeration on every cell for n = 5..18.
- **Source rebuild:** the packaged `source/adam2_v3.cpp`, recompiled in a
  clean environment, reproduces the packaged endpoint tables for n = 18
  and n = 24 exactly (see `checks/rebuild_check.log`).
- **Endpoint counts, n >= 19:** single implementation; internal checks only
  (row sums equal G(n); no violations of the endpoint-parity proposition or
  of Lemma 2 of the note; the zigzag value G(n; 0, ceil(m/2)) equals 1).
  The reversal and complement symmetries of the restored table are imposed
  by the restoration construction and are *not* independent checks.

`checks/verify_package.py` re-runs the total, parity, zigzag and
decomposition-parity checks on the packaged files; its output is in
`checks/checks_dataset.log`.

## How the counts were produced

The main program (`adam2`, Adamaszek-style layered dynamic programming)
counts graceful permutations level by level. Endpoint counts are restored
from the final layer, whose states carry an unordered endpoint pair {a, b}
and a complement-merged count M with M = 2 * G(n; a, b) when a + b != m and
M = G(n; a, b) when a + b = m. Source snapshot: `source/adam2_v3.cpp`
(SHA-256 1a1490c5e6d12e82e2d1d99dc02077dd61530c7c96f02be4b2e9891785e6e5fb);
the same code is developed in the linked GitHub repository.

Provenance by range: endpoint counts for n = 5..41 were computed on Linux
with 1 thread; n = 42..50 on Windows with 8 threads; n = 65, 66 with
8 threads (see the run tables in `checks/`). G(n) values were computed on
the author's machines with the builds and thread counts recorded in
`graceful_counts_n5_71.tsv`.

The programs were written with AI assistance (see the note).

## Limitations

- The endpoint counts for n >= 19 rest on a single implementation plus the
  internal checks listed above; an independent reimplementation would
  strengthen them.
- Predictions stated in advance for n = 42..47 (numbers of odd endpoint
  classes) and for n = 65 and n = 66 (numbers, positions and parities of
  the exceptional classes) were all confirmed by the computed files; these
  are consistency evidence, not verification by a third party.

## Related records

- Research note (Zenodo): DOI to be inserted (reserved in draft).
- Source code (GitHub): https://github.com/Moriyamax/graceful-path-endpoints
  (releases are archived to Zenodo; software DOI to be inserted).
- OEIS A006967: https://oeis.org/A006967

## License and author

Data and this README: CC BY 4.0. Author: M. Moriyama.
