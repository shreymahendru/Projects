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

int main(int argc, char **argv){
	if(argc != 4) 
    {
        fprintf(stderr, "Usage: ext2_cp <image file name>  <Absolute path on the disk image> <Absolute on the disk image>\n");
        exit(1);
    }
    int fd = open(argv[1], O_RDWR);
    if (!fd)
    {
        fprintf(stderr, "no file\n" );
        exit(1);
    }

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

    char *link_from[100];
    int length=0; 
    char *tok = strtok(abs_path, "/"); 
	while (tok != NULL)
    {
        link_from[length]= tok;
        length++;
        tok = strtok(NULL, "/");
    }

    abs_path = argv[3];

    char *link_to[100];
    int length2=0;
    char *tok2 = strtok(abs_path, "/"); 
	while (tok2 != NULL)
    {
        link_to[length2]= tok2;
        length2++;
        tok2 = strtok(NULL, "/");
    }

    blockgd = (struct ext2_group_desc *)(disk + 1024 + (EXT2_BLOCK_SIZE));
    inodeTable =  (struct ext2_inode *)(disk + (blockgd->bg_inode_table * (EXT2_BLOCK_SIZE)));


    int inode = 1;	
    int i;

    for (i  = 0; i< length-1; i++)
    {	
    	inode = find_inode(link_from[i], inode);
    	if (inode == -1)
    	{
    		fprintf(stderr, "no such directory\n");
    		exit(ENOENT);
    	}
    }

    int inode2 = 1;	

    for (i  = 0; i< length2-1; i++)
    {	
    	inode2 = find_inode(link_to[i], inode2);
    	if (inode2 == -1)
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

        while (shift < EXT2_BLOCK_SIZE)
        {
            char name[entry->name_len];
            int n;
            for(n= 0; n < entry->name_len; n++)
            {
                name[n]=entry->name[n];
            }
            name[entry->name_len] ='\0';
            if (strcmp(link_from[length - 1], name) == 0)
            {
            	if(entry->file_type == 1){
                	fileInode = entry->inode -1; //  -1 cause position in the inode table
                	break;
            	}else{
            		printf("link from is not a file\n");
            		exit(EISDIR);
            	}
            }
            else 
            {
                shift=shift + entry->rec_len; 
                entry = (struct ext2_dir_entry_2 *)(disk + inodeTable[inode].i_block[i] * (EXT2_BLOCK_SIZE)+ shift);
            }
        }
	} 

	dirInode = inodeTable[inode2];
	int file2Inode = -1;

	shift = 0;

	for(i=0;i<dirInode.i_blocks/2;i++){
		if(file2Inode!=-1){
			break;
		}

		struct ext2_dir_entry_2 *entry;
       if(i>=12){
            entry = (struct ext2_dir_entry_2 *)(disk + (((unsigned int *)(disk + (inodeTable[inode2].i_block[12] -1) * (EXT2_BLOCK_SIZE)))[i-12] -1)*EXT2_BLOCK_SIZE);
       }else{
            entry = (struct ext2_dir_entry_2 *)(disk + inodeTable[inode2].i_block[i] * (EXT2_BLOCK_SIZE));
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
            if (strcmp(link_to[length - 1], name) == 0)
            {
            	if(entry->file_type == 1){
                	file2Inode = entry->inode -1; //  -1 cause position in the inode table
                	break;
            	}else{
            		printf("link from is not a file\n");
            		exit(EISDIR);
            	}
            }
            else 
            {
                shift=shift + entry->rec_len; 
                entry = (struct ext2_dir_entry_2 *)(disk + inodeTable[inode2].i_block[i] * (EXT2_BLOCK_SIZE)+ shift);
            }
        }
	} 

	printf("file1: %d\n",fileInode);
	printf("file2: %d\n",file2Inode);

	return 0;
}