#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include "ext2.h"

unsigned char *disk;
struct ext2_group_desc *blockgd;
struct ext2_inode *inodeTable;




int find_inode(char *dir, int  inode)
{
	int shift = 0;
    int i;
    for(i=0;i<inodeTable[inode].i_blocks/2;i++){
	   struct ext2_dir_entry_2 *entry;
       if(i>=12){
            entry = (struct ext2_dir_entry_2 *)(disk + (((unsigned int *)(disk + (inodeTable[inode].i_block[12] -1) * (EXT2_BLOCK_SIZE)))[i-12] -1)*EXT2_BLOCK_SIZE);
       }else{
            entry = (struct ext2_dir_entry_2 *)(disk + inodeTable[inode].i_block[i] * (EXT2_BLOCK_SIZE));
       }
		while (shift < (EXT2_BLOCK_SIZE))
		{
			char name[entry->name_len];
            int n;
			for(n= 0; n< entry->name_len; n++)
        	{
            	name[n]=entry->name[n];
        	}
            name[entry->name_len] ='\0';
			if (entry->file_type == 2 && strcmp(dir, name) == 0)
			{
				return entry->inode -1;
			}
			else 
			{
				shift=shift + entry->rec_len; 
        		entry = (struct ext2_dir_entry_2 *)(disk + inodeTable[inode].i_block[i] * (EXT2_BLOCK_SIZE)+ shift);
			}
		}
    }

    return -1;
}

void ls_on(struct ext2_inode inode){
    int shift = 0;
    int i;
    for(i=0;i<inode.i_blocks/2;i++){
        struct ext2_dir_entry_2 *entry;
       if(i>=12){
            entry = (struct ext2_dir_entry_2 *)(disk + (((unsigned int *)(disk + (inode.i_block[12] -1) * (EXT2_BLOCK_SIZE)))[i-12] -1)*EXT2_BLOCK_SIZE);
       }else{
            entry = (struct ext2_dir_entry_2 *)(disk + inode.i_block[i] * (EXT2_BLOCK_SIZE));
       }
        while (shift < (EXT2_BLOCK_SIZE)){
            int n;
            for(n= 0; n< entry->name_len; n++){
                printf("%c", entry->name[n]);
            }
            printf("\n");
            shift=shift+ entry->rec_len;
            entry = (struct ext2_dir_entry_2 *)(disk + inode.i_block[i] * (EXT2_BLOCK_SIZE)+ shift);
        }    
    }
}

int main(int argc, char **argv) 
{

    if(argc != 3) 
    {
        fprintf(stderr, "Usage: ext2_ls <image file name>  <absolute path on the disk>\n");
        exit(1);
    }
    int fd = open(argv[1], O_RDWR);

    disk = mmap(NULL, 128 * 1024, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(disk == MAP_FAILED) 
    {
		perror("mmap");	
		exit(1);
    }
   
    char *abs_path = argv[2];
    
    if (abs_path[0] != '/')
    {
    	fprintf(stderr,"Not an Absolute Path" );
    }

    char *dir[100];
    int length=0; 
    char *tok = strtok(abs_path, "/"); 
	while (tok != NULL)
    {
        dir[length]= tok;
        length++;
        tok = strtok(NULL, "/");
    }

    blockgd = (struct ext2_group_desc *)(disk + 1024 + (EXT2_BLOCK_SIZE));
    inodeTable =  (struct ext2_inode *)(disk + (blockgd->bg_inode_table * (EXT2_BLOCK_SIZE)));

   
	int inode = 1;	
    int i;
    for (i  = 0; i< length; i++)
    {
    	inode = find_inode(dir[i], inode);
    	if (inode == -1)
    	{
    		fprintf(stderr, "No such file or directory\n");
    		exit(1);
    	}
    	
    } 

    struct ext2_inode dirInode =inodeTable[inode];

    ls_on(dirInode);

    return 0;

}

