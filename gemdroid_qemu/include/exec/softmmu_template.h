/*
 *  Software MMU support
 *
 * Generate helpers used by TCG for qemu_ld/st ops and code load
 * functions.
 *
 * Included from target op helpers and exec.c.
 *
 *  Copyright (c) 2003 Fabrice Bellard
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>.
 */
#include "qemu/timer.h"

//GemDroid added
//For GemDroid Tracer Functionality
#include "gemdroid-tracer.h"
//GemDroid end
 
#define DATA_SIZE (1 << SHIFT)

#if DATA_SIZE == 8
#define SUFFIX q
#define LSUFFIX q
#define SDATA_TYPE  int64_t
#elif DATA_SIZE == 4
#define SUFFIX l
#define LSUFFIX l
#define SDATA_TYPE  int32_t
#elif DATA_SIZE == 2
#define SUFFIX w
#define LSUFFIX uw
#define SDATA_TYPE  int16_t
#elif DATA_SIZE == 1
#define SUFFIX b
#define LSUFFIX ub
#define SDATA_TYPE  int8_t
#else
#error unsupported data size
#endif

#define DATA_TYPE   glue(u, SDATA_TYPE)

/* For the benefit of TCG generated code, we want to avoid the complication
   of ABI-specific return type promotion and always return a value extended
   to the register size of the host.  This is tcg_target_long, except in the
   case of a 32-bit host and 64-bit data, and for that we always have
   uint64_t.  Don't bother with this widened value for SOFTMMU_CODE_ACCESS.  */
#if defined(SOFTMMU_CODE_ACCESS) || DATA_SIZE == 8
# define WORD_TYPE  DATA_TYPE
# define USUFFIX    SUFFIX
#else
# define WORD_TYPE  tcg_target_ulong
# define USUFFIX    glue(u, SUFFIX)
# define SSUFFIX    glue(s, SUFFIX)
#endif

#ifdef SOFTMMU_CODE_ACCESS
#define READ_ACCESS_TYPE 2
#define ADDR_READ addr_code
#else
#define READ_ACCESS_TYPE 0
#define ADDR_READ addr_read
#endif

#if DATA_SIZE == 8
# define BSWAP(X)  bswap64(X)
#elif DATA_SIZE == 4
# define BSWAP(X)  bswap32(X)
#elif DATA_SIZE == 2
# define BSWAP(X)  bswap16(X)
#else
# define BSWAP(X)  (X)
#endif

#ifdef TARGET_WORDS_BIGENDIAN
# define TGT_BE(X)  (X)
# define TGT_LE(X)  BSWAP(X)
#else
# define TGT_BE(X)  BSWAP(X)
# define TGT_LE(X)  (X)
#endif

#if DATA_SIZE == 1
# define helper_le_ld_name  glue(glue(helper_ret_ld, USUFFIX), MMUSUFFIX)
# define helper_be_ld_name  helper_le_ld_name
# define helper_le_lds_name glue(glue(helper_ret_ld, SSUFFIX), MMUSUFFIX)
# define helper_be_lds_name helper_le_lds_name
# define helper_le_st_name  glue(glue(helper_ret_st, SUFFIX), MMUSUFFIX)
# define helper_be_st_name  helper_le_st_name
#else
# define helper_le_ld_name  glue(glue(helper_le_ld, USUFFIX), MMUSUFFIX)
# define helper_be_ld_name  glue(glue(helper_be_ld, USUFFIX), MMUSUFFIX)
# define helper_le_lds_name glue(glue(helper_le_ld, SSUFFIX), MMUSUFFIX)
# define helper_be_lds_name glue(glue(helper_be_ld, SSUFFIX), MMUSUFFIX)
# define helper_le_st_name  glue(glue(helper_le_st, SUFFIX), MMUSUFFIX)
# define helper_be_st_name  glue(glue(helper_be_st, SUFFIX), MMUSUFFIX)
#endif

#ifdef TARGET_WORDS_BIGENDIAN
# define helper_te_ld_name  helper_be_ld_name
# define helper_te_st_name  helper_be_st_name
#else
# define helper_te_ld_name  helper_le_ld_name
# define helper_te_st_name  helper_le_st_name
#endif


static inline DATA_TYPE glue(io_read, SUFFIX)(CPUArchState *env,
                                              hwaddr physaddr,
                                              target_ulong addr,
                                              uintptr_t retaddr)
{
    uint64_t val;
    int index = (physaddr >> IO_MEM_SHIFT) & (IO_MEM_NB_ENTRIES - 1);
    physaddr = (physaddr & TARGET_PAGE_MASK) + addr;
    env->mem_io_pc = retaddr;
    if (index > (IO_MEM_NOTDIRTY >> IO_MEM_SHIFT)
            && !can_do_io(env)) {
        cpu_io_recompile(env, retaddr);
    }

    env->mem_io_vaddr = addr;
#if SHIFT <= 2
    val = io_mem_read(index, physaddr, 1 << SHIFT);
#else
#ifdef TARGET_WORDS_BIGENDIAN
    val = (uint64_t)io_mem_read(index, physaddr, 4) << 32;
    val |= io_mem_read(index, physaddr + 4, 4);
#else
    val = io_mem_read(index, physaddr, 4);
    val |= (uint64_t)io_mem_read(index, physaddr + 4, 4) << 32;
#endif
#endif /* SHIFT > 2 */
    return val;
}

#ifdef SOFTMMU_CODE_ACCESS
static __attribute__((unused))
#endif
WORD_TYPE helper_le_ld_name(CPUArchState *env, target_ulong addr, int mmu_idx,
                            uintptr_t retaddr)
{
    int index = (addr >> TARGET_PAGE_BITS) & (CPU_TLB_SIZE - 1);
    target_ulong tlb_addr = env->tlb_table[mmu_idx][index].ADDR_READ;
    uintptr_t haddr;
    DATA_TYPE res;

    /* Adjust the given return address.  */
    retaddr -= GETPC_ADJ;

    //GemDroid Added
    //GemDroid INITS THE Dinero Cache
    if(first_flag == false)
    {
       init_cache();
       first_flag = true;
       cpu_cycles = 0;
    }

    // GemDroid adds - print tick
    if(MMU_tracer && timer_print_flag) {
       //printf("MS: %ld\n", cpu_get_ticks()/1000 - last_ticks);
       last_ticks = cpu_get_ticks()/1000;
       timer_print_flag = false;
    }

    //GemDroid added -- print cpu_inst count before printing loads/stores
    //GemDroid adds - Print icount for each BB
     if(ICOUNT_tracer)
     {
       if(env->cpu_inst_counter != 0 && cpu_inst_print_flag)
       {
               //printf("CPU %llu %lu \t", cpu_cycles, env->cpu_inst_counter);
               printf("CPUSummary %llu \t", env->cpu_inst_counter);
               printf("%lu \t %lu \t %lu \n", this_phase_l1_access, this_phase_l1_miss, this_phase_l2_miss);

               cpu_cycles+=env->cpu_inst_counter;
               total_insts+=env->cpu_inst_counter;
               env->cpu_inst_counter = 0;

               cpu_inst_print_flag = false;
               this_phase_l2_miss = 0;
               this_phase_l1_miss = 0;
               this_phase_l1_access = 0;
       }
     }
    //GemDroid end

    /* If the TLB entry is for a different page, reload and try again.  */
    if ((addr & TARGET_PAGE_MASK)
         != (tlb_addr & (TARGET_PAGE_MASK | TLB_INVALID_MASK))) {
#ifdef ALIGNED_ONLY
        if ((addr & (DATA_SIZE - 1)) != 0) {
            do_unaligned_access(env, addr, READ_ACCESS_TYPE, mmu_idx, retaddr);
        }
#endif
        tlb_fill(env, addr, READ_ACCESS_TYPE, mmu_idx, retaddr);
        tlb_addr = env->tlb_table[mmu_idx][index].ADDR_READ;
    }

    /* Handle an IO access.  */
    if (unlikely(tlb_addr & ~TARGET_PAGE_MASK)) {
        hwaddr ioaddr;
        if ((addr & (DATA_SIZE - 1)) != 0) {
            goto do_unaligned_access;
        }
        ioaddr = env->iotlb[mmu_idx][index];

        //GemDroid Added
        if(MMU_tracer)
        {
            //printf("IO ld %x %d\n",addr, DATA_SIZE);
            IO_ld+=DATA_SIZE;
            cpu_cycles+=DRAM_latency;
            timer_print_flag = true;
            cpu_inst_print_flag = false;
        }
        //GemDroid end

        /* ??? Note that the io helpers always read data in the target
           byte ordering.  We should push the LE/BE request down into io.  */
        res = glue(io_read, SUFFIX)(env, ioaddr, addr, retaddr);
        res = TGT_LE(res);
        return res;
    }

    /* Handle slow unaligned access (it spans two pages or IO).  */
    if (DATA_SIZE > 1
        && unlikely((addr & ~TARGET_PAGE_MASK) + DATA_SIZE - 1
                    >= TARGET_PAGE_SIZE)) {
        target_ulong addr1, addr2;
        DATA_TYPE res1, res2;
        unsigned shift;
    do_unaligned_access:
#ifdef ALIGNED_ONLY
        do_unaligned_access(env, addr, READ_ACCESS_TYPE, mmu_idx, retaddr);
#endif
        //GemDroid Added
        if(MMU_tracer)
        {
        	miss_status = check_miss(addr, DATA_SIZE,READ_ACCESS);
            if(miss_status)   //if miss, print address
			{
        		mmu1_ld+=DATA_SIZE;
        		if(miss_status == 2)// A L2 miss
        		{
                int64_t curr_ticks = qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL);
                int64_t diff_ticks =  curr_ticks - curr_gemdroid_tick;
                curr_gemdroid_tick = curr_ticks;

        		printf("CPU %llu ", env->cpu_inst_counter);
                printf("%" PRId64  "\n",diff_ticks);
        		// printf("total_insts %llu \n",new_total_insts);
        		new_total_insts = 0;
        		env->cpu_inst_counter = 0;
        		uint32_t phys_addr = cpu_get_phys_page_debug(env, addr);
        		printf("MMU_ld %x %d\n", phys_addr, DATA_SIZE);
        		cpu_cycles+=DRAM_latency;
        		}
        		else if(miss_status == 1) // A L1 miss
        		{
        		   cpu_cycles+=8;
        		}
      			timer_print_flag = true;
      			cpu_inst_print_flag = false;
      		}
//             else
//                     printf("\n HIT");
        }
        //GemDroid end

        addr1 = addr & ~(DATA_SIZE - 1);
        addr2 = addr1 + DATA_SIZE;
        /* Note the adjustment at the beginning of the function.
           Undo that for the recursion.  */
        res1 = helper_le_ld_name(env, addr1, mmu_idx, retaddr + GETPC_ADJ);
        res2 = helper_le_ld_name(env, addr2, mmu_idx, retaddr + GETPC_ADJ);
        shift = (addr & (DATA_SIZE - 1)) * 8;

        /* Little-endian combine.  */
        res = (res1 >> shift) | (res2 << ((DATA_SIZE * 8) - shift));
        return res;
    }

    /* Handle aligned access or unaligned access in the same page.  */
#ifdef ALIGNED_ONLY
    if ((addr & (DATA_SIZE - 1)) != 0) {
        do_unaligned_access(env, addr, READ_ACCESS_TYPE, mmu_idx, retaddr);
    }
#endif

    haddr = addr + env->tlb_table[mmu_idx][index].addend;
    
	//GemDroid Added
    if(MMU_tracer)
    {
    	miss_status = check_miss(addr, DATA_SIZE,READ_ACCESS);
        if(miss_status)   //if miss, print address
		{
        	mmu2_ld+=DATA_SIZE;
			if(miss_status == 2)
			{
            	int64_t curr_ticks = qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL);
            	int64_t diff_ticks =  curr_ticks - curr_gemdroid_tick;
            	curr_gemdroid_tick = curr_ticks;

		    	printf("CPU %llu ", env->cpu_inst_counter);
            	printf("%" PRId64  "\n",diff_ticks);

				env->cpu_inst_counter = 0;
				// printf("total_insts %llu \n",new_total_insts);
				new_total_insts = 0;
				uint32_t phys_addr = cpu_get_phys_page_debug(env, addr);
				printf("MMU_ld %x %d\n", phys_addr, DATA_SIZE);
				cpu_cycles+=DRAM_latency;
			}
			else if(miss_status == 1)
			{
		        cpu_cycles+=8;
			}

			timer_print_flag = true;
			cpu_inst_print_flag = false;
		}
        //else
        //printf("\n HIT");
     }
     //GemDroid end

#if DATA_SIZE == 1
    res = glue(glue(ld, LSUFFIX), _p)((uint8_t *)haddr);
#else
    res = glue(glue(ld, LSUFFIX), _le_p)((uint8_t *)haddr);
#endif
    return res;
}

#if DATA_SIZE > 1
#ifdef SOFTMMU_CODE_ACCESS
static __attribute__((unused))
#endif
WORD_TYPE helper_be_ld_name(CPUArchState *env, target_ulong addr, int mmu_idx,
                            uintptr_t retaddr)
{
    int index = (addr >> TARGET_PAGE_BITS) & (CPU_TLB_SIZE - 1);
    target_ulong tlb_addr = env->tlb_table[mmu_idx][index].ADDR_READ;
    uintptr_t haddr;
    DATA_TYPE res;

    //GemDroid Added
    //GemDroid INITS THE Dinero Cache
    if(first_flag == false)
    {
       init_cache();
       first_flag = true;
       cpu_cycles = 0;
    }

    // GemDroid adds - print tick
    if(MMU_tracer && timer_print_flag) {
       //printf("MS: %ld\n", cpu_get_ticks()/1000 - last_ticks);
       last_ticks = cpu_get_ticks()/1000;
       timer_print_flag = false;
    }

    //GemDroid added -- print cpu_inst count before printing loads/stores
    //GemDroid adds - Print icount for each BB
     if(ICOUNT_tracer)
     {
       if(env->cpu_inst_counter != 0 && cpu_inst_print_flag)
       {
               //printf("CPU %llu %lu \t", cpu_cycles, env->cpu_inst_counter);
               printf("CPUSummary %llu \t", env->cpu_inst_counter);
               printf("%lu \t %lu \t %lu \n", this_phase_l1_access, this_phase_l1_miss, this_phase_l2_miss);

               cpu_cycles+=env->cpu_inst_counter;
               total_insts+=env->cpu_inst_counter;
               env->cpu_inst_counter = 0;

               cpu_inst_print_flag = false;
               this_phase_l2_miss = 0;
               this_phase_l1_miss = 0;
               this_phase_l1_access = 0;
       }
     }
    //GemDroid end

    /* Adjust the given return address.  */
    retaddr -= GETPC_ADJ;

    /* If the TLB entry is for a different page, reload and try again.  */
    if ((addr & TARGET_PAGE_MASK)
         != (tlb_addr & (TARGET_PAGE_MASK | TLB_INVALID_MASK))) {
#ifdef ALIGNED_ONLY
        if ((addr & (DATA_SIZE - 1)) != 0) {
            do_unaligned_access(env, addr, READ_ACCESS_TYPE, mmu_idx, retaddr);
        }
#endif
        tlb_fill(env, addr, READ_ACCESS_TYPE, mmu_idx, retaddr);
        tlb_addr = env->tlb_table[mmu_idx][index].ADDR_READ;
    }

    /* Handle an IO access.  */
    if (unlikely(tlb_addr & ~TARGET_PAGE_MASK)) {
        hwaddr ioaddr;
        if ((addr & (DATA_SIZE - 1)) != 0) {
            goto do_unaligned_access;
        }
        ioaddr = env->iotlb[mmu_idx][index];

        //GemDroid Added
        if(MMU_tracer)
        {
            //printf("IO ld %x %d\n",addr, DATA_SIZE);
            IO_ld+=DATA_SIZE;
            cpu_cycles+=DRAM_latency;
            timer_print_flag = true;
            cpu_inst_print_flag = false;
        }
        //GemDroid end

        /* ??? Note that the io helpers always read data in the target
           byte ordering.  We should push the LE/BE request down into io.  */
        res = glue(io_read, SUFFIX)(env, ioaddr, addr, retaddr);
        res = TGT_BE(res);
        return res;
    }

    /* Handle slow unaligned access (it spans two pages or IO).  */
    if (DATA_SIZE > 1
        && unlikely((addr & ~TARGET_PAGE_MASK) + DATA_SIZE - 1
                    >= TARGET_PAGE_SIZE)) {
        target_ulong addr1, addr2;
        DATA_TYPE res1, res2;
        unsigned shift;
    do_unaligned_access:
#ifdef ALIGNED_ONLY
        do_unaligned_access(env, addr, READ_ACCESS_TYPE, mmu_idx, retaddr);
#endif
        //GemDroid Added
        if(MMU_tracer)
        {
        	miss_status = check_miss(addr, DATA_SIZE,READ_ACCESS);
            if(miss_status)   //if miss, print address
			{
        		mmu1_ld+=DATA_SIZE;
        		if(miss_status == 2)// A L2 miss
        		{
                int64_t curr_ticks = qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL);
                int64_t diff_ticks =  curr_ticks - curr_gemdroid_tick;
                curr_gemdroid_tick = curr_ticks;

        		printf("CPU %llu ", env->cpu_inst_counter);
                printf("%" PRId64  "\n",diff_ticks);
        		// printf("total_insts %llu \n",new_total_insts);
        		new_total_insts = 0;
        		env->cpu_inst_counter = 0;
        		uint32_t phys_addr = cpu_get_phys_page_debug(env, addr);
        		printf("MMU_ld %x %d\n", phys_addr, DATA_SIZE);
        		cpu_cycles+=DRAM_latency;
        		}
        		else if(miss_status == 1) // A L1 miss
        		{
        		   cpu_cycles+=8;
        		}
      			timer_print_flag = true;
      			cpu_inst_print_flag = false;
      		}
//             else
//                     printf("\n HIT");
        }
        //GemDroid end

        addr1 = addr & ~(DATA_SIZE - 1);
        addr2 = addr1 + DATA_SIZE;
        /* Note the adjustment at the beginning of the function.
           Undo that for the recursion.  */
        res1 = helper_be_ld_name(env, addr1, mmu_idx, retaddr + GETPC_ADJ);
        res2 = helper_be_ld_name(env, addr2, mmu_idx, retaddr + GETPC_ADJ);
        shift = (addr & (DATA_SIZE - 1)) * 8;

        /* Big-endian combine.  */
        res = (res1 << shift) | (res2 >> ((DATA_SIZE * 8) - shift));
        return res;
    }

    /* Handle aligned access or unaligned access in the same page.  */
#ifdef ALIGNED_ONLY
    if ((addr & (DATA_SIZE - 1)) != 0) {
        do_unaligned_access(env, addr, READ_ACCESS_TYPE, mmu_idx, retaddr);
    }
#endif

	//GemDroid Added 
	//in same page
    if(MMU_tracer)
    {
    	miss_status = check_miss(addr, DATA_SIZE,READ_ACCESS);
        if(miss_status)   //if miss, print address
		{
        	mmu2_ld+=DATA_SIZE;
			if(miss_status == 2)
			{
            	int64_t curr_ticks = qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL);
            	int64_t diff_ticks =  curr_ticks - curr_gemdroid_tick;
            	curr_gemdroid_tick = curr_ticks;

		    	printf("CPU %llu ", env->cpu_inst_counter);
            	printf("%" PRId64  "\n",diff_ticks);

				env->cpu_inst_counter = 0;
				// printf("total_insts %llu \n",new_total_insts);
				new_total_insts = 0;
				uint32_t phys_addr = cpu_get_phys_page_debug(env, addr);
				printf("MMU_ld %x %d\n", phys_addr, DATA_SIZE);
				cpu_cycles+=DRAM_latency;
			}
			else if(miss_status == 1)
			{
		        cpu_cycles+=8;
			}

			timer_print_flag = true;
			cpu_inst_print_flag = false;
		}
        //else
        //printf("\n HIT");
     }
     //GemDroid end

    haddr = addr + env->tlb_table[mmu_idx][index].addend;
    res = glue(glue(ld, LSUFFIX), _be_p)((uint8_t *)haddr);
    return res;
}
#endif /* DATA_SIZE > 1 */

DATA_TYPE
glue(glue(helper_ld, SUFFIX), MMUSUFFIX)(CPUArchState *env, target_ulong addr,
                                         int mmu_idx)
{
    return helper_te_ld_name (env, addr, mmu_idx, GETRA());
}

#ifndef SOFTMMU_CODE_ACCESS

/* Provide signed versions of the load routines as well.  We can of course
   avoid this for 64-bit data, or for 32-bit data on 32-bit host.  */
#if DATA_SIZE * 8 < TCG_TARGET_REG_BITS
WORD_TYPE helper_le_lds_name(CPUArchState *env, target_ulong addr,
                             int mmu_idx, uintptr_t retaddr)
{
    return (SDATA_TYPE)helper_le_ld_name(env, addr, mmu_idx, retaddr);
}

# if DATA_SIZE > 1
WORD_TYPE helper_be_lds_name(CPUArchState *env, target_ulong addr,
                             int mmu_idx, uintptr_t retaddr)
{
    return (SDATA_TYPE)helper_be_ld_name(env, addr, mmu_idx, retaddr);
}
# endif
#endif

static inline void glue(io_write, SUFFIX)(CPUArchState *env,
                                          hwaddr physaddr,
                                          DATA_TYPE val,
                                          target_ulong addr,
                                          uintptr_t retaddr)
{
    int index = (physaddr >> IO_MEM_SHIFT) & (IO_MEM_NB_ENTRIES - 1);
    physaddr = (physaddr & TARGET_PAGE_MASK) + addr;
    if (index > (IO_MEM_NOTDIRTY >> IO_MEM_SHIFT)
            && !can_do_io(env)) {
        cpu_io_recompile(env, retaddr);
    }

    env->mem_io_vaddr = addr;
    env->mem_io_pc = retaddr;
#if SHIFT <= 2
    io_mem_write(index, physaddr, val, 1 << SHIFT);
#else
#ifdef TARGET_WORDS_BIGENDIAN
    io_mem_write(index, physaddr, val >> 32, 4);
    io_mem_write(index, physaddr + 4, val, 4);
#else
    io_mem_write(index, physaddr, val, 4);
    io_mem_write(index, physaddr + 4, val >> 32, 4);
#endif
#endif /* SHIFT > 2 */
}

void helper_le_st_name(CPUArchState *env, target_ulong addr, DATA_TYPE val,
                       int mmu_idx, uintptr_t retaddr)
{
    int index = (addr >> TARGET_PAGE_BITS) & (CPU_TLB_SIZE - 1);
    target_ulong tlb_addr = env->tlb_table[mmu_idx][index].addr_write;
    uintptr_t haddr;

    //GemDroid Added
    //print tick
    if(MMU_tracer && timer_print_flag) {
       // printf("MS: %ld\n", cpu_get_ticks()/1000 - last_ticks);
       last_ticks = cpu_get_ticks()/1000;
       timer_print_flag = false;
    }
    //print cpu_inst count before printing loads/stores
     if(ICOUNT_tracer)
     {
       if(env->cpu_inst_counter != 0 && cpu_inst_print_flag)
       {
               //printf("CPU %llu %lu \t", cpu_cycles, env->cpu_inst_counter);
               printf("CPUSummary %llu \t", env->cpu_inst_counter);
               //fflush(stdout);
               printf("%lu \t %lu \t %lu \n", this_phase_l1_access, this_phase_l1_miss, this_phase_l2_miss);
               total_insts+=env->cpu_inst_counter;
               cpu_cycles+=env->cpu_inst_counter;
               cpu_inst_print_flag = false;
               env->cpu_inst_counter = 0;
               this_phase_l2_miss = 0;
               this_phase_l1_miss = 0;
               this_phase_l1_access = 0;
       }
     }
     //GemDroid End

    /* Adjust the given return address.  */
    retaddr -= GETPC_ADJ;

    /* If the TLB entry is for a different page, reload and try again.  */
    if ((addr & TARGET_PAGE_MASK)
        != (tlb_addr & (TARGET_PAGE_MASK | TLB_INVALID_MASK))) {
#ifdef ALIGNED_ONLY
        if ((addr & (DATA_SIZE - 1)) != 0) {
            do_unaligned_access(env, addr, 1, mmu_idx, retaddr);
        }
#endif
        tlb_fill(env, addr, 1, mmu_idx, retaddr);
        tlb_addr = env->tlb_table[mmu_idx][index].addr_write;
    }

    /* Handle an IO access.  */
    if (unlikely(tlb_addr & ~TARGET_PAGE_MASK)) {
        hwaddr ioaddr;
        if ((addr & (DATA_SIZE - 1)) != 0) {
            goto do_unaligned_access;
        }
        ioaddr = env->iotlb[mmu_idx][index];

        //GemDroid Added
        if(MMU_tracer)
        {
         	// printf("IO st %x %d\n",addr, DATA_SIZE);
          	IO_st+=DATA_SIZE;
           	cpu_cycles+=DRAM_latency;
           	timer_print_flag = true;
           	cpu_inst_print_flag = false;
        }
        //GemDroid End

        /* ??? Note that the io helpers always read data in the target
           byte ordering.  We should push the LE/BE request down into io.  */
        val = TGT_LE(val);
        glue(io_write, SUFFIX)(env, ioaddr, val, addr, retaddr);
        return;
    }

    /* Handle slow unaligned access (it spans two pages or IO).  */
    if (DATA_SIZE > 1
        && unlikely((addr & ~TARGET_PAGE_MASK) + DATA_SIZE - 1
                     >= TARGET_PAGE_SIZE)) {
        int i;
    do_unaligned_access:
#ifdef ALIGNED_ONLY
        do_unaligned_access(env, addr, 1, mmu_idx, retaddr);
#endif
        //GemDroid Added
		if(MMU_tracer)
		{
			miss_status = check_miss(addr, DATA_SIZE,WRITE_ACCESS);
			if(miss_status)   //if miss, print address
			{
				mmu1_st+=DATA_SIZE;
				if(miss_status == 2)
				{
                	int64_t curr_ticks = qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL);
               		int64_t diff_ticks =  curr_ticks - curr_gemdroid_tick;
               		curr_gemdroid_tick = curr_ticks;

               		printf("CPU %llu ", env->cpu_inst_counter);
               		printf("%" PRId64  "\n",diff_ticks);


  				    env->cpu_inst_counter = 0;
					// printf("total_insts %llu \n",new_total_insts);
					new_total_insts = 0;
					uint32_t phys_addr = cpu_get_phys_page_debug(env, addr);
					printf("MMU_st %x %d\n",phys_addr, DATA_SIZE);
					cpu_cycles+=DRAM_latency;
				}
				else if(miss_status == 1)
				{
					   cpu_cycles+=8;
				}
				timer_print_flag = true;
				cpu_inst_print_flag = false;
			}
		}
        //GemDroid End

        /* XXX: not efficient, but simple */
        /* Note: relies on the fact that tlb_fill() does not remove the
         * previous page from the TLB cache.  */
        for (i = DATA_SIZE - 1; i >= 0; i--) {
            /* Little-endian extract.  */
            uint8_t val8 = val >> (i * 8);
            /* Note the adjustment at the beginning of the function.
               Undo that for the recursion.  */
            glue(helper_ret_stb, MMUSUFFIX)(env, addr + i, val8,
                                            mmu_idx, retaddr + GETPC_ADJ);
        }
        return;
    }

    /* Handle aligned access or unaligned access in the same page.  */
#ifdef ALIGNED_ONLY
    if ((addr & (DATA_SIZE - 1)) != 0) {
        do_unaligned_access(env, addr, 1, mmu_idx, retaddr);
    }
#endif

    //GemDroid Added
	if(MMU_tracer)
	{
	  	miss_status = check_miss(addr, DATA_SIZE,WRITE_ACCESS);
		if(miss_status)   //if miss, print address
		{
			mmu2_st+=DATA_SIZE;
			if(miss_status == 2)
			{
            int64_t curr_ticks = qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL);
            int64_t diff_ticks =  curr_ticks - curr_gemdroid_tick;
            curr_gemdroid_tick = curr_ticks;

            printf("CPU %llu ", env->cpu_inst_counter);
            printf("%" PRId64  "\n",diff_ticks);

			total_insts += env->cpu_inst_counter;
			env->cpu_inst_counter = 0;
	     	// printf("total_insts %llu \n",new_total_insts);
			new_total_insts = 0;
			uint32_t phys_addr = cpu_get_phys_page_debug(env, addr);
			printf("MMU_st %x %d\n",phys_addr, DATA_SIZE);
			cpu_cycles+=DRAM_latency;
			}
			else if(miss_status == 1)
			{
				cpu_cycles+=8;
			}
			timer_print_flag = true;
			cpu_inst_print_flag = false;
		}
	}
    //GemDroid End
    
	haddr = addr + env->tlb_table[mmu_idx][index].addend;
#if DATA_SIZE == 1
    glue(glue(st, SUFFIX), _p)((uint8_t *)haddr, val);
#else
    glue(glue(st, SUFFIX), _le_p)((uint8_t *)haddr, val);
#endif
}

#if DATA_SIZE > 1
void helper_be_st_name(CPUArchState *env, target_ulong addr, DATA_TYPE val,
                       int mmu_idx, uintptr_t retaddr)
{
    int index = (addr >> TARGET_PAGE_BITS) & (CPU_TLB_SIZE - 1);
    target_ulong tlb_addr = env->tlb_table[mmu_idx][index].addr_write;
    uintptr_t haddr;

    /* Adjust the given return address.  */
    retaddr -= GETPC_ADJ;

    //GemDroid Added
    //print tick
    if(MMU_tracer && timer_print_flag) {
       // printf("MS: %ld\n", cpu_get_ticks()/1000 - last_ticks);
       last_ticks = cpu_get_ticks()/1000;
       timer_print_flag = false;
    }
    //print cpu_inst count before printing loads/stores
     if(ICOUNT_tracer)
     {
       if(env->cpu_inst_counter != 0 && cpu_inst_print_flag)
       {
               //printf("CPU %llu %lu \t", cpu_cycles, env->cpu_inst_counter);
               printf("CPUSummary %llu \t", env->cpu_inst_counter);
               //fflush(stdout);
               printf("%lu \t %lu \t %lu \n", this_phase_l1_access, this_phase_l1_miss, this_phase_l2_miss);
               total_insts+=env->cpu_inst_counter;
               cpu_cycles+=env->cpu_inst_counter;
               cpu_inst_print_flag = false;
               env->cpu_inst_counter = 0;
               this_phase_l2_miss = 0;
               this_phase_l1_miss = 0;
               this_phase_l1_access = 0;
       }
     }
     //GemDroid End

    /* If the TLB entry is for a different page, reload and try again.  */
    if ((addr & TARGET_PAGE_MASK)
        != (tlb_addr & (TARGET_PAGE_MASK | TLB_INVALID_MASK))) {
#ifdef ALIGNED_ONLY
        if ((addr & (DATA_SIZE - 1)) != 0) {
            do_unaligned_access(env, addr, 1, mmu_idx, retaddr);
        }
#endif
        tlb_fill(env, addr, 1, mmu_idx, retaddr);
        tlb_addr = env->tlb_table[mmu_idx][index].addr_write;
    }

    /* Handle an IO access.  */
    if (unlikely(tlb_addr & ~TARGET_PAGE_MASK)) {
        hwaddr ioaddr;
        if ((addr & (DATA_SIZE - 1)) != 0) {
            goto do_unaligned_access;
        }
        ioaddr = env->iotlb[mmu_idx][index];
        
		//GemDroid Added
        if(MMU_tracer)
        {
         	// printf("IO st %x %d\n",addr, DATA_SIZE);
          	IO_st+=DATA_SIZE;
           	cpu_cycles+=DRAM_latency;
           	timer_print_flag = true;
           	cpu_inst_print_flag = false;
        }
        //GemDroid End

        /* ??? Note that the io helpers always read data in the target
           byte ordering.  We should push the LE/BE request down into io.  */
        val = TGT_BE(val);
        glue(io_write, SUFFIX)(env, ioaddr, val, addr, retaddr);
        return;
    }

    /* Handle slow unaligned access (it spans two pages or IO).  */
    if (DATA_SIZE > 1
        && unlikely((addr & ~TARGET_PAGE_MASK) + DATA_SIZE - 1
                     >= TARGET_PAGE_SIZE)) {
        int i;
    do_unaligned_access:
#ifdef ALIGNED_ONLY
        do_unaligned_access(env, addr, 1, mmu_idx, retaddr);
#endif

        //GemDroid Added
		if(MMU_tracer)
		{
			miss_status = check_miss(addr, DATA_SIZE,WRITE_ACCESS);
			if(miss_status)   //if miss, print address
			{
				mmu1_st+=DATA_SIZE;
				if(miss_status == 2)
				{
                	int64_t curr_ticks = qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL);
               		int64_t diff_ticks =  curr_ticks - curr_gemdroid_tick;
               		curr_gemdroid_tick = curr_ticks;

               		printf("CPU %llu ", env->cpu_inst_counter);
               		printf("%" PRId64  "\n",diff_ticks);


  				    env->cpu_inst_counter = 0;
					// printf("total_insts %llu \n",new_total_insts);
					new_total_insts = 0;
					uint32_t phys_addr = cpu_get_phys_page_debug(env, addr);
					printf("MMU_st %x %d\n",phys_addr, DATA_SIZE);
					cpu_cycles+=DRAM_latency;
				}
				else if(miss_status == 1)
				{
					   cpu_cycles+=8;
				}
				timer_print_flag = true;
				cpu_inst_print_flag = false;
			}
		}
        //GemDroid End

        /* XXX: not efficient, but simple */
        /* Note: relies on the fact that tlb_fill() does not remove the
         * previous page from the TLB cache.  */
        for (i = DATA_SIZE - 1; i >= 0; i--) {
            /* Big-endian extract.  */
            uint8_t val8 = val >> (((DATA_SIZE - 1) * 8) - (i * 8));
            /* Note the adjustment at the beginning of the function.
               Undo that for the recursion.  */
            glue(helper_ret_stb, MMUSUFFIX)(env, addr + i, val8,
                                            mmu_idx, retaddr + GETPC_ADJ);
        }
        return;
    }

    /* Handle aligned access or unaligned access in the same page.  */
#ifdef ALIGNED_ONLY
    if ((addr & (DATA_SIZE - 1)) != 0) {
        do_unaligned_access(env, addr, 1, mmu_idx, retaddr);
    }
#endif

    //GemDroid Added
	if(MMU_tracer)
	{
	  	miss_status = check_miss(addr, DATA_SIZE,WRITE_ACCESS);
		if(miss_status)   //if miss, print address
		{
			mmu2_st+=DATA_SIZE;
			if(miss_status == 2)
			{
            int64_t curr_ticks = qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL);
            int64_t diff_ticks =  curr_ticks - curr_gemdroid_tick;
            curr_gemdroid_tick = curr_ticks;

            printf("CPU %llu ", env->cpu_inst_counter);
            printf("%" PRId64  "\n",diff_ticks);

			total_insts += env->cpu_inst_counter;
			env->cpu_inst_counter = 0;
	     	// printf("total_insts %llu \n",new_total_insts);
			new_total_insts = 0;
			uint32_t phys_addr = cpu_get_phys_page_debug(env, addr);
			printf("MMU_st %x %d\n",phys_addr, DATA_SIZE);
			cpu_cycles+=DRAM_latency;
			}
			else if(miss_status == 1)
			{
				cpu_cycles+=8;
			}
			timer_print_flag = true;
			cpu_inst_print_flag = false;
		}
	}
    //GemDroid End

    haddr = addr + env->tlb_table[mmu_idx][index].addend;
    glue(glue(st, SUFFIX), _be_p)((uint8_t *)haddr, val);
}
#endif /* DATA_SIZE > 1 */

void
glue(glue(helper_st, SUFFIX), MMUSUFFIX)(CPUArchState *env, target_ulong addr,
                                         DATA_TYPE val, int mmu_idx)
{
    helper_te_st_name(env, addr, val, mmu_idx, GETRA());
}

#endif /* !defined(SOFTMMU_CODE_ACCESS) */

#undef READ_ACCESS_TYPE
#undef SHIFT
#undef DATA_TYPE
#undef SUFFIX
#undef LSUFFIX
#undef DATA_SIZE
#undef ADDR_READ
#undef WORD_TYPE
#undef SDATA_TYPE
#undef USUFFIX
#undef SSUFFIX
#undef BSWAP
#undef TGT_BE
#undef TGT_LE
#undef CPU_BE
#undef CPU_LE
#undef helper_le_ld_name
#undef helper_be_ld_name
#undef helper_le_lds_name
#undef helper_be_lds_name
#undef helper_le_st_name
#undef helper_be_st_name
#undef helper_te_ld_name
#undef helper_te_st_name
