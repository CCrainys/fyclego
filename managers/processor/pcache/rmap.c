/*
 * Copyright (c) 2016-2017 Wuklab, Purdue University. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <lego/mm.h>
#include <lego/slab.h>
#include <lego/log2.h>
#include <lego/kernel.h>
#include <lego/pgfault.h>
#include <lego/syscalls.h>
#include <lego/ratelimit.h>
#include <lego/comp_processor.h>
#include <asm/io.h>

#include <processor/include/pcache.h>

int pcache_add_rmap(struct pcache_meta *pcm, pte_t *page_table)
{
	struct pcache_rmap *rmap, *pos;
	struct pcache_set *pset;

	rmap = kmalloc(sizeof(*rmap), GFP_KERNEL);
	if (!rmap)
		return -ENOMEM;
	rmap->page_table = page_table;
	rmap->owner = current;

	/*
	 * Use the lock of pcache set to protect
	 * all ways rmap operations:
	 */
	pset = pcache_meta_to_pcache_set(pcm);
	spin_lock(&pset->lock);
	if (likely(list_empty(&pcm->rmap)))
		goto add;

	list_for_each_entry(pos, &pcm->rmap, next)
		BUG_ON(pos->page_table == page_table);

add:
	list_add(&rmap->next, &pcm->rmap);
	atomic_inc(&pcm->mapcount);
	spin_unlock(&pset->lock);
	return 0;
}

static inline void pcache_paronoid_unmap_check(pte_t pte, struct pcache_meta *pcm)
{
	unsigned long pcm_pfn, pgtable_pfn;

	pcm_pfn = pcache_meta_to_pfn(pcm);
	pgtable_pfn = pte_pfn(pte);
	if (unlikely(pcm_pfn != pgtable_pfn)) {
		pr_err("pcm_pfn: %lu, pgtable_pfn: %lu\n",
			pcm_pfn, pgtable_pfn);
		BUG();
	}
}

/**
 * pcache_try_to_unmap
 * @pcm: the pcache to get unmapped
 *
 * Tries to remove all the page table entries which are mapping this
 * pcache, used int the pageout path.
 *
 * Return:
 *	PCACHE_RMAP_SUCCEED	- we succeeded in removing all mappings
 *	PCACHE_RMAP_AGAIN	- we missed a mapping, try again later
 */
enum pcache_rmap_status pcache_try_to_unmap(struct pcache_meta *pcm)
{
	struct pcache_rmap *rmap;
	struct pcache_set *pset;
	pte_t *page_table;
	pte_t pteval;

	pset = pcache_meta_to_pcache_set(pcm);
	spin_lock(&pset->lock);
	list_for_each_entry(rmap, &pcm->rmap, next) {
		page_table = rmap->page_table;
		BUG_ON(!page_table);

		pteval = ptep_get_and_clear(0, page_table);
		atomic_dec(&pcm->mapcount);
		pcache_paronoid_unmap_check(pteval, pcm);

		/* TODO: *right* TLB flush function! */
	}
	spin_unlock(&pset->lock);

	return PCACHE_RMAP_SUCCEED;
}
