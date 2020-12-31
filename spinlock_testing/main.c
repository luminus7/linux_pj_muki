#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include "Lock_Free_BST.h"
#include "Lock_Free_BST.c"
#include "Fine_Grained_BST.h"
#include "Fine_Grained_BST_Lock.c"

#define NODECOUNT 100000
#define MAXTHREAD 144

FG_BST_Node *g_root;
LF_BST_Node* base_root;
bool hazard_pointers;
pthread_spinlock_t tree_lock;
int threadCount;
long time_sec;
struct timespec myclock[2];


void *jobs(void* _arg)
{
	int*arg = (int*)_arg;
	printf("adding node : %d\n",*arg);
	LF_add((int)*arg,(int)*arg);
	return 0;
}
//FG write jobs
void *FG_wjobs(void* _arg)
{
	int unit = NODECOUNT/threadCount;
	int* arg = (int*)_arg;
	int threadNum = *arg;
	for(int j=threadNum*unit+1;j<threadNum*unit+unit+1;j++)
	{
		FG_insert(j,g_root,NULL,(int)*arg);
	}
}

//LF write jobs
void *LF_wjobs(void* _arg)
{
	int unit = NODECOUNT/threadCount;
	int* arg = (int*)_arg;
	int threadNum = *arg;
	
	for(int j=threadNum*unit+1;j<threadNum*unit+unit+1;j++)
	{
		LF_add(j,(int)*arg);
		//printf("[%d,%d] STEP 777| add  node DONEDONEDONE**********************\n",(int)*arg,j);
	}
	
}


//FG read jobs
void *FG_rjobs(void* _arg)
{
	int unit = NODECOUNT/threadCount;
	int* arg = (int*)_arg;
	int threadNum = *arg;
	for(int j=threadNum*unit+1;j<threadNum*unit+unit+1;j++)
	{
		FG_search(j,g_root,NULL);
	}
}

//LF read jobs
void *LF_rjobs(void* _arg)
{
	int unit = NODECOUNT/threadCount;
	int* arg = (int*)_arg;
	int threadNum = *arg;
	LF_BST_Node *pred,*curr;
	void* pred_op, *curr_op;
	for(int j=threadNum*unit+1;j<threadNum*unit+unit+1;j++)
	{
		LF_find(j,&pred,&pred_op,&curr,&curr_op,base_root,(int)*arg);
	}
}


void thread_create(int j,int num)
{
	int status;
	if(j==1)//FG_BST write
	{
		pthread_t curThread[MAXTHREAD];
		clock_gettime(CLOCK_REALTIME,&myclock[0]);
		for(int i=0;i<num;i++)
		{
			int* arg = (int*)malloc(sizeof(int));
			*arg=i;
			pthread_create(&curThread[i],NULL,FG_wjobs,arg);		
		}
		for(int i=0;i<num;i++)
			pthread_join(curThread[i],NULL);
		clock_gettime(CLOCK_REALTIME,&myclock[1]);
	}
	else if(j==2)//LF_BST write
	{
		pthread_t curThread[MAXTHREAD];
		clock_gettime(CLOCK_REALTIME,&myclock[0]);
		for(int i=0;i<num;i++)
		{
			int* arg = (int*)malloc(sizeof(int));
			*arg=i;
			pthread_create(&curThread[i],NULL,LF_wjobs,arg);		
		}
		for(int i=0;i<num;i++)
			pthread_join(curThread[i],NULL);
		clock_gettime(CLOCK_REALTIME,&myclock[1]);
	}
	else if(j==3)//FG_BST read
	{
		pthread_t curThread[MAXTHREAD];
		clock_gettime(CLOCK_REALTIME,&myclock[0]);
		for(int i=0;i<num;i++)
		{
			int* arg = (int*)malloc(sizeof(int));
			*arg=i;
			pthread_create(&curThread[i],NULL,FG_rjobs,arg);		
		}
		for(int i=0;i<num;i++)
			pthread_join(curThread[i],NULL);
		clock_gettime(CLOCK_REALTIME,&myclock[1]);
	}
	else if(j==4)//LF_BST read
	{
		pthread_t curThread[MAXTHREAD];
		clock_gettime(CLOCK_REALTIME,&myclock[0]);
		for(int i=0;i<num;i++)
		{
			int* arg = (int*)malloc(sizeof(int));
			*arg=i;
			pthread_create(&curThread[i],NULL,LF_rjobs,arg);		
		}
		for(int i=0;i<num;i++)
			pthread_join(curThread[i],NULL);
		clock_gettime(CLOCK_REALTIME,&myclock[1]);
	}
}





void InOrderPrintTreeF(FG_BST_Node * node)
{
	if(node == NULL)
		return ;
	InOrderPrintTreeF(node->left);

	printf(" %d -->",node->value);

	InOrderPrintTreeF(node->right);
}


void InOrderPrintTreeL(LF_BST_Node * node)
{
        if(IS_NULL(node))
                return;
        InOrderPrintTreeL(node->left);

        printf(" %d -->",node->key);

        InOrderPrintTreeL(node->right);
}


long calclock(struct timespec *myclock) {
	long timedelay,temp,temp_n;
	if(myclock[1].tv_nsec >=myclock[0].tv_nsec){
		temp=myclock[1].tv_sec - myclock[0].tv_sec;
		temp_n=myclock[1].tv_sec - myclock[0].tv_nsec;
		time_sec = temp;
		timedelay = temp*1000000000+temp_n;

	}else{
		temp=myclock[1].tv_sec - myclock[0].tv_sec -1;
		temp_n= 1000000000 + myclock[1].tv_nsec - myclock[0].tv_nsec;
		time_sec=temp;
		timedelay = temp*1000000000+temp_n;
		       	//1000000000*temp + temp_n;
	}
	return timedelay;
}

long testing(int num)
{
	threadCount=num;
	long timedelay;
	//FG_BST setting
	g_root->left=NULL;
	g_root->right=NULL;
	g_root->value=0;
	//FG_BST writing
	thread_create(1,num);
	//time calculating
	timedelay=calclock(myclock);
	printf("------------writing----------------\n");
	printf("'FG_BST' %d thread 200000 node write time : %10ld nsec | %10ld sec\n",num,timedelay,time_sec);

	//LF_BST setting
	base_root->key = 0;
	base_root->right = (LF_BST_Node *) SET_NULL(NULL);
	base_root->left = (LF_BST_Node *) SET_NULL(NULL);
	base_root->op = NULL;
	
	//LF_BST writing
	thread_create(2,num);
	timedelay=calclock(myclock);	
	printf("'LF_BST' %d thread 200000 node write time : %10ld nsec | %10ld sec\n",num,timedelay,time_sec);


	//printf("************reading****************\n");


	//FG_BST reading
	thread_create(3,num);
	timedelay=calclock(myclock);
	printf("'FG_BST' %d thread 200000 node read time : %10ld nsec | %10ld sec\n",num,timedelay,time_sec);

	//LF_BST reading
	thread_create(4,num);
	timedelay=calclock(myclock);	
	printf("'LF_BST' %d thread 200000 node read time : %10ld nsec | %10ld sec\n",num,timedelay,time_sec);
	
	printf("----------------------------\n");


}
int main(){
	struct timespec myclock[2];
	long timedelay;
	pthread_spin_init(&tree_lock,0);	
	g_root = (FG_BST_Node*)malloc(sizeof(FG_BST_Node));
	base_root = malloc(sizeof(LF_BST_Node));
	pthread_spin_init(&g_root->lock,0);

	printf("HELLO\n");
	//thread 2	
	testing(2);
	//thread 4
	testing(4);
	//thread 8
	testing(8);
	//thread 16
	testing(16);
	//thread 32
	testing(32);
	//thread 64
	testing(64);
	//thread 72
	testing(72);
	free(g_root);
	free(base_root);
	return 0;
}
