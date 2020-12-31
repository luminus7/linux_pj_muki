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
spinlock_t counter_lock;
struct task_struct *writer_thread1, *writer_thread2,*writer_thread3,*writer_thread4;
struct list_head my_list;

#define BILLION 1000000000
unsigned long long calclock(struct timespec *spclock, unsigned long long *total_time, unsigned long long *total_count){
	long temp,temp_n;
	unsigned long long timedelay=0;
        if(spclock[1].tv_nsec >= spclock[0].tv_nsec){
		temp=spclock[1].tv_sec-spclock[0].tv_sec;
                temp_n=spclock[1].tv_nsec - spclock[0].tv_nsec;
		timedelay = BILLION * temp + temp_n;
        }else{
		temp=spclock[1].tv_sec - spclock[0].tv_sec -1;
                temp_n=BILLION + spclock[1].tv_nsec - spclock[0].tv_nsec;
		timedelay = BILLION * temp + temp_n;
        }

	__sync_fetch_and_add(total_time, timedelay);
	__sync_fetch_and_add(total_count, 1);
	return timedelay;
}

unsigned long long insertTime,insertCount,searchTime,searchCount,deleteTime,deleteCount;

struct my_node{
	struct list_head list;
	int data;
};
struct list_head *current_node;

static int writer_function(void *data)
{
	struct timespec spclock[2];
	getnstimeofday(&spclock[0]);
	while(!kthread_should_stop()){
		spin_lock(&counter_lock);
		int i;
		for(i=0;i<25000;i++)
		{
			struct my_node *new=kmalloc(sizeof(struct my_node),GFP_KERNEL);
			new->data=(++j);
			list_add(&new->list,&my_list);
		}
		spin_unlock(&counter_lock);
		msleep(500);
	}
	getnstimeofday(&spclock[1]);
	calclock(spclock,&insertTime,&insertCount);
	do_exit(0);
	
}

static int search_function(void *data)
{
	
	struct timespec spclock[2];
	getnstimeofday(&spclock[0]);
	while(!kthread_should_stop()){
		spin_lock(&counter_lock);
		int i=0;
		for(i=0;i<25000;i++)
		{
			current_node=current_node->next;
		}
		spin_unlock(&counter_lock);
		msleep(500);
	}
	getnstimeofday(&spclock[1]);
	calclock(spclock,&searchTime,&searchCount);
	do_exit(0);
}	

static int delete_function(void* data)
{
	struct list_head *temp;
	struct my_node *node_temp;
	struct timespec spclock[2];
	current_node=current_node->next;
	getnstimeofday(&spclock[0]);
	while(!kthread_should_stop()){
		spin_lock(&counter_lock);
		int i;
		for(i=0;i<25000;i++)
		{
			temp=current_node->next;
			node_temp=list_entry(current_node,struct my_node,list);
			list_del(&node_temp->list);
			kfree(node_temp);
			current_node=temp;

		}
		spin_unlock(&counter_lock);
		msleep(500);
	}
	getnstimeofday(&spclock[1]);
	calclock(spclock,&deleteTime,&deleteCount);
	do_exit(0);
}


int __init hello_module_init(void)
{
	spin_lock_init(&counter_lock);
	INIT_LIST_HEAD(&my_list);
	insertTime=0;insertCount=0;searchTime=0;searchCount=0;deleteTime=0;deleteCount=0;
	long nano_time,temp,temp_n;
	struct timespec ST,ET;
	printk(KERN_EMERG "Hello Simple Module!\n");
	/*insert*/
	writer_thread1 = kthread_run(writer_function,NULL,"writer_function");
	writer_thread2 = kthread_run(writer_function,NULL,"writer_function");
	writer_thread3 = kthread_run(writer_function,NULL,"writer_function");
	writer_thread4 = kthread_run(writer_function,NULL,"writer_function");
	kthread_stop(writer_thread1);
	kthread_stop(writer_thread2);
	kthread_stop(writer_thread3);
	kthread_stop(writer_thread4);
	
	
	/*search*/
	current_node=&my_list;
	writer_thread1 = kthread_run(search_function,NULL,"search_function");
	writer_thread2 = kthread_run(search_function,NULL,"search_function");
	writer_thread3 = kthread_run(search_function,NULL,"search_function");
	writer_thread4 = kthread_run(search_function,NULL,"search_function");
	kthread_stop(writer_thread1);
	kthread_stop(writer_thread2);
	kthread_stop(writer_thread3);
	kthread_stop(writer_thread4);
	/*delete*/
	current_node=&my_list;
	
	writer_thread1 = kthread_run(delete_function,NULL,"delete_function");
	writer_thread2 = kthread_run(delete_function,NULL,"delete_function");
	writer_thread3 = kthread_run(delete_function,NULL,"delete_function");
	writer_thread4 = kthread_run(delete_function,NULL,"delete_function");
	kthread_stop(writer_thread1);
	kthread_stop(writer_thread2);
	kthread_stop(writer_thread3);
	kthread_stop(writer_thread4);
	
	msleep(2000);
	printk("Spinlock linked list insert time : %lluns | count : %llu\n",insertTime,insertCount);
	printk("Spinlock linked list search time : %lluns | count : %llu\n",searchTime,searchCount);
	printk("Spinlock linked list delete time : %lluns | count : %llu\n",deleteTime,deleteCount);
	return 0;
}

void __exit hello_module_cleanup(void)
{
	printk("Bye Module!\n");
}

module_init(hello_module_init);
module_exit(hello_module_cleanup);

MODULE_LICENSE("GPL");
