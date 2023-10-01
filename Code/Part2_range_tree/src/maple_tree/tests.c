#include "errno.h"
#include "maple_tree.h"
#include "compat_internal.h"

#define pr_cont(fmt, ...) \
	printf(fmt, ##__VA_ARGS__)
#define pr_info(fmt, ...) \
	printf(fmt, ##__VA_ARGS__)
#define pr_err(fmt, ...) \
	printf(fmt, ##__VA_ARGS__)



#define MTREE_ALLOC_MAX 0x2000000000000Ul
#ifndef CONFIG_DEBUG_MAPLE_TREE
#define CONFIG_DEBUG_MAPLE_TREE
#endif
#define CONFIG_MAPLE_SEARCH
#define MAPLE_32BIT (MAPLE_NODE_SLOTS > 31)

#define mt_set_in_rcu(x)
#define mt_zero_nr_tallocated(x)
#define cond_resched()

#define mt_set_non_kernel(x)            do {} while (0)

static int mtree_test_insert_range(struct maple_tree *mt, unsigned long start,
				unsigned long end, void *ptr)
{
	return mtree_insert_range(mt, start, end, ptr, GFP_KERNEL);
}

int mtree_insert_index(struct maple_tree *mt, unsigned long index, gfp_t gfp)
{
	return mtree_insert(mt, index, xa_mk_value(index & LONG_MAX), gfp);
}

static void *mtree_test_load(struct maple_tree *mt, unsigned long index)
{
	return mtree_load(mt, index);
}

static void check_load(struct maple_tree *mt, unsigned long index,
				void *ptr)
{
	void *ret = mtree_test_load(mt, index);

	if (ret != ptr)
		pr_err("Load %lu returned %p expect %p\n", index, ret, ptr);
	MT_BUG_ON(mt, ret != ptr);
}

static void check_index_load(struct maple_tree *mt, unsigned long index)
{
	return check_load(mt, index, xa_mk_value(index & LONG_MAX));
}

static int mtree_test_store_range(struct maple_tree *mt, unsigned long start,
				unsigned long end, void *ptr)
{
	return mtree_store_range(mt, start, end, ptr, GFP_KERNEL);
}

static void check_store_range(struct maple_tree *mt,
		unsigned long start, unsigned long end, void *ptr, int expected)
{
	int ret = -EINVAL;
	unsigned long i;

	ret = mtree_test_store_range(mt, start, end, ptr);
	MT_BUG_ON(mt, ret != expected);

	if (ret)
		return;

	for (i = start; i <= end; i++)
		check_load(mt, i, ptr);
}

static void check_insert_range(struct maple_tree *mt,
		unsigned long start, unsigned long end, void *ptr, int expected)
{
	int ret = -EINVAL;
	unsigned long i;

	ret = mtree_test_insert_range(mt, start, end, ptr);
	MT_BUG_ON(mt, ret != expected);

	if (ret)
		return;

	for (i = start; i <= end; i++)
		check_load(mt, i, ptr);
}

int main() {

	DEFINE_MTREE(tree);	
	MT_BUG_ON(&tree, !mtree_empty(&tree));

	unsigned long i, j;
	int max;
	unsigned long r[] = {
		10, 15,
		20, 25,
		17, 22, /* Overlaps previous range. */
		9, 1000, /* Huge. */
		100, 200,
		45, 168,
		118, 128,
			};

	MT_BUG_ON(&tree, !mtree_empty(&tree));
	check_insert_range(&tree, r[0], r[1], xa_mk_value(r[0]), 0);
	check_insert_range(&tree, r[2], r[3], xa_mk_value(r[2]), 0);
	check_insert_range(&tree, r[4], r[5], xa_mk_value(r[4]), -EEXIST);
	MT_BUG_ON(&tree, !mt_height(&tree));

	/* Store */
	check_store_range(&tree, r[4], r[5], xa_mk_value(r[4]), 0);
	check_store_range(&tree, r[6], r[7], xa_mk_value(r[6]), 0);
	check_store_range(&tree, r[8], r[9], xa_mk_value(r[8]), 0);
	MT_BUG_ON(&tree, !mt_height(&tree));
	mtree_destroy(&tree);
	MT_BUG_ON(&tree, mt_height(&tree));

	//check_seq(&tree, 50, false);
	max=50;
	for (i = 0; i <= max; i++) {
		MT_BUG_ON(&tree, mtree_insert_index(&tree, i, GFP_KERNEL));
		for (j = 0; j <= i; j++)
			check_index_load(&tree, j);

		if (i)
			MT_BUG_ON(&tree, !mt_height(&tree));
		check_load(&tree, i + 1, NULL);
	}


	mt_set_non_kernel(4);
	check_store_range(&tree, 5, 47,  xa_mk_value(47), 0);
	MT_BUG_ON(&tree, !mt_height(&tree));
	mtree_destroy(&tree);

	pr_info("\nTEST 100\n\n");

	max=100;
	for (i = 0; i <= max; i++) {
		printf("%ld ",i);
		MT_BUG_ON(&tree, mtree_insert_range(&tree, i, i, xa_mk_value(i), GFP_KERNEL));
		printf("inserted\n");
		for (j = 0; j <= i; j++)
		{
			printf("%ld, ", j);
			check_index_load(&tree, j);
		}

		if (i)
			MT_BUG_ON(&tree, !mt_height(&tree));
		check_load(&tree, i + 1, NULL);
		printf(" /// %ld\n", i+1);
	}

	pr_info("\nTEST FINISHED\n\n");
}
