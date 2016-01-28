#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include "pagetable.h"

#define MAXLINE 256 

extern int memsize;

extern int debug;

extern struct frame *coremap;
extern char* tracefile;

int num_lines_in_ref;

unsigned long *vddr_array;	
int vddr_index=0;

unsigned long *memory;

void get_num_of_vddr(FILE *tfp) // get the total number of vddr to make our array of all the vddr
{
	char buf[MAXLINE];
	addr_t vaddr = 0;
	char type;
	while(fgets(buf, MAXLINE, tfp) != NULL)
	{
		if(buf[0] != '=')
		{
			sscanf(buf, "%c %lx", &type, &vaddr);
			num_lines_in_ref++; 
		} 
	}

}

 

/* Page to evict is chosen using the optimal (aka MIN) algorithm. 
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */
int opt_evict() {
	int i;
	int j;
	int farthest =-1;
	int frame;
	for(i = 0; i < memsize - 1; i++) //checking all vddr in memory now
	{
		for(j = vddr_index; j < num_lines_in_ref; j++)  // and checking if they are called any where else in the reference file 
		{
			if(memory[i] == vddr_array[j]) //if the are refferenced again
			{	
				if(j >= farthest)	// keep it in frathest to check if there are other possiable victims that would be better than the one we have 
				{ // by better I mean that is refferenced later than the current one
					farthest = j; // so this is basicallly finding the farthes vddr
					frame= i;
				}
				break;
			}
		}
		if(j == num_lines_in_ref) // vddr at pos i is not found anywhere in the refference file so that is the one to evict 
		{
			return i;
		}
	}
	return frame;
}

/* This function is called on each access to a page to update any information
 * needed by the opt algorithm.
 * Input: The page table entry for the page that is being accessed.
 */
void opt_ref(pgtbl_entry_t *p) {

	memory[p->frame >> PAGE_SHIFT]= vddr_array[vddr_index];
	vddr_index++;
	return;
}

/* Initializes any data structures needed for this
 * replacement algorithm.
 */
void opt_init() {

	memory = malloc(memsize * sizeof(unsigned long));

	num_lines_in_ref=0;
	FILE *tfp;
	if((tfp = fopen(tracefile, "r")) == NULL)
	{
		perror("Error opening tracefile:");
		exit(1);
	}
	get_num_of_vddr(tfp);
	fclose(tfp);
	vddr_array = malloc( num_lines_in_ref * sizeof(unsigned long)); // array to read all the vddr in the ref file to an dstore there

	if((tfp = fopen(tracefile, "r")) == NULL)
	{
		perror("Error opening tracefile:");
		exit(1);
	}
	
	char buf[MAXLINE];
	addr_t vaddr = 0;
	char type;
	int i= 0;
	while(fgets(buf, MAXLINE, tfp) != NULL) // readl all the vddr from the file and save it in the address to make it easier to look up later 
	{
		if(buf[0] != '=')
		{
			sscanf(buf, "%c %lx", &type, &vaddr);
			vddr_array[i]= vaddr;
			i++;
		} 
	}
	fclose(tfp);


	
}

