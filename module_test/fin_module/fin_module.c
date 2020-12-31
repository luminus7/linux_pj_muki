#define FALSE 0
#define TRUE 1

#include<linux/kernel.h>
#include<linux/module.h>
#include<linux/init.h>
#include<linux/slab.h>
#include<linux/kthread.h>
#include<linux/sched.h>
#include<linux/delay.h>
#include<linux/time.h>
#include<linux/list.h>
#include<linux/rbtree.h>
#include<linux/mutex.h>


struct mutex my_mutex;
struct rb_root my_tree=RB_ROOT;
int key=1;
struct timespec ST,ET;
long nano_time;
struct my_type{
	struct rb_node node;
	int key;
	int value;
};


int rb_insert(struct rb_root *root,struct my_type *data)
{
	struct rb_node **new = &(root->rb_node),*parent = NULL;
	while(*new){
		struct my_type *this=container_of(*new, struct my_type,node);
		parent = *new;
		if(this->key > data->key)
			new = &((*new)->rb_left);
		else if (this->key < data->key)
			new = &((*new)->rb_right);
		else
			return 0;
	}
	rb_link_node(&data->node,parent,new);
	rb_insert_color(&data->node,root);
	return -1;
}



int search_thread(void* _arg)
{
	set_user_nice(current,-20);
	mutex_lock(&my_mutex);
	struct timespec start,end;
	getnstimeofday(&start);
	struct rb_node* node;
	for(node=rb_first(&my_tree);node;node=rb_next(node))
	{
	}
	getnstimeofday(&end);
	nano_time=end.tv_nsec-start.tv_nsec;
	printk("one thread search time : %ldns\n",nano_time);
	mutex_unlock(&my_mutex);
	return 0;
}


int search_thread_left(void* _arg)
{
	set_user_nice(current,-10);
	getnstimeofday(&ST);
	struct rb_node* node;
	for(node=my_tree.rb_node->rb_left;node;node=rb_next(node))
	{
	}
	return 0;
}

int search_thread_right(void* _arg)
{
	set_user_nice(current,-10);
	struct rb_node* node;
	for(node=my_tree.rb_node->rb_right;node;node=rb_next(node))
	{
	}
	getnstimeofday(&ET);
	nano_time=ET.tv_nsec-ST.tv_nsec;
	printk("two thread search time : %ldns\n",nano_time);
	return 0;
}



void struct_example(int num)
{
	long nano_time;
	int i=0, ret;
	
	/* create and insert */
	for(;i<num;i++){
		struct my_type *new=kmalloc(sizeof(struct my_type),GFP_KERNEL);
		if(!new)
			return NULL;
		new->value=i*10;
		new->key=i;
		ret=rb_insert(&my_tree,new);
	}
	printk("RED BLACK TREE insert %d data\n",num);
	/* search */
	kthread_run(&search_thread,NULL,"search_thread");
	kthread_run(&search_thread_left,NULL,"search_thread_left");
	kthread_run(&search_thread_right,NULL,"search_thread_right");
	
}
int __init hello_module_init(void)
{
	struct_example(1000);
	struct_example(10000);
	struct_example(100000);
	struct_example(1000000);
	struct_example(1000000);
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
