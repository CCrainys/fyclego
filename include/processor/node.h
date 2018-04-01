/*
 * Copyright (c) 2016-2018 Wuklab, Purdue University. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

/*
 * This file defines all the node related helpers.
 * You should always should these functions to get corresponding node id.
 */

#ifndef _LEGO_PROCESSOR_NODE_H_
#define _LEGO_PROCESSOR_NODE_H_

#include <lego/sched.h>

#ifdef CONFIG_COMP_PROCESSOR

static inline int get_memory_home_node(struct task_struct *tsk)
{
	return tsk->pm_data.home_node;
}

static inline void set_memory_home_node(struct task_struct *tsk, int node)
{
	tsk->pm_data.home_node = node;
}

static inline int get_replica_node(struct task_struct *tsk)
{
	return tsk->pm_data.replica_node;
}

static inline void set_replica_node(struct task_struct *tsk, int node)
{
	tsk->pm_data.replica_node = node;
}

static inline int current_memory_home_node(void)
{
	return get_memory_home_node(current);
}

static inline int current_replica_node(void)
{
	return get_replica_node(current);
}

/* TODO */
static inline int current_vnode_id(void)
{
	return 0;
}

#ifdef CONFIG_GSM
int get_info_from_gsm(int my_vnode_id);

static inline int get_pgcache_home_node(struct task_struct *tsk)
{
	return tsk->pm_data.pgcache_node;
}

static inline void set_pgcache_home_node(struct task_struct *tsk, int node)
{
	tsk->pm_data.pgcache_node = node;
}

static inline int get_storage_home_node(struct task_struct *tsk)
{
	return tsk->pm_data.storage_node;
}

static inline void set_storage_home_node(struct task_struct *tsk, int node)
{
	tsk->pm_data.storage_node = node;
}

static inline int current_pgcache_home_node(void)
{
	int pgcache_node;
retry:
	pgcache_node = get_pgcache_home_node(current);

	if (unlikely(pgcache_node == UNSET_PGCACHE_NODE)) {
		get_info_from_gsm(current_vnode_id());
		goto retry;
	}
	return pgcache_node;
}

static inline int current_storage_home_node(void)
{
	int storage_node;
retry:
	storage_node = get_storage_home_node(current);

	if (unlikely(storage_node == UNSET_STORAGE_NODE)) {
		get_info_from_gsm(current_vnode_id());
		goto retry;
	}
	return storage_node;
}

#else
/*
 * TODO:
 *
 * Temporary pgcache_node. If GSM is not configured, pgcache is too.
 * Should be patched in the next pgcache patch.
 */

static inline int get_pgcache_home_node(struct task_struct *tsk)
{
	return UNSET_PGCACHE_NODE;
}

static inline void set_pgcache_home_node(struct task_struct *tsk, int node)
{
}

static inline int get_storage_home_node(struct task_struct *tsk)
{
	return UNSET_STORAGE_NODE;
}

static inline void set_storage_home_node(struct task_struct *tsk, int node)
{
}

static inline int current_pgcache_home_node(void)
{
	return UNSET_PGCACHE_NODE;
}

static inline int current_storage_home_node(void)
{
	return UNSET_STORAGE_NODE;
}
#endif /* CONFIG_GSM */


#else
/*
 * Not a processor manager.
 * These helpers should never be used.
 */

static inline int get_memory_home_node(struct task_struct *p)
{
	BUG();
}

static inline void set_memory_home_node(struct task_struct *tsk, int node)
{
	BUG();
}

static inline int get_replica_node(struct task_struct *tsk)
{
	BUG();
}

static inline void set_replica_node(struct task_struct *tsk, int node)
{
	BUG();
}

static inline int current_memory_home_node(void)
{
	BUG();
}

static inline int current_replica_node(void)
{
	BUG();
}
#endif /* CONFIG_COMP_PROCESSOR */

#endif /* _LEGO_PROCESSOR_NODE_H_ */