#include<linux/kernel.h>
#include<linux/module.h>
#include<linux/init.h>
#include<linux/slab.h>
#include<linux/kthread.h>
#include<linux/sched.h>
#include<linux/delay.h>
#include<linux/time.h>
#include<linux/list.h>

int Thread_count=8,last_thread=0;

struct my_node{
	struct list_head list;
	int data;
};


struct list_head my_list;

struct timespec start_test, end_test;

int function_thread(void* _arg)
{
	int*arg=(int*)_arg;
	struct timespec start,end;

	set_user_nice(current,*arg);
	getnstimeofday(&start);
	printk("thread %d: nice prior is : %d\n",*arg,*arg);
	/* function list add */
	int i;
	for(i=0;i<100000;i++)
	{
		struct my_node* new = kmalloc(sizeof(struct my_node),GFP_KERNEL);
		new->data=i;
		list_add(&new->list,&my_list);
	}
	getnstimeofday(&end);
	printk("%d thread execution time: %ld ns\n",*arg,end.tv_nsec-start.tv_nsec);
	/*check total time in last thread*/
	last_thread++;
	
	if(last_thread >= Thread_count){
		getnstimeofday(&end_test);
		printk("total time is %ld\n",end_test.tv_nsec-start_test.tv_nsec);
	}
	kfree(arg);
	return 0;
}
	



void thread_create(void)
{
	/*list&time setting*/
	INIT_LIST_HEAD(&my_list);
	getnstimeofday(&start_test);
	int i;

	/*new thread*/
	for(i=0;i<Thread_count;i++)
	{
		int* arg =(int*)kmalloc(sizeof(int),GFP_KERNEL);
		*arg=i;
		kthread_run(&function_thread,(void*)arg,"function_thread");
	}
}



int __init hello_module_init(void)
{
	thread_create();
	msleep(500);
	printk(KERN_EMERG "Priority Module START!!\n");
	return 0;
}

void __exit hello_module_cleanup(void)
{
	printk("BYE MODULE");
}

module_init(hello_module_init);
module_exit(hello_module_cleanup);

