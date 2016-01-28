#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include "pagetable.h"


extern int memsize;

extern int debug;

extern struct frame *coremap;

int *ref_bits; // this array would correspond to the memory 
int hand; //hand of the clock

/* Page to evict is chosen using the clock algorithm.
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */

int clock_evict() {
	while(ref_bits[hand] != 0) // checking unset ref bit
	{
		ref_bits[hand] =0; // unseting it cause it passes while looking it a unset bit  
		hand++;
		if (hand == memsize -1) // bring clock hand back to 0
		{
			hand=0;	 
		}
	}
	return hand;
}

/* This function is called on each access to a page to update any information
 * needed by the clock algorithm.
 * Input: The page table entry for the page that is being accessed.
 */
void clock_ref(pgtbl_entry_t *p) {

	ref_bits[p->frame >> PAGE_SHIFT] = 1; // setting page ref to 0 
	return;
}

/* Initialize any data structures needed for this replacement
 * algorithm. 
 */
void clock_init() {
	ref_bits = malloc(memsize * sizeof(int));
	hand =0;
	int i;
	for(i = 0; i < memsize;i++)
	{
		ref_bits[i]=0; 
	} // setting every thing to 0 

}
