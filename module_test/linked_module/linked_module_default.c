#include<linux/kernel.h>
#include<linux/module.h>
#include<linux/init.h>
#include<linux/list.h>
#include<linux/slab.h>

struct my_node{
	struct list_head list;
	int data;
}

void struct_example(int num)
{
	struct list_head my_list;
	/* initialize*/
	INIT_LIST_HEAD(&my_list);
	
	/*list element add */
	int i;
	for(i=0;i<num;i++)
	{
		struct my_node *new=kmalloc(sizeof(struct my_node),GFP_KERNEL);
		new->data=i;
		list_add(&new->list,&my_list);
	}
}

int __init hello_module_init(void)
{
	struct_example(1000);
	printk("linked module init\n");
	return 0;
}

void __exit hello_module_cleanup(void)
{
	printk("Bye linked  Module\n");
}

module_init(hello_module_init);
module_exit(hello_module_cleanup);


