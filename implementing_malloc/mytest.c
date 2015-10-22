#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include "smalloc.h"


#define SIZE 4096 * 64

//This is really interesting cause we allocate every thing to the memory and then we try to delete a random address not the one that is not in the list so instead of crashing the the whole program. the crash is protected. this could occur in real time use of malloc. 


int main(void) {

    mem_init(SIZE);
    
    char *ptrs[10];
    //int i;

    
    
    //for(i = 0; i < 4; i++) {
        int num_bytes = SIZE ;//(i+200) * 10;
    
        ptrs[0] = smalloc(num_bytes);
        write_to_mem(num_bytes, ptrs[0], 0);
    //}
    printf("List of allocated blocks:\n");
    print_allocated();
    printf("List of free blocks:\n");
    print_free();
    printf("Contents of allocated memory:\n");
    //print_mem();
    
    printf("freeing %p result = %d\n", ptrs[1], sfree(ptrs[1]));
    
    printf("List of allocated blocks:\n");
    print_allocated();
    printf("List of free blocks:\n");
    print_free();
    printf("Contents of allocated memory:\n");
    //print_mem();

    mem_clean();
    return 0;
}

