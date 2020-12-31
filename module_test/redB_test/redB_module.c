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

struct task_struct *thread1;
struct task_struct *thread2;
struct task_struct *thread3;
struct rb_root my_tree=RB_ROOT;

struct my_type{
	struct rb_node node;
	int key;
	int value;
};


bool rb_insert(struct rb_root *root,struct my_type *data)
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
			return FALSE;
	}
	rb_link_node(&data->node,parent,new);
	rb_insert_color(&data->node,root);
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
			return data;
	}
	return NULL;
}

int rb_delete(struct rb_root *mytree,int key)
{
	struct my_type *data=rb_search(mytree,key);
	if(data){
		rb_erase(&data->node,mytree);
		kfree(data);
	}
}

static int insert_function(void * data)
{
	while(!kthread_should_stop()){
		int i;
		for(i=0;i<2500;i++)
		{
			struct my_type *new=kmalloc(sizeof(struct my_type),GFP_KERNEL);
			new->key=(i);
			rb_insert(&my_tree,new);
		}
		msleep(500);
	}
	printk("insert complete\n");
	do_exit(0);
}

static int search_function(void *data)
{
	
	while(!kthread_should_stop()){
		int i=0;
		for(i=0;i<2500;i++)
		{
			rb_search(&my_tree,i);
		}
		msleep(500);
	}
	printk("search complete\n");
	do_exit(0);
}	

static int delete_function(void* data)
{
	while(!kthread_should_stop()){
		int i;
		for(i=0;i<2500;i++)
		{
			rb_delete(&my_tree,i);
		}
	}
	printk("delete complete\n");
	do_exit(0);
}


void struct_example(int num)
{
	long nano_time,temp,temp_n;
	struct timespec ST,ET;
	int i=0, ret;
	
	thread1 = kthread_run(insert_function,NULL,"insert_function");
	thread2 = kthread_run(search_function,NULL,"search_function");
	thread3 = kthread_run(delete_function,NULL,"delete_function");
	kthread_stop(thread1);
	kthread_stop(thread2);
	kthread_stop(thread3);
	

}
int __init hello_module_init(void)
{
	struct_example(1000);
	struct_example(10000);
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
