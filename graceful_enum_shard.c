/* graceful_enum_shard.c
 * Sharded variant of graceful_enum.c: fixes the first term of the
 * permutation to a value A given as the second argument, so that a
 * long enumeration can be split into independent shards whose
 * outputs survive interruptions.  Usage: ./graceful_enum_shard N A
 * Output: CSV rows n,a,b,count for the fixed a only, on stdout.
 * Self-checks are NOT performed per shard (they need the full
 * table); they are run on the merged table after all shards finish.
 */
/* graceful_enum.c
 * Complete backtracking enumeration of graceful permutations of the path P_n
 * (equivalently, graceful labelings of P_n with labelled positions), with
 * endpoint-class counts G(n; a, b) = #{permutations with first term a, last term b}.
 *
 * Usage: ./graceful_enum N
 * Output (stdout): CSV rows  n,a,b,count  for all 0<=a,b<n,
 * plus summary lines on stderr: G(n), S, T, T2 and self-checks
 * (sum of counts = G(n); decomposition G(n) = 4*(S+T2) for n>=3;
 *  symmetry counts: cnt[a][b] = cnt[b][a] = cnt[m-b][m-a]).
 *
 * Build: gcc -O3 -o graceful_enum graceful_enum.c
 * Method: depth-first placement; vertex-use and difference-use bitmasks.
 * Complexity grows by a factor of about 6.5 per step of n on one core
 * (measured: n=13 ~2 s, n=14 ~11 s, n=15 ~68 s, n=16 ~460 s).
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

static int n, m;
static int fixed_a = -1;
static uint32_t usedv, usedd;
static int seq[64];
static long long cnt[64][64];

static void bt(int pos) {
    if (pos == n) { cnt[seq[0]][seq[n-1]]++; return; }
    int last = pos ? seq[pos-1] : -1;
    for (int v = 0; v < n; v++) {
        if (pos == 0 && fixed_a >= 0 && v != fixed_a) continue;
        if (usedv >> v & 1u) continue;
        if (pos) {
            int d = v > last ? v - last : last - v;
            if (usedd >> d & 1u) continue;
            usedd |= 1u << d; usedv |= 1u << v; seq[pos] = v;
            bt(pos + 1);
            usedd &= ~(1u << d); usedv &= ~(1u << v);
        } else {
            usedv |= 1u << v; seq[pos] = v; bt(pos + 1); usedv &= ~(1u << v);
        }
    }
}

int main(int argc, char **argv) {
    if (argc < 2) { fprintf(stderr, "usage: %s N [A]\n", argv[0]); return 1; }
    n = atoi(argv[1]); m = n - 1;
    if (argc >= 3) fixed_a = atoi(argv[2]);
    bt(0);
    long long G = 0, S = 0, T = 0;
    for (int a = 0; a < n; a++)
        for (int b = 0; b < n; b++) {
            G += cnt[a][b];
            printf("%d,%d,%d,%lld\n", n, a, b, cnt[a][b]);
            if (a < b) {
                if (a + b < m) S += cnt[a][b];
                else if (a + b == m) T += cnt[a][b];
            }
        }
    long long T2 = T / 2;
    fprintf(stderr, "n=%d  G(n)=%lld  S=%lld  T=%lld  T2=%lld\n", n, G, S, T, T2);
    if (n >= 3)
        fprintf(stderr, "check decomposition G = 4*(S+T2): %s\n",
                G == 4 * (S + T2) ? "OK" : "FAIL");
    int sym = 1;
    for (int a = 0; a < n; a++)
        for (int b = 0; b < n; b++)
            if (cnt[a][b] != cnt[b][a] || cnt[a][b] != cnt[m-b][m-a]) sym = 0;
    fprintf(stderr, "check symmetries cnt[a][b]=cnt[b][a]=cnt[m-b][m-a]: %s\n",
            sym ? "OK" : "FAIL");
    return 0;
}
