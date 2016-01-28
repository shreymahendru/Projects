#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include "pagetable.h"

#define true 1
#define false 0

extern int memsize;

extern int debug;

extern struct frame *coremap;


struct stacknode{
	int frame;
	struct stacknode *next; // the linked list
};
/* Page to evict is chosen using the accurate LRU algorithm.
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */
struct stacknode  *head = NULL;

 

 int contains(struct stacknode *stack, int frame) // checks if the frame is already in the linked list or not
 {
 	while (stack != NULL)
 	{
 		if (stack->frame == frame)
 		{
 			return true;
 		}
 		stack = stack->next;
 	}
 	return false;
 }
 
struct stacknode *moveToBottom(struct stacknode *head, int frame) //moving the frame node to the bottom of the list which mean it was just refferenced 
{
	struct stacknode *curr = head;
	struct stacknode *perv = NULL;
	struct stacknode *end = (struct stacknode *)malloc(sizeof(struct stacknode));
	end->frame= frame;
	end->next = NULL;
	//struct node *move; 
	if(head->next == NULL)
	{
		return head; 
	}
	while(curr){
        if(curr->frame == frame){
            if(curr == head){
                head = curr->next;
            } else {
                perv ->next = curr->next;
            }
            curr = curr->next;
        } else {
            perv = curr;
            curr = curr->next;
        }
    }
    perv->next = end;
    //element->next = NULL;
    return head;
}


int lru_evict() {
	int victim= head->frame; // the top most frame in the linked list would be the least reacent used
	head= head->next;
	return victim;
}

/* This function is called on each access to a page to update any information
 * needed by the lru algorithm.
 * Input: The page table entry for the page that is being accessed.
 */
void lru_ref(pgtbl_entry_t *p) {

	if (head == NULL) // Linked list is not init 
	{
		struct stacknode *newnode = (struct stacknode *)malloc(sizeof(struct stacknode));
		newnode->frame= p->frame >> PAGE_SHIFT;
		newnode->next= NULL;
		head= newnode; // this works
	}
	else  // head is not null it has something in it.
	{
		if(contains(head, p->frame >> PAGE_SHIFT) == false) //check if it has the current refferenced frame
		{
			struct stacknode *node = (struct stacknode *)malloc(sizeof(struct stacknode));
			node->frame = p->frame >> PAGE_SHIFT;
			node->next= NULL;
			//head= node;
			if(head->next == NULL) // if only one element in the list
			{
				head->next= node;
			}
			else
			{
				struct stacknode *temp = head; 
				while (temp->next != NULL)
				{
					temp= temp->next;
				}
				temp->next = node; // traverses to the end and adds it to the end of the list

			}

		}
		else if (contains(head, p->frame >> PAGE_SHIFT) == true) //if in the linked list 
		{
			head = moveToBottom(head, p->frame >> PAGE_SHIFT); // moved to the bottom
		}
	}
	return;
}


/* Initialize any data structures needed for this 
 * replacement algorithm 
 */
void lru_init() {
	head = NULL;
}
