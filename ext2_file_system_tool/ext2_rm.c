#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include "ext2.h"
#include <errno.h>

unsigned char *disk;
struct ext2_group_desc *blockgd;
struct ext2_inode *inodeTable;
struct ext2_super_block *sb;

void unset_bit(unsigned int bitmap, int index)
{   
    unsigned char *bitmap_ptr = (disk + EXT2_BLOCK_SIZE * bitmap);

    int i = (index/(sizeof(unsigned char)*8));
    int rem = index%(sizeof(unsigned char)*8);

    bitmap_ptr[i] &= ~(1 << rem);
}


int find_inode(char *dir, int  inode)
{
    int shift = 0;
    int i;
    for(i=0;i<inodeTable[inode].i_blocks/2;i++)
    {
       struct ext2_dir_entry_2 *entry;
       if(i>=12){
            entry = (struct ext2_dir_entry_2 *)(disk + (((unsigned int *)(disk + (inodeTable[inode].i_block[12] -1) * (EXT2_BLOCK_SIZE)))[i-12] -1)*EXT2_BLOCK_SIZE);
       }else{
            entry = (struct ext2_dir_entry_2 *)(disk + inodeTable[inode].i_block[i] * (EXT2_BLOCK_SIZE));
       }

        while (shift < EXT2_BLOCK_SIZE)
        {
            char name[entry->name_len];
            int n;
            for(n= 0; n < entry->name_len; n++)
            {
                name[n]=entry->name[n];
            }
            name[entry->name_len] ='\0';
            if (entry->file_type == 2 && strcmp(dir, name) == 0)
            {
                return entry->inode -1; //  -1 cause position in the inode table
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


int main(int argc, char **argv){
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
    	exit(1);
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

    for (i  = 0; i< length-1; i++)
    {	
    	inode = find_inode(dir[i], inode);
    	if (inode == -1)
    	{
    		fprintf(stderr, "no such directory\n");
    		exit(ENOENT);
    	}
    } 

	struct ext2_inode dirInode = inodeTable[inode];
	int fileInode = -1;

	int shift=0;
	for(i=0;i<dirInode.i_blocks/2;i++){
		if(fileInode!=-1){
			break;
		}

		struct ext2_dir_entry_2 *entry;
       if(i>=12){
            entry = (struct ext2_dir_entry_2 *)(disk + (((unsigned int *)(disk + (inodeTable[inode].i_block[12] -1) * (EXT2_BLOCK_SIZE)))[i-12] -1)*EXT2_BLOCK_SIZE);
       }else{
            entry = (struct ext2_dir_entry_2 *)(disk + inodeTable[inode].i_block[i] * (EXT2_BLOCK_SIZE));
       }

		struct ext2_dir_entry_2 *prev;
        while (shift < EXT2_BLOCK_SIZE)
        {
            char name[entry->name_len];
            int n;
            for(n= 0; n < entry->name_len; n++)
            {
                name[n]=entry->name[n];
            }
            name[entry->name_len] ='\0';
            if (entry->file_type == 1 && strcmp(dir[length - 1], name) == 0)
            {
                fileInode = entry->inode -1; //  -1 cause position in the inode table
                if(prev){
                	prev->rec_len += entry->rec_len;
            	}
                break;
            }
            else 
            {
                shift=shift + entry->rec_len; 
                prev = entry;
                entry = (struct ext2_dir_entry_2 *)(disk + inodeTable[inode].i_block[i] * (EXT2_BLOCK_SIZE)+ shift);
            }
        }
	} 

    if(fileInode!=-1){
    	struct ext2_inode fileInodeStruct = inodeTable[fileInode];
    	int j;
    	for(j=0;j<fileInodeStruct.i_blocks/2;j++){
			int blockID;

    		if(j>=12){
    			blockID = (disk + (fileInodeStruct.i_block[12] -1)*EXT2_BLOCK_SIZE)[j-12] -1;
    		}else{
    			blockID = fileInodeStruct.i_block[j] - 1;
    		}

    		unset_bit(blockgd->bg_block_bitmap, blockID);
    	}

    	unset_bit(blockgd->bg_inode_bitmap, fileInode);
    }else{
    	printf("no such file");
    	exit(ENOENT);
    }

    return 0;

}