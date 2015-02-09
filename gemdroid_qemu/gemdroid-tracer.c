//Nachi added
#include "gemdroid-tracer.h"
#include "d4-7/d4.h"

//int init_cache();

int IP_tracer = 0;
int MMU_tracer = 0;
int CPU_tracer = 0;
int ICOUNT_tracer = 0;

int current_pid = -1;
char current_pid_path[4096];


//Counters
unsigned long mmu1_ld = 0;
unsigned long mmu2_ld = 0;
unsigned long mmu1_st = 0;
unsigned long mmu2_st = 0;
unsigned long IO_ld = 0;
unsigned long IO_st = 0;

unsigned long cache_hits = 0;
unsigned long cache_misses = 0;
unsigned long this_phase_l1_miss = 0;
unsigned long this_phase_l1_access = 0;
unsigned long this_phase_l2_miss = 0;

unsigned long long total_insts = 0;
unsigned long long new_total_insts = 0;
unsigned long long cpu_cycles = 0;


int64_t curr_gemdroid_tick = 0;

//Should the timer be printed now or not.
int timer_print_flag = 0;
//Should the cpu_inst be printed now or not.
int cpu_inst_print_flag = 1;
int64_t last_ticks = 0;
//Initialize Cache first flag -- should the cache be initialized or not
int first_flag = 0;
int miss_status = 0;

d4cache* Mem;
d4cache* L1;
d4cache* L2;

unsigned long misses_seen_l1 = 0;
unsigned long curr_miss_l1 = 0;
unsigned long misses_seen_l2 = 0;
unsigned long curr_miss_l2 = 0;


int init_cache()
{
  Mem = d4new(0);

  L2 = d4new(Mem);
  L2->name = "L2";
  L2->flags = D4F_CCC;
  L2->lg2blocksize = 6;    //log of block size - 2^5 = 32
  L2->lg2subblocksize = L2->lg2blocksize;
  L2->lg2size = 19;   //log of size = 2^20 (1MB cache)  2^10 bytes is 1KB.
  L2->assoc = 8;
  L2->replacementf = d4rep_lru;
  L2->prefetchf = d4prefetch_none;
  L2->wallocf = d4walloc_always;
  L2->wbackf = d4wback_always;
  L2->name_replacement = L2->name_prefetch = L2->name_walloc = L2->name_wback = "L2";

  L1 = d4new(Mem);
  L1->name = "L1";
  L1->flags = D4F_CCC;
  L1->lg2blocksize = 6;    //log of block size - 2^5 = 32
  L1->lg2subblocksize = L1->lg2blocksize;
  L1->lg2size = 15;   //log of size = 2^20 (1MB cache)  2^10 bytes is 1KB.
  L1->assoc = 2;
  L1->replacementf = d4rep_lru;
  L1->prefetchf = d4prefetch_none;
  L1->wallocf = d4walloc_always;
  L1->wbackf = d4wback_always;
  L1->name_replacement = L1->name_prefetch = L1->name_walloc = L1->name_wback = "L1";

  if(d4setup()!=0)
	{
		printf("\n Error setting up cache!!!\n\n\n\n\n");
	}
	else
	{
		printf("\n Successfully finished cache setup.\n");
	}
	return 0;
}

int check_miss(uint32_t addr, int size, int read_or_write)
{
	this_phase_l1_access++;
	d4memref R;
	R.address = (d4addr)addr;
	//printf("Address being checked : %x\n",R.address);
	R.size = size;
	if(read_or_write == READ_ACCESS)	{
		R.accesstype = D4XREAD;
	}
	else if(read_or_write == WRITE_ACCESS)	{
		R.accesstype = D4XWRITE;
	}

	d4ref(L1, R);

	curr_miss_l1 = L1->miss[D4XREAD] + L1->miss[D4XWRITE];
	curr_miss_l2 = L2->miss[D4XREAD] + L2->miss[D4XWRITE];

	if((curr_miss_l1 == misses_seen_l1) && (curr_miss_l2 == misses_seen_l2)){
		return 0;
	}
	else if(curr_miss_l2 > misses_seen_l2)	{			//First check L2 miss. Because, if its a L2 miss(even an access itself), it would have definitely been a L1 miss.
		this_phase_l2_miss++;
		this_phase_l1_miss++;

		misses_seen_l2 = curr_miss_l2;
		misses_seen_l1 = curr_miss_l1;

		return 2;	//L2 cache miss
	}
	else if(curr_miss_l1 > misses_seen_l1)	{
		misses_seen_l2 = curr_miss_l2;
		misses_seen_l1 = curr_miss_l1;

		this_phase_l1_miss++;
		return 2;	//FOr Nandhini--changed from 1 to 2 to get traces from L1 cache. //L1 cache miss
	}
	else{
		printf("Difference in misses = %ld\n", (curr_miss_l1 - misses_seen_l1) + (curr_miss_l2 - misses_seen_l2));
		printf("Difference in L1 misses = %ld\n", (curr_miss_l1 - misses_seen_l1));
		printf("Difference in l2 misses = %ld\n", (curr_miss_l2 - misses_seen_l2));

		misses_seen_l2 = curr_miss_l2;
		misses_seen_l1 = curr_miss_l1;
		return 0;
	}
}

