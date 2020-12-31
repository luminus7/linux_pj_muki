#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Lock_Free_BST.h"

typedef struct Vector{
        LF_BST_Node** data;
        int size;
        int count;
}vector;

void vector_init(vector *v)
{
        v->data = NULL;
        v->size = 0;
        v->count = 0;
}

void vector_clear(vector *v)
{
        free(v->data);
}

void vector_push_back(vector* v,LF_BST_Node* node)
{
        if(v->size == 0){
                v->size = 10;
                v->data = malloc(sizeof(LF_BST_Node*) * v->size);
                memset(v->data,'\0',sizeof(LF_BST_Node *) * v->size);
        }
        if(v->size == v->count) {
                v->size *=2;
                v->data = realloc(v->data,sizeof(LF_BST_Node*) * v->size);
        }
        v->data[v->count]=node;
        v->count++;
}

void vector_erase(vector *v ,LF_BST_Node* node)
{
        /*if(index >= v->count){
                printf("범위 내의 원소를 지워야 합니다.\n");
                return ;
        }*/

        int cnt=0;
        bool Isfind=false;
        for(int i=0;i<v->count;i++)
        {
                if(v->data[i]==node)
                {
                        Isfind=true;
                        cnt=i;
                }
        }

        if(Isfind)
        {

                v->data[cnt]=NULL;

                int i=0,j=0;
                LF_BST_Node **newarr = (LF_BST_Node**)malloc(sizeof(LF_BST_Node*) *v->count*2);
                for(;i<v->count;i++)
                {
                        if(v->data[i] != NULL){
                                newarr[j]=v->data[i];
                                j++;
                        }
                }
                free(v->data);
                v->data=newarr;
                v->count--;

        }
        else
                printf("can't find in vector --- failed\n");


}

LF_BST_Node* vector_find(vector *v,LF_BST_Node* node)
{
	printf("-----------------\nSTEP 0 : into the vector find\n");
        for(int i=0;i<v->count;i++)
        {
                if(v->data[i] == node)
                        return v->data[i];
        }
	printf("STEP 1 : can't find\n");
	printf("v->count = %d\n",v->count);
	if(v->count==0)
		return NULL;
        return v->data[v->count-1];
}

void print_vector(vector* v)
{
        for(int i=0;i<v->count;i++)
        {
                printf("%d ---> ",v->data[i]->key);
        }
        printf("\n");
}

