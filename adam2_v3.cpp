// adam2.cpp : graceful permutation の総数 G(n) を、差の大きい順に辺を足す幅優先探索 + 同値類の併合で数える。
//   方式: M. Adamaszek, "Efficient enumeration of graceful permutations", arXiv:math/0608513 の
//         (free, forb) 状態 + 補数対称による併合を、自前で実装したもの。
//   省メモリ化: 状態を固定長ビット列(24 or 32 B)に圧縮 / 128bit計数(桁あふれ検出つき) /
//               処理済みの殻(shard)から順にメモリ解放 / n単位の結果追記 / 層単位のチェックポイント。
// ビルド: g++ -O3 -std=c++17 -pthread -o adam2 adam2.cpp
// 使い方: ./adam2 N_FROM [N_TO] [--threads T] [--results FILE] [--ckpt DIR [--ckpt-every K]] [--endpoints DIR] [--verbose] [--stop-after-level L]
//   既に results に n の行があれば、その n は飛ばす(再開用)。
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>
#include <memory>
#include <fstream>
#include <sstream>
#include <map>
typedef unsigned __int128 u128;
typedef uint64_t u64;
static int n, m, NT = 1, STOP_LEVEL = -1, CKPT_EVERY = 1;
static bool VERBOSE = false;
static std::string CKPT_DIR, ENDP_DIR;
#include <tuple>
static std::vector<std::tuple<int,int,unsigned __int128>> g_eps;   // 最終層の状態: (端点a, 端点b, 併合後の計数)
static std::atomic<bool> g_overflow(false), g_ecap(false);
static std::atomic<int> g_emax_seen(0);
static thread_local int tl_emax = 0;

static const struct { int n; const char* g; } KNOWN[] = {
    {5,"8"},
    {6,"24"},
    {7,"32"},
    {8,"40"},
    {9,"120"},
    {10,"296"},
    {11,"648"},
    {12,"1328"},
    {13,"3200"},
    {14,"9912"},
    {15,"25592"},
    {16,"55920"},
    {17,"143192"},
    {18,"510696"},
    {19,"1451296"},
    {20,"3497344"},
    {21,"10451824"},
    {22,"38570704"},
    {23,"118914992"},
    {24,"315235872"},
    {25,"1014824752"},
    {26,"3963684496"},
    {27,"13166130152"},
    {28,"37846301904"},
    {29,"130507967088"},
    {30,"533318630936"},
    {31,"1884550215976"},
    {32,"5800121391936"},
    {33,"21158113465976"},
    {34,"90567541913496"},
    {35,"338067146187024"},
    {36,"1108133518300512"},
    {37,"4252675867341584"},
    {38,"18945451090229632"},
    {39,"74250341774955936"},
    {40,"257148148674392752"},
    {41,"1032009647743958000"},
    {42,"4776537381650913456"},
    {43,"19563451130134939776"},
    {44,"71216650440776894752"},
    {45,"298154148754915127632"},
    {46,"1427854923572338657696"},
    {47,"6087438933717522530432"},
    {48,"23185534258894833484288"},
    {49,"100821801398466961491968"},
    {50,"498890222330199041922112"},
    {51,"2206875106796834596069888"},
    {52,"8760865325250366633996352"},
    {53,"39490602329798464414485840"},
    {54,"201294973031943364359120864"},
    {55,"921550660971604228859129664"},
    {56,"3801515895556228764420703872"},
    {57,"17706902160662958819309560128"},
    {58,"92854432014254348418723459568"},
    {59,"438964301896906753434739837880"}
};
static std::string pr(u128 x){ if(x==0) return "0"; std::string s; while(x){ s.insert(s.begin(),'0'+(int)(x%10)); x/=10;} return s; }
static inline u64 mix(u64 x){ x^=x>>30; x*=0xbf58476d1ce4e5b9ULL; x^=x>>27; x*=0x94d049bb133111ebULL; x^=x>>31; return x; }
static long vmhwm_kb(){ FILE*p=fopen("/proc/self/status","r"); char l[256]; long v=-1; while(p&&fgets(l,256,p)) if(sscanf(l,"VmHWM: %ld kB",&v)==1)break; if(p)fclose(p); return v; }

template<int W> struct Engine {
    static constexpr int KB = 64*W;
    struct Key { u64 w[W]; };
    struct Entry { u64 k[W]; u64 lo, hi; };
    struct Shard { Entry* tab=nullptr; size_t cap=0, cnt=0; std::mutex mu; };
    struct Table { std::unique_ptr<Shard[]> sh; Table():sh(new Shard[256]){}
        void clear(){ for(int i=0;i<256;i++){ free(sh[i].tab); sh[i].tab=nullptr; sh[i].cap=sh[i].cnt=0; } }
        ~Table(){ clear(); }
        u64 states() const { u64 s=0; for(int i=0;i<256;i++) s+=sh[i].cnt; return s; } };
    int emax;
    static inline void put(Key&k,int off,int nb,u64 v){ int wi=off>>6, sh=off&63; k.w[wi]|=v<<sh; if(sh+nb>64) k.w[wi+1]|=v>>(64-sh); }
    static inline u64 get(const Key&k,int off,int nb){ int wi=off>>6, sh=off&63; u64 v=k.w[wi]>>sh; if(sh+nb>64) v|=k.w[wi+1]<<(64-sh); return v&((1ULL<<nb)-1); }
    static constexpr int RB = 5;   // 対の相手の順位を RB ビットで格納 (端点数<=32)
    bool encode(const int*fr,const int*fb,Key&k) const {
        for(int i=0;i<W;i++) k.w[i]=0;
        int e=0; int rk[128];
        for(int u=0;u<n;u++){ int s=fr[u]==0?0:(fr[u]==2?1:2); put(k,2*u,2,(u64)s); if(s==2) rk[u]=e++; }
        if(e>tl_emax) tl_emax=e;
        if(e>emax) return false;
        int i=0, idx=0;   // 各対について「順位の小さい側」からのみ相手の順位を1回格納する
        for(int u=0;u<n;u++) if(fr[u]==1){ int pr=rk[fb[u]]; if(pr>i){ put(k,2*n+RB*idx,RB,(u64)pr); idx++; } i++; }
        return true;
    }
    void decode(const Key&k,int*fr,int*fb) const {
        int E[32], pr[32], e=0;
        for(int u=0;u<n;u++){ int s=(int)get(k,2*u,2); fr[u]=s==0?0:(s==1?2:1); fb[u]=u; if(s==2){ pr[e]=-1; E[e++]=u; } }
        int idx=0;
        for(int i=0;i<e;i++) if(pr[i]<0){ int r=(int)get(k,2*n+RB*idx,RB); idx++; pr[i]=r; pr[r]=i; }
        for(int i=0;i<e;i++) fb[E[i]]=E[pr[i]];
    }
    static bool less(const Key&a,const Key&b){ for(int i=W-1;i>=0;i--){ if(a.w[i]!=b.w[i]) return a.w[i]<b.w[i]; } return false; }
    bool canon(const int*fr,const int*fb,int*fr2,int*fb2,Key&out) const {
        Key a,b; if(!encode(fr,fb,a)) return false;
        for(int u=0;u<n;u++){ int w=n-1-u; fr2[u]=fr[w]; fb2[u]=n-1-fb[w]; }
        if(!encode(fr2,fb2,b)) return false;
        out = less(b,a)?b:a; return true;
    }
    static inline u64 hashKey(const Key&k){ u64 h=0x9E3779B97F4A7C15ULL; for(int i=0;i<W;i++) h=mix(h^k.w[i])+0x632BE59BD9B4E019ULL*(i+1); return h; }
    static inline bool empty(const Entry&e){ for(int i=0;i<W;i++) if(e.k[i]) return false; return true; }
    static inline bool eq(const Entry&e,const Key&k){ for(int i=0;i<W;i++) if(e.k[i]!=k.w[i]) return false; return true; }
    static inline void addcnt(Entry&e,u64 lo,u64 hi){
        u64 nlo=e.lo+lo; u64 c=nlo<e.lo; u64 nhi=e.hi+hi; bool o1=nhi<e.hi; u64 nhi2=nhi+c; bool o2=nhi2<nhi;
        if(o1||o2) g_overflow=true;
        e.lo=nlo; e.hi=nhi2;
    }
    static inline size_t slot(u64 h,size_t cap){ return (size_t)(((u128)mix(h^0xD6E8FEB86659FD93ULL)*cap)>>64); }
    static void grow(Shard&s){
        size_t nc = s.cap? s.cap + s.cap/2 + 1 : 64;
        Entry* nt=(Entry*)calloc(nc,sizeof(Entry)); if(!nt){ fprintf(stderr,"メモリ確保失敗\n"); exit(2); }
        for(size_t j=0;j<s.cap;j++){ Entry&e=s.tab[j]; if(empty(e)) continue;
            Key k; for(int i=0;i<W;i++) k.w[i]=e.k[i];
            size_t p=slot(hashKey(k),nc); while(!empty(nt[p])){ if(++p==nc) p=0; }
            nt[p]=e; }
        free(s.tab); s.tab=nt; s.cap=nc;
    }
    static void add(Table&t,const Key&k,u64 lo,u64 hi){
        u64 h=hashKey(k); Shard&s=t.sh[h>>56]; std::lock_guard<std::mutex> g(s.mu);
        if(s.cap==0 || s.cnt*10>=s.cap*8) grow(s);
        size_t p=slot(h,s.cap);
        for(;;){ Entry&e=s.tab[p];
            if(empty(e)){ for(int i=0;i<W;i++) e.k[i]=k.w[i]; e.lo=lo; e.hi=hi; s.cnt++; return; }
            if(eq(e,k)){ addcnt(e,lo,hi); return; }
            if(++p==s.cap) p=0; }
    }
    void expandShard(Shard&s,int d,Table&nxt,u64&expc){
        int fr[128],fb[128],fr2[128],fb2[128];
        for(size_t j=0;j<s.cap;j++){ Entry&e=s.tab[j]; if(empty(e)) continue;
            Key k; for(int i=0;i<W;i++) k.w[i]=e.k[i]; decode(k,fr,fb);
            for(int u=0;u+d<=m;u++){ int v=u+d;
                if(fr[u]==0||fr[v]==0||fb[u]==v) continue;
                int a=fb[u], b=fb[v];
                int su=fr[u],sv=fr[v],fa=fb[a],fbb=fb[b],fu=fb[u],fv=fb[v];
                fr[u]--; fr[v]--; fb[a]=b; fb[b]=a;
                Key c; if(!canon(fr,fb,fr2,fb2,c)) g_ecap=true; else add(nxt,c,e.lo,e.hi);
                fr[u]=su; fr[v]=sv; fb[a]=fa; fb[b]=fbb; fb[u]=fu; fb[v]=fv;
                expc++; } }
        free(s.tab); s.tab=nullptr; s.cap=0; s.cnt=0;   // 処理済みの殻は解放
    }
    // ---- チェックポイント ----
    bool ckpt_write(const Table&t,int d) const {
        if(CKPT_DIR.empty()) return true;
        std::string fn=CKPT_DIR+"/ckpt_n"+std::to_string(n)+".bin", tmp=fn+".tmp";
        FILE*f=fopen(tmp.c_str(),"wb"); if(!f) return false;
        u64 hdr[4]={0x47524332ULL,(u64)n,(u64)d,(u64)W}; fwrite(hdr,8,4,f);
        u64 tot=t.states(); fwrite(&tot,8,1,f);
        for(int i=0;i<256;i++){ const Shard&s=t.sh[i]; for(size_t j=0;j<s.cap;j++) if(!empty(s.tab[j])) fwrite(&s.tab[j],sizeof(Entry),1,f); }
        bool ok=(fflush(f)==0); fclose(f); if(!ok) return false;
        return rename(tmp.c_str(),fn.c_str())==0;
    }
    bool ckpt_read(Table&t,int&d) const {
        if(CKPT_DIR.empty()) return false;
        std::string fn=CKPT_DIR+"/ckpt_n"+std::to_string(n)+".bin"; FILE*f=fopen(fn.c_str(),"rb"); if(!f) return false;
        u64 hdr[4],tot; if(fread(hdr,8,4,f)!=4||fread(&tot,8,1,f)!=1||hdr[0]!=0x47524332ULL||(int)hdr[1]!=n||(int)hdr[3]!=W){ fclose(f); return false; }
        d=(int)hdr[2]; Entry e; u64 got=0;
        while(fread(&e,sizeof(Entry),1,f)==1){ Key k; for(int i=0;i<W;i++) k.w[i]=e.k[i]; add(t,k,e.lo,e.hi); got++; }
        fclose(f); if(got!=tot){ t.clear(); return false; } return true;
    }
    // ---- 本体 ----
    bool run(u128&Gout,u64&maxstates,u64&totexp){
        emax = 2*((KB-2*n)/RB); if(emax>32) emax=32;
        Table cur,nxt; int dstart=m;   // dstart = 次に足す差
        int done=-1;
        if(ckpt_read(cur,done)){ dstart=done-1; fprintf(stderr,"チェックポイントから再開: 層 d=%d まで完了, 状態数=%llu\n",done,(unsigned long long)cur.states()); }
        else { int fr[128],fb[128]; for(int u=0;u<n;u++){ fr[u]=2; fb[u]=u; } Key k; encode(fr,fb,k);
               u64 lo=1,hi=0;
#ifdef INIT_HI
               hi=(u64)1<<63; lo=0;     // 桁あふれ検出のテスト用
#endif
               add(cur,k,lo,hi); }
        maxstates=cur.states(); totexp=0;
        for(int d=dstart; d>=1; d--){
            std::atomic<int> nextShard(0); std::vector<u64> expc(NT,0);
            auto worker=[&](int id){ for(;;){ int si=nextShard.fetch_add(1); if(si>=256) break; expandShard(cur.sh[si],d,nxt,expc[id]); } int c=g_emax_seen; while(tl_emax>c && !g_emax_seen.compare_exchange_weak(c,tl_emax)){} };
            if(NT==1) worker(0); else { std::vector<std::thread> th; for(int i=0;i<NT;i++) th.emplace_back(worker,i); for(auto&t:th) t.join(); }
            for(u64 x:expc) totexp+=x;
            if(g_overflow){ fprintf(stderr,"128bit桁あふれを検出 (n=%d, d=%d)。結果は無効。\n",n,d); return false; }
            if(g_ecap){ fprintf(stderr,"端点数が符号長の上限(%d)を超過 (n=%d, d=%d)。結果は無効。\n",emax,n,d); return false; }
            std::swap(cur.sh,nxt.sh); nxt.clear();
            u64 st=cur.states(); if(st>maxstates) maxstates=st;
            if(VERBOSE) fprintf(stderr,"n=%d level d=%d states=%llu\n",n,d,(unsigned long long)st);
            if(!CKPT_DIR.empty() && ((m-d+1)%CKPT_EVERY==0)){ if(!ckpt_write(cur,d)) fprintf(stderr,"警告: チェックポイント書き込み失敗\n"); }
            if(STOP_LEVEL==d){ fprintf(stderr,"--stop-after-level: 層 %d で停止\n",d); exit(3); }
        }
        g_eps.clear();
        { int fr[128],fb[128]; for(int i=0;i<256;i++) for(size_t j=0;j<cur.sh[i].cap;j++){ Entry&e=cur.sh[i].tab[j]; if(empty(e)) continue;
            Key k; for(int q=0;q<W;q++) k.w[q]=e.k[q]; decode(k,fr,fb); int a=-1,b=-1; for(int u=0;u<n;u++) if(fr[u]==1){ if(a<0) a=u; else b=u; }
            g_eps.emplace_back(a,b,((u128)e.hi<<64)|e.lo); } }
        u128 leaves=0; bool ov=false;
        for(int i=0;i<256;i++) for(size_t j=0;j<cur.sh[i].cap;j++){ Entry&e=cur.sh[i].tab[j]; if(empty(e)) continue; u128 c=((u128)e.hi<<64)|e.lo; u128 nl=leaves+c; if(nl<leaves) ov=true; leaves=nl; }
        if(leaves>(~(u128)0)/2) ov=true;
        if(ov||g_overflow){ fprintf(stderr,"最終和で128bit桁あふれ (n=%d)\n",n); return false; }
        Gout=leaves*2; return true;
    }
};

int main(int argc,char**argv){
    if(argc<2){ fprintf(stderr,"usage: %s N_FROM [N_TO] [--threads T] [--results FILE] [--ckpt DIR [--ckpt-every K]] [--endpoints DIR] [--verbose] [--stop-after-level L]\n",argv[0]); return 1; }
    int nfrom=atoi(argv[1]), nto=nfrom; std::string resfile="results.tsv"; int pos=2;
    if(argc>2 && argv[2][0]!='-'){ nto=atoi(argv[2]); pos=3; }
    for(int i=pos;i<argc;i++){ std::string a=argv[i];
        if(a=="--threads"&&i+1<argc) NT=atoi(argv[++i]);
        else if(a=="--results"&&i+1<argc) resfile=argv[++i];
        else if(a=="--ckpt"&&i+1<argc) CKPT_DIR=argv[++i];
        else if(a=="--stop-after-level"&&i+1<argc) STOP_LEVEL=atoi(argv[++i]);
        else if(a=="--ckpt-every"&&i+1<argc) CKPT_EVERY=atoi(argv[++i]);
        else if(a=="--endpoints"&&i+1<argc) ENDP_DIR=argv[++i];
        else if(a=="--verbose") VERBOSE=true;
        else { fprintf(stderr,"不明な引数 %s\n",a.c_str()); return 1; } }
    if(NT<1) NT=1;
    std::map<int,std::string> have;
    { std::ifstream f(resfile); std::string line; while(std::getline(f,line)){ std::istringstream ss(line); int nn; std::string g; if(ss>>nn>>g) have[nn]=g; } }
    for(int nn=nfrom; nn<=nto; nn++){
        if(nn<3||nn>96){ fprintf(stderr,"n=%d は未対応 (3..96)\n",nn); return 1; }
        if(have.count(nn)){ printf("n=%d 結果ファイルに既存 (G=%s)。スキップ\n",nn,have[nn].c_str()); continue; }
        n=nn; m=n-1; g_overflow=false; g_ecap=false; g_emax_seen=0;
        auto t0=std::chrono::steady_clock::now();
        u128 G=0; u64 ms=0, te=0; bool ok;
        if((192-2*n)/5>=10) { Engine<3> e; ok=e.run(G,ms,te); } else { Engine<4> e; ok=e.run(G,ms,te); }
        double sec=std::chrono::duration<double>(std::chrono::steady_clock::now()-t0).count();
        if(!ok){ printf("n=%d 失敗\n",n); return 4; }
        const char* chk="(既知値なし)"; for(auto&k:KNOWN) if(k.n==n) chk=(pr(G)==k.g)?"既知値と一致":"既知値と不一致!!";
        printf("n=%d G=%s  time=%.2fs maxstates/level=%llu expansions=%llu maxendpoints=%d peakRSS=%ldKB threads=%d  [%s]\n",n,pr(G).c_str(),sec,(unsigned long long)ms,(unsigned long long)te,g_emax_seen.load(),vmhwm_kb(),NT,chk);
        fflush(stdout);
        if(!ENDP_DIR.empty()){
            static u128 cnt[96][96]; memset(cnt,0,sizeof(cnt)); bool bad=false;
            for(auto&t:g_eps){ int a=std::get<0>(t), b=std::get<1>(t); u128 M=std::get<2>(t);
                if(a<0||b<0){ bad=true; break; }
                if(a+b==m){ cnt[a][b]=cnt[b][a]=M; }
                else { if(M&1){ bad=true; break; } u128 L=M/2; cnt[a][b]=cnt[b][a]=cnt[m-a][m-b]=cnt[m-b][m-a]=L; } }
            u128 tot=0; for(int a=0;a<n;a++) for(int b=0;b<n;b++) tot+=cnt[a][b];
            if(bad||tot!=G){ fprintf(stderr,"端点別個数の復元に失敗 (n=%d): 総和が G と一致しません\n",n); return 5; }
            std::string fn=ENDP_DIR+"/endpoints_n"+std::to_string(n)+".csv"; FILE*ef=fopen(fn.c_str(),"wb");
            if(!ef){ fprintf(stderr,"端点別CSVを開けません: %s\n",fn.c_str()); return 5; }
            for(int a=0;a<n;a++) for(int b=0;b<n;b++) fprintf(ef,"%d,%d,%d,%s\n",n,a,b,pr(cnt[a][b]).c_str());
            fclose(ef);
        }
        FILE*f=fopen(resfile.c_str(),"a"); if(f){ fprintf(f,"%d\t%s\t%.2f\t%llu\t%llu\t%ld\t%d\t%d\n",n,pr(G).c_str(),sec,(unsigned long long)ms,(unsigned long long)te,vmhwm_kb(),NT,g_emax_seen.load()); fclose(f); }
    }
    return 0;
}
