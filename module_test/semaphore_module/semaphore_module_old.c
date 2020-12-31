#include<linux/kernel.h>
#include<linux/module.h>
#include<linux/init.h>
#include<linux/slab.h>
#include<linux/spinlock.h>
#include<linux/kthread.h>
#include<linux/sched.h>
#include<linux/delay.h>
#include<linux/time.h>
#include<linux/list.h>
int j=0;
struct rw_semaphore counter_rwse;
struct task_struct *writer_thread1, *writer_thread2,*writer_thread3,*writer_thread4;
struct list_head my_list;

struct my_node{
	struct list_head list;
	int data;
};
struct list_head *current_node;

static int writer_function(void *data)
{
	while(!kthread_should_stop()){
		down_write(&counter_rwse);
		int i;
		for(i=0;i<2500;i++)
		{
			struct my_node *new=kmalloc(sizeof(struct my_node),GFP_KERNEL);
			new->data=(++j);
			list_add(&new->list,&my_list);
		}
		up_write(&counter_rwse);
		msleep(500);
	}
	do_exit(0);
	
}

static int search_function(void *data)
{
	
	while(!kthread_should_stop()){
		down_read(&counter_rwse);
		int i=0;
		for(i=0;i<2500;i++)
		{
			current_node=current_node->next;
		}
		up_read(&counter_rwse);
		msleep(500);
	}
	do_exit(0);
}	
static int delete_function(void* data)
{
	struct list_head *temp;
	struct my_node *node_temp;
	current_node=current_node->next;
	while(!kthread_should_stop()){
		down_write(&counter_rwse);
		int i;
		for(i=0;i<5;i++)
		{
			temp=current_node->next;
			node_temp=list_entry(current_node,struct my_node,list);
			list_del(&node_temp->list);
			kfree(node_temp);
			current_node=temp;

		}
		up_write(&counter_rwse);
		msleep(500);
	}
	do_exit(0);
}


int __init hello_module_init(void)
{
	init_rwsem(&counter_rwse);
	INIT_LIST_HEAD(&my_list);
	long nano_time,temp,temp_n;
	struct timespec ST,ET;
	printk(KERN_EMERG "Hello Simple Module!\n");
	/*insert*/
	getnstimeofday(&ST);
	writer_thread1 = kthread_run(writer_function,NULL,"writer_function");
	writer_thread2 = kthread_run(writer_function,NULL,"writer_function");
	writer_thread3 = kthread_run(writer_function,NULL,"writer_function");
	writer_thread4 = kthread_run(writer_function,NULL,"writer_function");
	getnstimeofday(&ET);
	temp=ET.tv_sec-ST.tv_sec;
	temp_n=ET.tv_nsec-ST.tv_nsec;
	nano_time=1000000000*temp+temp_n;
	printk("RW Semaphore linked list insert time : %ldns\n",nano_time);
	kthread_stop(writer_thread1);
	kthread_stop(writer_thread2);
	kthread_stop(writer_thread3);
	kthread_stop(writer_thread4);
	
	
	/*search*/
	current_node=&my_list;
	getnstimeofday(&ST);
	writer_thread1 = kthread_run(search_function,NULL,"search_function");
	writer_thread2 = kthread_run(search_function,NULL,"search_function");
	writer_thread3 = kthread_run(search_function,NULL,"search_function");
	writer_thread4 = kthread_run(search_function,NULL,"search_function");
	getnstimeofday(&ET);
	temp=ET.tv_sec-ST.tv_sec;
	temp_n=ET.tv_nsec-ST.tv_nsec;
	nano_time=1000000000*temp+temp_n;
	printk("RW Semaphore linked list search time : %ldns\n",nano_time);
	kthread_stop(writer_thread1);
	kthread_stop(writer_thread2);
	kthread_stop(writer_thread3);
	kthread_stop(writer_thread4);
	/*delete*/
	current_node=&my_list;
	getnstimeofday(&ST);
	getnstimeofday(&ST);
	writer_thread1 = kthread_run(delete_function,NULL,"delete_function");
	writer_thread2 = kthread_run(delete_function,NULL,"delete_function");
	writer_thread3 = kthread_run(delete_function,NULL,"delete_function");
	writer_thread4 = kthread_run(delete_function,NULL,"delete_function");
	getnstimeofday(&ET);
	temp=ET.tv_sec-ST.tv_sec;
	temp_n=ET.tv_nsec-ST.tv_nsec;
	nano_time=1000000000*temp+temp_n;
	printk("RW Semaphore linked list delete time : %ldns\n",nano_time);
	kthread_stop(writer_thread1);
	kthread_stop(writer_thread2);
	kthread_stop(writer_thread3);
	kthread_stop(writer_thread4);
	
	return 0;
}

void __exit hello_module_cleanup(void)
{
	printk("Bye Module!\n");
}

module_init(hello_module_init);
module_exit(hello_module_cleanup);

MODULE_LICENSE("GPL");
