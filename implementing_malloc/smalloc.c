#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include "smalloc.h"



void *mem;
struct block *freelist;
struct block *allocated_list;

void delete_from_freelist(unsigned int nbytes) //a helper function to delete a particular node from freelist
{
    struct block *cur, *prev;
    for(cur=freelist, prev = NULL; cur != NULL && cur->size !=nbytes; prev= cur, cur= cur->next) //traversing through the list to reach the disired point
        ;
    
    
    if (prev == NULL)// if first nod then do the following 
    {
        freelist=freelist->next;
    }
    else //if not the delete from the list by changing the pointers
    {
        prev->next=cur->next;
        free(cur);
        
    }
    
    
}



void *smalloc(unsigned int nbytes) {
    
    struct block *temp=freelist;
    

    if (temp != NULL)// checking if freelist id not null
    {
        while (temp!= NULL)//while loop to the end
        {

            if (temp->size == nbytes) // if the size being allocated = size of a block in freelist
            {
                struct block *new_node;
                new_node=malloc(sizeof(struct block));
                new_node->addr= temp->addr;
                new_node->size= nbytes;
                new_node->next=allocated_list; //adding the node to the front of the allocated list
                allocated_list=new_node;
                delete_from_freelist(nbytes);//helper function called
                return new_node->addr;
                    
            }

            else if (temp->size > nbytes) //if the size being allocated < size of a block in freelist
            {
            struct block *new_node;
            new_node=malloc(sizeof(struct block));
            new_node->addr= temp->addr;
            new_node->size= nbytes;
            temp->size= (temp->size)-nbytes; // adjusting size 
            temp->addr= (temp->addr) + nbytes;// adjusting the pointer (addr)
            new_node->next=allocated_list; // adding to the allocated list
            allocated_list=new_node;
            return new_node->addr;
        }


        temp=temp->next;
        }
    }


    return NULL;
}
void delete_from_allocated_list(void *addr)// helper function to delete from the allocated list
{
    struct block *cur, *prev;
    for(cur=allocated_list, prev = NULL; cur != NULL && cur->addr != addr; prev= cur, cur= cur->next) //traversing through the list to reach the disired point
        ;
    

        if (prev == NULL)// if first nod then do the following 
        {
                allocated_list=allocated_list->next;
        }
        else
        {
                prev->next=cur->next; //if not the delete from the list by changing the pointers
                free(cur);
           

        }

    
}



int sfree(void *addr){
//   
    struct block *temp=allocated_list;
    
    if(temp != NULL)
    {
    while (temp != NULL)
    {
        
       
        if (temp->addr==addr) { // the address to be deleted id found in the allocated list
           
            struct block *new_node;
            new_node=malloc(sizeof(struct block)); // new node to store the values to add to free list
            new_node->addr=temp->addr;
            new_node->size=temp->size;
            new_node->next=freelist;
            freelist=new_node;
            delete_from_allocated_list(addr);//helper function called to delete from the allocated list
            
            return 0;
        }
    temp=temp->next;
    }
    }
    




    return -1;
}


/* Initialize the memory space used by smalloc,
 * freelist, and allocated_list
 * Note:  mmap is a system call that has a wide variety of uses.  In our
 * case we are using it to allocate a large region of memory. 
 * - mmap returns a pointer to the allocated memory
 * Arguments:
 * - NULL: a suggestion for where to place the memory. We will let the 
 *         system decide where to place the memory.
 * - PROT_READ | PROT_WRITE: we will use the memory for both reading
 *         and writing.
 * - MAP_PRIVATE | MAP_ANON: the memory is just for this process, and 
 *         is not associated with a file.
 * - -1: because this memory is not associated with a file, the file 
 *         descriptor argument is set to -1
 * - 0: only used if the address space is associated with a file.
 */
void mem_init(int size) {
    mem = mmap(NULL, size,  PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
    if(mem == MAP_FAILED) {
         perror("mmap");
         exit(1);
	}
	freelist= malloc(sizeof(struct block)); // creating the first node of freelist
	
    freelist->addr =mem;
	freelist->size=size;
	freelist->next=NULL; 
	


    /* NOTE: this function is incomplete */
        
}

void mem_clean(){
    
    struct block *temp2;
    struct block *temp;
    
    while(freelist!=NULL)//freeing every node of the free list
    {
        temp2=freelist;
        freelist=freelist->next;
        free(temp2);
        
    }
    
    
    while (allocated_list!=NULL) //freeing every node of the allocated list
    {
     
        temp=allocated_list;
        allocated_list=allocated_list->next;
        free(temp);
        
    }
  
    

}

