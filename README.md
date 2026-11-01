# adam2 — counting graceful permutations of paths

Programs for counting graceful permutations of the path graph P_n
(OEIS A006967), written for the research note *"Conjecture: 8 | G(n)
for every n ≥ 5, checked for all n ≤ 71"* (M. Moriyama) and the
technical note *"Endpoint structure of graceful permutations of
paths"* (M. Moriyama).

A graceful permutation is a permutation (x_1, ..., x_n) of {0, ..., n-1}
whose consecutive absolute differences are all distinct. G(n) is their
number, and G(n; a, b) the number with first term a and last term b.

## Programs

- `adam2_v3.cpp` — the main counter. Adamaszek-style layered dynamic
  programming, multithreaded, exact arithmetic in `unsigned __int128`.
  With `--endpoints DIR` it also restores the endpoint-resolved counts
  G(n; a, b) from the final layer: final states carry an unordered
  endpoint pair {a, b} and a complement-merged count M with
  M = 2 * G(n; a, b) if a + b != m and M = G(n; a, b) if a + b = m
  (m = n - 1).
- `graceful_enum.c`, `graceful_enum_shard.c` — a plain complete
  enumeration (backtracking), used as an independent cross-check for
  small n. The shard version splits the count by first term.

The programs were written with AI assistance.

## Build

```
g++ -O2 -pthread -o adam2 adam2_v3.cpp
```

Verified with g++ 13.3 on Linux; the production runs were made on the
author's Windows and Linux machines.

## Usage

```
./adam2 N_FROM [N_TO] [--threads T] [--results FILE]
        [--ckpt DIR [--ckpt-every K]] [--endpoints DIR]
        [--verbose] [--stop-after-level L]
```

Examples:

```
./adam2 18 18 --threads 4 --endpoints ep --results res.tsv
./adam2 65 66 --threads 8 --endpoints ep --results ep65.tsv
```

Outputs:

- results TSV, one row per n: `n, G, seconds, max_states_per_level,
  expansions, peak_RSS_KB, threads, max_endpoints`.
- `endpoints_n<n>.csv` per n: rows `n,a,b,count` (no header), the full
  n x n table of G(n; a, b).

## Scale of the production runs

G(n) was computed for n = 5..71. On the author's machine the single runs
for n = 69..71 took about 2,600–4,400 s at 8 threads (maximum states per
level 815,079,482 at n = 71); restoring endpoint tables for n = 65 took
about 900 s at 8 threads. Memory use grows with the state count; the
author's pre-run estimate for the n = 65/66 endpoint runs was
25–30 GiB.

## Verification status (summary)

- G(n) agrees with the published b-file of OEIS A006967 for n <= 59.
- Two builds agree for n = 60..68; a rerun with a different thread count
  reproduces n = 69..71 exactly (counts, state maxima, expansions).
- The endpoint tables of `adam2` agree with the independent complete
  enumeration (`graceful_enum.c`) on every cell for n <= 18.
- The sums over the endpoint cells that carry alpha-labelings agree
  with the independent counts of OEIS A395104 for all N = 5..55.
- This source, recompiled from a clean checkout, reproduces the
  published endpoint tables for n = 18 and n = 24 exactly.
- No third-party tabulation of the endpoint counts was found; for
  n >= 19 they rest on this implementation plus internal consistency
  checks (row sums, endpoint-parity proposition, zigzag value).

Full data and the detailed verification record are in the accompanying
dataset (see below, and `data/README.md`).

## Data and notes

- Dataset (Zenodo): doi:10.5281/zenodo.23801688 (all versions:
  doi:10.5281/zenodo.23801687) — *"Endpoint-resolved counts of graceful
  permutations of paths, n = 5 to 71"*. The `data/` directory of this
  repository mirrors that record: the values of G(n) for n = 5..71,
  the combined endpoint table (121,806 rows), the per-n endpoint
  tables (zipped), the reproducibility checks (zipped), a data README
  and a SHA-256 manifest. The Zenodo record is the version of record.
- Research note (Zenodo): *"Conjecture: 8 | G(n) for every n ≥ 5,
  checked for all n ≤ 71"*, doi:10.5281/zenodo.23634015 (all versions);
  current version v4: https://zenodo.org/records/23803911
- Technical note (Zenodo): *"Endpoint structure of graceful
  permutations of paths"*, doi:10.5281/zenodo.23802065 (all versions);
  current version v2: https://zenodo.org/records/23820863
- Parity structure of the odd endpoint classes and pre-registered
  predictions for n = 51..64 and 67..71: see
  `docs/odd_class_structure.md` and
  `docs/predictions_n51_64_67_71.md`, figures in `docs/figures/`.
- OEIS A006967: https://oeis.org/A006967 ;
  OEIS A395104: https://oeis.org/A395104

## License

Code: MIT (see `LICENSE`). Data in `data/`: CC BY 4.0.
Author: M. Moriyama. Citation metadata: see `CITATION.cff`.
