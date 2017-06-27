/*
 * Copyright (c) 2016-2017 Wuklab, Purdue University. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef _LEGO_MM_TYPES_H
#define _LEGO_MM_TYPES_H

#include <asm/pgtable.h>

#include <lego/types.h>
#include <lego/spinlock.h>
#include <lego/rbtree.h>

typedef unsigned long vm_flags_t;

/*
 * Each physical page in the system has a struct page associated with
 * it to keep track of whatever it is we are using the page for at the
 * moment. Note that we have no way to track which tasks are using
 * a page, though if it is a pagecache page, rmap structures can tell us
 * who is mapping it.
 *
 * The objects in struct page are organized in double word blocks in
 * order to allows us to use atomic double word operations on portions
 * of struct page. That is currently only used by slub but the arrangement
 * allows the use of atomic double word operations on the flags/mapping
 * and lru list pointers also.
 */
struct page {
	unsigned long flags;		/* Atomic flags, some possibly
					 * updated asynchronously */

	union {
		pgoff_t index;		/* Our offset within mapping.
					 * Point to mm_struct if pgd page
					 * Point to pcp chunk if used by pcp
					 */
		void *freelist;		/* slab first free object */
	};

	struct list_head lru;
	unsigned long private;
	atomic_t _mapcount;
	atomic_t _refcount;

#ifdef CONFIG_MEMCOMPONENT
	
#endif
};

/*
 * This struct defines a memory VMM memory area. There is one of these
 * per VM-area/task.  A VM area is any part of the process virtual memory
 * space that has a special rule for the page-fault handlers (ie a shared
 * library, the executable area etc).
 */
struct vm_area_struct {

	/* data structures for managing 
	 * application process address 
	 * space, for processor
	 */
	struct mm_struct *vm_mm;	/* The app address space we belong to. */

	unsigned long vm_start;		/* Our start address within vm_mm. */
	unsigned long vm_end;		/* The first byte after our end address */

	/* linked list of VM areas per task, sorted by address */
	struct vm_area_struct *vm_next, *vm_prev;

	pgprot_t vm_page_prot;		/* Access permissions of this VMA. */
	unsigned long vm_flags;		/* Flags, see mm.h. */

	/* Information about our backing store: */
	char *vm_file;			/* mmap filename */					
	unsigned long vm_pgoff;		/* Offset (within vm_file) in PAGE_SIZE
					   units */

	struct rb_node vm_rb;		/* rb tree to link vma to app mm */

	/* data structures for managing
	 * local kernel addresses
	 * and local link of all vmas
	 */
	unsigned long local_vm_start;
	unsigned long local_vm_end;
	struct rb_node local_vm_rb;		/* rb tree to link vma to current */

};

enum {
	MM_FILEPAGES,	/* Resident file mapping pages */
	MM_ANONPAGES,	/* Resident anonymous pages */
	MM_SWAPENTS,	/* Anonymous swap entries */
	MM_SHMEMPAGES,	/* Resident shared memory pages */
	NR_MM_COUNTERS
};

struct mm_struct {
	struct vm_area_struct *mmap;		/* list of VMAs */
	unsigned long task_size;		/* size of task vm space */
	unsigned long highest_vm_end;		/* highest vma end address */

	atomic_t mm_users;
	atomic_t mm_count;

	pgd_t * pgd;
	int map_count;				/* number of VMAs */

	spinlock_t page_table_lock;		/* Protects page tables and some counters */
	struct list_head mmlist;		/* list of all mm_structs */

	unsigned long total_vm;		/* Total pages mapped */
	unsigned long pinned_vm;	/* Refcount permanently increased */
	unsigned long data_vm;		/* VM_WRITE & ~VM_SHARED & ~VM_STACK */
	unsigned long exec_vm;		/* VM_EXEC & ~VM_WRITE & ~VM_STACK */
	unsigned long stack_vm;		/* VM_STACK */
	unsigned long def_flags;
	unsigned long start_code, end_code, start_data, end_data;
	unsigned long start_brk, brk, start_stack;
	unsigned long arg_start, arg_end, env_start, env_end;

	unsigned long flags; /* Must use atomic bitops to access the bits */

	unsigned long mmap_base;                /* base of mmap area */
	unsigned long cached_hole_size;         /* if non-zero, the largest hole below free_area_cache */
	unsigned long free_area_cache;          /* first hole of size cached_hole_size or larger */

	struct vm_area_struct * mmap_cache;     /* last find_vma result */
	struct rb_root mm_rb;

	int gpid;
	struct list_head list;
};

#endif /* _LEGO_MM_TYPES_H */
