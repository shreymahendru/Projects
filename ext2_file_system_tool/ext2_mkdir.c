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

void set_bit(unsigned int bitmap, int index)
{   
    unsigned char *bitmap_ptr = (disk + EXT2_BLOCK_SIZE * bitmap);

    int i = (index/(sizeof(unsigned char)*8));
    int rem = index%(sizeof(unsigned char)*8);

    bitmap_ptr[i] |= 1 << rem;
}


int find_inode(char *dir, int  inode)
{
    int shift = 0;
    int i;
    for(i=0;i<inodeTable[inode].i_blocks/2;i++)
    {
        struct ext2_dir_entry_2 *entry = (struct ext2_dir_entry_2 *)(disk + (inodeTable[inode].i_block[i]-1) * (EXT2_BLOCK_SIZE));
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



void check_already_exist(int inode, char *dir_name)
{
    int shift = 0;
    int i;
    for(i= 0; i<inodeTable[inode].i_blocks/2; i++)
    {
        struct ext2_dir_entry_2 *entry = (struct ext2_dir_entry_2 *)(disk + inodeTable[inode].i_block[i] * (EXT2_BLOCK_SIZE));
        while (shift < EXT2_BLOCK_SIZE)
        {
            char name[entry->name_len];
            int n;
            for(n= 0; n < entry->name_len; n++)
            {
                name[n]=entry->name[n];
            }
            name[entry->name_len] ='\0';
          
            if (entry->file_type == 2 && strcmp(dir_name, name) == 0)
            {
                fprintf(stderr, "Directory already Exists\n");
                exit(EEXIST); //  -1 cause position in the inode table
            }
            else 
            {
                shift=shift + entry->rec_len; 
                entry = (struct ext2_dir_entry_2 *)(disk + inodeTable[inode].i_block[i] * (EXT2_BLOCK_SIZE)+ shift);
            }
        }
    }
}

int get_free_inode()
{
    unsigned char *bitmap;
    bitmap = (disk + EXT2_BLOCK_SIZE*blockgd->bg_inode_bitmap);
    int inodeBitmap[sb->s_inodes_count];
    int k = 0;
    int i;
    for(i=0;i<sb->s_inodes_count/8;i++){
        int j;
        for(j=0;j<8;j++){
          //  printf("%u",((bitmap[i]>>j)&1));
            inodeBitmap[k] = ((bitmap[i]>>j)&1);
            k++;
        }
    }
    
    unsigned int l;
    for(l =11; l < k; l ++)
    {
        if(inodeBitmap[l] == 0)
        {
            set_bit(blockgd->bg_inode_bitmap, l);

            return l;
        }
    }
    return -1;
}


int find_free_block()
{

    unsigned char *bitmap;
    bitmap = (disk + EXT2_BLOCK_SIZE*blockgd->bg_block_bitmap);
    int blockBitmap[sb->s_blocks_count];
    int k =0;
    int i;
    for(i=0;i<sb->s_blocks_count/8;i++){
        int j;
        for(j=0;j<8;j++){
           // printf("%u",((bitmap[i]>>j)&1));
            blockBitmap[k] = ((bitmap[i]>>j)&1);
            k++;
        }
        //printf(" ");
    }

   // printf("\n");
    int l;
    for(l =0; l < k; l++)
    {
        if(blockBitmap[l] == 0)
        {
            set_bit(blockgd->bg_block_bitmap,l);
            return l;
        }
    }
    return -1;  
}



void add_dir_entry(int parent_inode, unsigned int dir_inode, char *dir_name)
{
    int last_block = (inodeTable[parent_inode].i_blocks/2)-1;
    struct ext2_dir_entry_2 *entry = (struct ext2_dir_entry_2 *)(disk + inodeTable[parent_inode].i_block[last_block] * (EXT2_BLOCK_SIZE));
    int shift = 0;
	if(last_block < 12)
    {
        while (shift < EXT2_BLOCK_SIZE)
        {
            shift += entry->rec_len; 
            if(shift == EXT2_BLOCK_SIZE)
            {
                shift = shift - entry->rec_len; 
                break;
            }
            else
            {
                entry = (struct ext2_dir_entry_2 *)(disk + inodeTable[parent_inode].i_block[last_block] * (EXT2_BLOCK_SIZE)+ shift);
            }
        }
        unsigned short last_rec_len = entry->rec_len; 
        entry->rec_len = (unsigned short) (sizeof(struct ext2_dir_entry_2) + entry->name_len); 
        unsigned short  size_of_last = (unsigned short) (sizeof(struct ext2_dir_entry_2) + entry->name_len);
        entry = (struct ext2_dir_entry_2 *)(disk + inodeTable[parent_inode].i_block[last_block] * (EXT2_BLOCK_SIZE)+  shift + size_of_last);
        unsigned short i =0;
        char c =dir_name[i];
        while (i <= strlen(dir_name))
        {
            c=dir_name[i];
            entry->name[i]= c;
            i++;

        } 
        entry->file_type= 2;

        entry->name_len = (unsigned short) i-1;
        entry->inode= (unsigned int)dir_inode;
        entry->rec_len=(unsigned short) (last_rec_len - size_of_last); 
       // printf("rec len %d\n", entry->rec_len );
    }
}




int main(int argc, char **argv) 
{
    if(argc != 3) 
    {
        fprintf(stderr, "Usage: ext2_mkdir <image file name> <Absolute on the disk image>\n");
        exit(1);
    }
    
    int fd = open(argv[1], O_RDWR);
    if (fd < 0)
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

    char *path = argv[2];
    if (path[0] != '/')
    {
    	fprintf(stderr,"Not an Absolute Path ");
        exit(1);
    }

    char *dir[100];
    int length=0; 
    char *tok = strtok(path, "/"); 
	while (tok != NULL)
    {
        dir[length]= tok;
        length++;
        tok = strtok(NULL, "/");
    }
    
    char new_dir_name[strlen(dir[length-1])];  //get name of the new dir
    strcpy(new_dir_name, dir[length-1]);

    blockgd = (struct ext2_group_desc *)(disk + 1024 + (EXT2_BLOCK_SIZE)); //block gd
    sb = (struct ext2_super_block *)(disk + 1024); // super block
    inodeTable =  (struct ext2_inode *)(disk + (blockgd->bg_inode_table * (EXT2_BLOCK_SIZE))); // Inode table

    int free_blocks  =  sb->s_free_blocks_count - sb->s_r_blocks_count;
    if (free_blocks <= 0)
    {
        fprintf(stderr, "No free Blocks left\n");  //check free blocks
        exit(ENOSPC);
    }
    
    int free_inodes = sb->s_free_inodes_count;
    if (free_inodes <= 0)
    {
        fprintf(stderr, "No free Inodes left\n"); //check free inodes
        exit(ENOSPC);
    }

    int inode = 1; 
    if (length != 0)
    {
        int i;
        for(i = 0; i < length-1 ; i++)
        {
            inode = find_inode(dir[i], inode);
            if (inode == -1)
            {
                fprintf(stderr, "No such Directory\n");
                exit(1);
            }

        }
    }
    
    check_already_exist(inode, new_dir_name);

    unsigned int free_inode = get_free_inode();
    if(free_inode == -1)
    {
        fprintf(stderr, "no inode found\n");
        exit(EEXIST);
    }

    struct ext2_inode *newinode = (struct ext2_inode *)(disk + (blockgd->bg_inode_table * (EXT2_BLOCK_SIZE)) + free_inode * sizeof(struct ext2_inode));
    sb->s_free_inodes_count -= 1;
    newinode->i_mode = (unsigned short)16384; //setting the mask to Dir
    newinode->i_links_count= (unsigned int)2; // cause . and .. dir entries
    newinode->i_blocks =(unsigned int) 2;
    int free_block = find_free_block();
    if (free_block == -1)
    {
    	fprintf(stderr, "Not enough space\n");
        exit(ENOSPC);   
    }
    newinode->i_block[0]=free_block+1;
    add_dir_entry(inode, free_inode+1, new_dir_name);
   
    
    struct ext2_dir_entry_2 *self =(struct ext2_dir_entry_2 *)(disk + (free_block+1) * EXT2_BLOCK_SIZE);
    self->inode=(unsigned int)(free_inode+1);
    self->file_type = 2;
    self->name_len = (unsigned short)1;
    self->name[0]= '.'; 
    self->rec_len = (unsigned short)sizeof(struct ext2_dir_entry_2) + self->name_len;

    struct ext2_dir_entry_2 *doubleD =(struct ext2_dir_entry_2 *)(disk + (free_block+1) * EXT2_BLOCK_SIZE + self->rec_len );
    doubleD->inode=(unsigned int)inode+1;
    doubleD->name_len = (unsigned short)2;
    doubleD->file_type = 2;
    doubleD->name[0]= '.';
    doubleD->name[1]='.'; 
    doubleD->rec_len = (unsigned short)EXT2_BLOCK_SIZE - self->rec_len;



    //Change Link count for parent directory
    struct ext2_inode *parentDir =(struct ext2_inode *)(disk + (blockgd->bg_inode_table * (EXT2_BLOCK_SIZE)) + inode * sizeof(struct ext2_inode));
    parentDir->i_links_count += 1;
	
	return 0;

}










