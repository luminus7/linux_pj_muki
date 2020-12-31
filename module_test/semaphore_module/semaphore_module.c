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
long j=0;
struct rw_semaphore counter_rwse;
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
	long data;
};

static int writer_function(void *data)
{
	struct timespec spclock[2];
	long i;
	for(i=0;i<250;i++)
	{
		struct my_node *new=kmalloc(sizeof(struct my_node),__GFP_NOFAIL);
		getnstimeofday(&spclock[0]);
		down_write(&counter_rwse);
		new->data=(long)__sync_fetch_and_add(&j,1);
		list_add(&new->list,&my_list);
		up_write(&counter_rwse);
		getnstimeofday(&spclock[1]);
		calclock(spclock,&insertTime,&insertCount);
	}
}

static int search_function(void *data)
{
	int threadNum=*(int*)data;	
	struct timespec spclock[2];
	long i=0;
	for(i=threadNum*250;i<250*(threadNum+1);i++){	
		struct list_head* p;
		struct my_node* current_node;
		getnstimeofday(&spclock[0]);
		down_read(&counter_rwse);
		list_for_each(p,&my_list)
		{
			current_node=list_entry(p,struct my_node,list);
			if(current_node -> data == i){
				__sync_fetch_and_add(&j,-1);
				break;
			}
		}
		up_read(&counter_rwse);
		getnstimeofday(&spclock[1]);
		calclock(spclock,&searchTime,&searchCount);
	}
}	

static int delete_function(void* data)
{
	int threadNum=*(int*)data;	
	struct list_head *temp;
	struct my_node *node_temp;
	struct timespec spclock[2];
	struct my_node *current_nodeD;
	long i;
	getnstimeofday(&spclock[0]);

	for(i=threadNum*250;i<250*(threadNum+1);i++){	
		struct list_head* p;struct list_head* temp;
		struct my_node* current_node;
		getnstimeofday(&spclock[0]);
		down_write(&counter_rwse);
		list_for_each_safe(p,temp,&my_list)
		{
			current_node=list_entry(p,struct my_node,list);
			if(current_node->data == i){
				list_del(&current_node->list);
				kfree(current_node);
				j++;
				break;
			}
		}
		up_write(&counter_rwse);
		getnstimeofday(&spclock[1]);
		calclock(spclock,&deleteTime,&deleteCount);
	}
	
}


int __init hello_module_init(void)
{
	init_rwsem(&counter_rwse);
	INIT_LIST_HEAD(&my_list);
	printk(KERN_EMERG "Hello Simple Module!\n");
	/*insert*/
	writer_thread1 = kthread_run(writer_function,NULL,"writer_function");
	writer_thread2 = kthread_run(writer_function,NULL,"writer_function");
	writer_thread3 = kthread_run(writer_function,NULL,"writer_function");
	writer_thread4 = kthread_run(writer_function,NULL,"writer_function");
	
	/*
	kthread_stop(writer_thread1);
	kthread_stop(writer_thread2);
	kthread_stop(writer_thread3);
	kthread_stop(writer_thread4);
	*/
	
	msleep(4000);
	printk("after insert JJJJJJJJJ: %ld\n",j);	
	/*search*/
	int zero=0, one=1,two=2,three=3;	
	writer_thread1 = kthread_run(search_function,&zero,"search_function");
	writer_thread2 = kthread_run(search_function,&one,"search_function");
	writer_thread3 = kthread_run(search_function,&two,"search_function");
	writer_thread4 = kthread_run(search_function,&three,"search_function"); 
/*
	kthread_stop(writer_thread1);
	kthread_stop(writer_thread2);
	kthread_stop(writer_thread3);
	kthread_stop(writer_thread4);
*/
	/*delete*/

	msleep(30000);
	printk("after search JJJJJJJJJ: %ld\n",j);	

	writer_thread1 = kthread_run(delete_function,&zero,"delete_function");
	writer_thread2 = kthread_run(delete_function,&one,"delete_function");
	writer_thread3 = kthread_run(delete_function,&two,"delete_function");
	writer_thread4 = kthread_run(delete_function,&three,"delete_function");
	kthread_stop(writer_thread1);
	kthread_stop(writer_thread2);
	kthread_stop(writer_thread3);
	kthread_stop(writer_thread4);
	
	msleep(60000);
	printk("semaphore lock linked list insert time : %lluns | count : %llu\n",insertTime,insertCount);
	printk("semaphore lock linked list search time : %lluns | count : %llu\n",searchTime,searchCount);
	printk("semaphore lock linked list delete time : %lluns | count : %llu\n",deleteTime,deleteCount);
	return 0;
}

void __exit hello_module_cleanup(void)
{
	printk("Bye Module!\n");
}

module_init(hello_module_init);
module_exit(hello_module_cleanup);

MODULE_LICENSE("GPL");
