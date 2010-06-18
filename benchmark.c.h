/* Included by benchmark.cpp - has same licence as it */

typedef struct BENCHMARK_PREFIX(region_node_s) BENCHMARK_PREFIX(region_node_t);
struct BENCHMARK_PREFIX(region_node_s) {
  REGION_ENTRY(BENCHMARK_PREFIX(region_node_s)) link;
  size_t key;
};
#ifndef BENCHMARK_NOHEADTYPE
typedef struct BENCHMARK_PREFIX(region_tree_s) BENCHMARK_PREFIX(region_tree_t);
REGION_HEAD(BENCHMARK_PREFIX(region_tree_s), BENCHMARK_PREFIX(region_node_s));
#else
REGION_HEAD(BENCHMARK_PREFIX(region_tree_s), BENCHMARK_PREFIX(region_node_s));
typedef struct BENCHMARK_PREFIX(region_node_s) *BENCHMARK_PREFIX(region_tree_t);
#endif

#ifdef BENCHMARK_USEKEYFUNCT
size_t BENCHMARK_PREFIX(regionFunct)(const BENCHMARK_PREFIX(region_node_t) *a)
{
  return a->key;
}
#else
int BENCHMARK_PREFIX(regionFunct)(const BENCHMARK_PREFIX(region_node_t) *a, const BENCHMARK_PREFIX(region_node_t) *b)
{
  return (a->key > b->key) - (a->key < b->key);
}
#endif

REGION_GENERATE(static, BENCHMARK_PREFIX(region_tree_s), BENCHMARK_PREFIX(region_node_s), link, BENCHMARK_PREFIX(regionFunct));

static BENCHMARK_PREFIX(region_tree_t) BENCHMARK_PREFIX(regiontree);

typedef struct BENCHMARK_PREFIX(region_node2_s)
{
  BENCHMARK_PREFIX(region_node_t) node;
#if ITEMSIZE > 0
  char padding[ITEMSIZE<=sizeof(BENCHMARK_PREFIX(region_node_t)) ? 1 : (ITEMSIZE-sizeof(BENCHMARK_PREFIX(region_node_t)))];
#endif
} BENCHMARK_PREFIX(region_node2_t);
static void BENCHMARK_PREFIX(RunTest)(AlgorithmInfo *ai)
{
  static BENCHMARK_PREFIX(region_node2_s) nodes[ALLOCATIONS];
  BENCHMARK_PREFIX(region_node_s) *r;
  int l, n, m;
  usCount start, end;
  printf("Running scalability test for %s\n", ai->name);
  printf("sizeof(REGION_ENTRY)=%d\n", (int) sizeof(nodes[0].node.link));
  srand(1);
  for(n=0; n<ALLOCATIONS; n++)
    nodes[n].node.key=((size_t) rand()<<48)^((size_t) rand()<<32)^((size_t) rand()<<16)^((size_t) rand()<<0);
  REGION_INIT(&BENCHMARK_PREFIX(regiontree));
  for(m=0; m<ALLOCATIONS; m++)
  {
    usCount insert=0, find1=0, find2=0, remove=0, iterate=0;
    int lmax=4*(nedtriebitscanr(ALLOCATIONS)-nedtriebitscanr(m)); /* Loop more when m is smaller */
    if(lmax<1) lmax=1;
    for(l=0; l<lmax; l++)
    {
      for(n=0; n<m; n++)
      {
        int ridx=rand() % (n+1);
        start=GetUsCount();
        REGION_INSERT(BENCHMARK_PREFIX(region_tree_s), &BENCHMARK_PREFIX(regiontree), &(nodes+n)->node);
        end=GetUsCount();
        insert+=end-start;
        start=GetUsCount();
        r=REGION_FIND(BENCHMARK_PREFIX(region_tree_s), &BENCHMARK_PREFIX(regiontree), &(nodes+ridx)->node);
        end=GetUsCount();
        if(!r) abort();
        find1+=end-start;
      }
      for(n=0; n<m; n++)
      {
        start=GetUsCount();
        r=REGION_FIND(BENCHMARK_PREFIX(region_tree_s), &BENCHMARK_PREFIX(regiontree), &(nodes+n)->node);
        end=GetUsCount();
        if(!r) abort();
        find2+=end-start;
      }
      for(r=REGION_MIN(BENCHMARK_PREFIX(region_tree_s), &BENCHMARK_PREFIX(regiontree)); r;)
      {
        start=GetUsCount();
        r=REGION_NEXT(BENCHMARK_PREFIX(region_tree_s), &BENCHMARK_PREFIX(regiontree), r);
        end=GetUsCount();
        iterate+=end-start;
      }
      for(n=0; n<m; n++)
      {
        start=GetUsCount();
        REGION_REMOVE(BENCHMARK_PREFIX(region_tree_s), &BENCHMARK_PREFIX(regiontree), &(nodes+n)->node);
        end=GetUsCount();
        remove+=end-start;
      }
    }
    ai->inserts[m]=(usCount)((double) insert/l);
    ai->finds1[m]=(usCount)((double)find1/l);
    ai->finds2[m]=(usCount)((double)find2/l);
    ai->removes[m]=(usCount)((double)remove/l);
    ai->iterates[m]=(usCount)((double)iterate/l);
  }
}