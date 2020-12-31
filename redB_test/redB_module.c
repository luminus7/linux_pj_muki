#include<linux/kernel.h>
#include<linux/module.h>
#include<linux/init.h>
#include<linux/rbtree.h>
#include<linux/slab.h>
#include<linux/time.h>
#include<linux/kthread.h>
#include<linux/delay.h>

#define FALSE 0
#define TRUE 1

struct rw_semaphore rwse;
struct task_struct *thread1;
struct task_struct *thread2;
struct task_struct *thread3;
struct rb_root my_tree=RB_ROOT;

struct my_type{
	struct rb_node node;
	int key;
	int value;
};

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

bool rb_insert(struct rb_root *root,struct my_type *data)
{
	struct timespec clock[2];
	struct rb_node **new = &(root->rb_node),*parent = NULL;
	getnstimeofday(&clock[0]);
	down_write(&rwse);
	while(*new){
		struct my_type *this=container_of(*new, struct my_type,node);
		parent = *new;
		if(this->key > data->key)
			new = &((*new)->rb_left);
		else if (this->key < data->key)
			new = &((*new)->rb_right);
		else
			return FALSE;
	}
	rb_link_node(&data->node,parent,new);
	rb_insert_color(&data->node,root);
	up_write(&rwse);
	getnstimeofday(&clock[1]);
	calclock(clock,&insertTime,&insertCount);
	return TRUE;
}

struct my_type *rb_search(struct rb_root *root,int key)
{
	struct rb_node *node=root->rb_node;
	while(node){
		struct my_type *data=container_of(node,struct my_type,node);
		if(data->key > key)
			node=node->rb_left;
		else if(data->key < key)
			node = node->rb_right;
		else
		{
			return data;
		}
	}
	return NULL;
}

int rb_delete(struct rb_root *mytree,int key)
{
	struct timespec clock[2];
	struct my_type *data=rb_search(mytree,key);
	getnstimeofday(&clock[0]);
	down_write(&rwse);
	if(data){
		rb_erase(&data->node,mytree);
		kfree(data);
	}
	up_write(&rwse);
	getnstimeofday(&clock[1]);
	calclock(clock,&deleteTime,&deleteCount);
}

static int insert_function(void * data)
{
	int threadNum=*(int*)data;
	int i;
	for(i=threadNum*10;i<10*(threadNum+1);i++){	
	
		struct my_type *new=kmalloc(sizeof(struct my_type),GFP_KERNEL);
		new->key=(i);
		rb_insert(&my_tree,new);
	
	}
}

static int search_function(void *data)
{
	int threadNum=*(int*)data;
	int i=0;
	struct timespec clock[0];	
	getnstimeofday(&clock[0]);
	down_read(&rwse);
	for(i=threadNum*10;i<10*(threadNum+1);i++)	
	{
		rb_search(&my_tree,i);
	}
	up_read(&rwse);
	getnstimeofday(&clock[1]);
	calclock(clock,&searchTime,&searchCount);
	
	printk("search complete\n");
}	

static int delete_function(void* data)
{
	int threadNum=*(int*)data;
	int i;
	for(i=threadNum*2500;i<2500*(threadNum+1);i++)	
	{
		rb_delete(&my_tree,i);
	}
	
	printk("delete complete\n");
}


void struct_example(int num)
{
	int i=0, ret;
	int zero=0,one=1,two=2,three=3;	
	
	thread1 = kthread_run(insert_function,&zero,"insert_function");
	msleep(5000);
	thread2 = kthread_run(search_function,&one,"search_function");
	//msleep(5000);
	//thread3 = kthread_run(delete_function,&two,"delete_function");
	

	printk("sema insert time : %lluns | count : %llu\n",insertTime,insertCount);
	printk("sema search time : %lluns | count : %llu\n",searchTime,searchCount);
	printk("sema delete time : %lluns | count : %llu\n",deleteTime,deleteCount);

}
int __init hello_module_init(void)
{
	init_rwsem(&rwse);
	struct_example(100000);
	printk("RED BLACK TREE module init\n");
	return 0;
}
void __exit hello_module_cleanup(void)
{
	printk("Bye Module\n");
}

module_init(hello_module_init);
module_exit(hello_module_cleanup);

MODULE_LICENSE("GPL");
