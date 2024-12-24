// SPDX-License-Identifier: Dual BSD/GPL
#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/kernel.h>
#include <linux/ktime.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <linux/errno.h>

MODULE_AUTHOR("Serhii Popovych <serhii.popovych@globallogic.com>");
MODULE_DESCRIPTION("Hello, world in Linux Kernel Training");
MODULE_LICENSE("Dual BSD/GPL");

static uint hello_count = 1; // Параметр за замовчуванням
module_param(hello_count, uint, 0444);
MODULE_PARM_DESC(hello_count, "Number of times to print 'Hello, world!'");

struct hello_item {
	struct list_head list;
	ktime_t time;
};

static LIST_HEAD(hello_list);

static int __init hello_init(void)
{
	int i;

	if (hello_count == 0 || (hello_count >= 5 && hello_count <= 10))
		pr_warn("Parameter hello_count is 0 or between 5 and 10\n");

	if (hello_count > 10) {
		pr_err("Parameter hello_count is greater than 10. Aborting.\n");
		return -EINVAL;
	}

	for (i = 0; i < hello_count; i++) {
		struct hello_item *item;

		item = kmalloc(sizeof(*item), GFP_KERNEL);
		if (!item)
			return -ENOMEM;

		item->time = ktime_get();
		list_add_tail(&item->list, &hello_list);

		pr_emerg("Hello, world!\n");
	}

	return 0;
}

static void __exit hello_exit(void)
{
	struct hello_item *item, *tmp;

	list_for_each_entry_safe(item, tmp, &hello_list, list) {
		pr_info("Time: %lld ns\n", ktime_to_ns(item->time));
		list_del(&item->list);
		kfree(item);
	}
}

module_init(hello_init);
module_exit(hello_exit);
