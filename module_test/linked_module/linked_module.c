#include<linux/kernel.h>
#include<linux/module.h>
#include<linux/init.h>
#include<linux/list.h>
#include<linux/slab.h>
#include<linux/time.h>
#include<linux/posix-clock.h>
#include<linux/syscalls.h>
struct my_node{
	struct list_head list;
	int data;
};


void struct_example(int num)
{
	struct list_head my_list;
	/* initialize*/
	INIT_LIST_HEAD(&my_list);
	long nano_time, temp,temp_n;
	struct timespec ST,ET;
	/*list element add */
	int i;
	getnstimeofday(&ST);
	for(i=0;i<num;i++)
	{
		struct my_node *new=kmalloc(sizeof(struct my_node),GFP_KERNEL);
		new->data=i;
		list_add(&new->list,&my_list);
	}
	getnstimeofday(&ET);
	temp=ET.tv_sec-ST.tv_sec;
	temp_n=ET.tv_nsec-ST.tv_nsec;
	nano_time=1000000000*temp+temp_n;
	printk("Linked List insert %d data Time NANO :%ld\n",num,nano_time); 
	
	/*list search */
	struct my_node *current_node;
	struct list_head *p;
	getnstimeofday(&ST);
	list_for_each(p,&my_list){
		current_node=list_entry(p,struct my_node,list);
	}
	getnstimeofday(&ET);
	temp=ET.tv_sec-ST.tv_sec;
	temp_n=ET.tv_nsec-ST.tv_nsec;
	nano_time=1000000000*temp+temp_n;
	printk("Linked List search %d data Time NANO :%ld\n",num,nano_time); 
	
	/*list delete */
	struct my_node *tmp;
	getnstimeofday(&ST);
	list_for_each_entry_safe(current_node,tmp,&my_list,list)
	{
		if(current_node->data==2){
			list_del(&current_node->list);
			kfree(current_node);
		}
	}
	getnstimeofday(&ET);	
	temp=ET.tv_sec-ST.tv_sec;
	temp_n=ET.tv_nsec-ST.tv_nsec;
	nano_time=1000000000*temp+temp_n;
	printk("Linked DELETE search %d data Time NANO :%ld\n",num,nano_time); 
	

}

int __init hello_module_init(void)
{
	printk("linked module init\n");
	struct_example(1000);
	struct_example(10000);
	struct_example(100000);

	return 0;
}

void __exit hello_module_cleanup(void)
{
	printk("Bye linked  Module\n");
}

module_init(hello_module_init);
module_exit(hello_module_cleanup);


