#include<linux/kernel.h> //included for all kernel modules
#include<linux/module.h> //included for KERN_INFO
#include<linux/init.h>	 //included for __init and __exit macros
#include<linux/kthread.h> //kthread_run()
//#include<linux/slab.h>   //for kmalloc() and kfree()
//#include<linux/delay.h>	//for mdelay() and udelay()

int i=0,j=0;

int foo(void *_arg)
{
	//compare and swap
	//__sync_val_compare_and_swap(&i,i,++i);

	//fetch_and_add
	//__sync_fetch_and_add(&i,1);
	
	//test_and_set
	__sync_lock_test_and_set(&i,++i);

	return 0;
}

int bar(void *_arg)
{
	j++;
	return 0;
}




void thread_create(void)
{
	int argN=23;
	int *arg=&argN; 
	printk("start i: %-8d\t j: %-8d\n",i,j);

	kthread_run(&foo,(void*)arg,"foo1");
	kthread_run(&bar,(void*)arg,"bar1");
	printk("K1 i: %-8d\t j: %-8d\n",i,j);

	kthread_run(&foo,(void*)arg,"foo2");
	kthread_run(&bar,(void*)arg,"bar2");
	printk("K2 i: %-8d\t j: %-8d\n",i,j);

	kthread_run(&foo,(void*)arg,"foo3");
	kthread_run(&bar,(void*)arg,"bar3");
	printk("K3 i: %-8d\t j: %-8d\n",i,j);

	kthread_run(&foo,(void*)arg,"foo4");
	kthread_run(&bar,(void*)arg,"bar4");
	printk("K4 i: %-8d\t j: %-8d\n",i,j);

	printk("final : %-8d\t j: %-8d\n",i,j);

}


int __init hello_module_init(void)
{
	printk(KERN_EMERG "Hello Simple Module!\n");
	thread_create();
	return 0;
}

void __exit hello_module_cleanup(void)
{
	printk("Bye Module!\n");
}

module_init(hello_module_init);
module_exit(hello_module_cleanup);

MODULE_LICENSE("GPL");
