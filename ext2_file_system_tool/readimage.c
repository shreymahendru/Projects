#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "ext2.h"

unsigned char *disk;


int main(int argc, char **argv) {

    if(argc != 2) {
        fprintf(stderr, "Usage: readimg <image file name>\n");
        exit(1);
    }
    int fd = open(argv[1], O_RDWR);

    disk = mmap(NULL, 128 * 1024, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(disk == MAP_FAILED) {
	perror("mmap");
	exit(1);
    }

    struct ext2_super_block *sb = (struct ext2_super_block *)(disk + 1024);
    printf("Inodes: %d\n", sb->s_inodes_count);
    printf("Blocks: %d\n", sb->s_blocks_count);
    struct ext2_group_desc *blockgd = (struct ext2_group_desc *)(disk + 1024 + (1024 << sb->s_log_block_size));
    printf("Block group:\n");
    printf("    block bitmap: %d\n", blockgd->bg_block_bitmap);
    printf("    inode bitmap: %d\n", blockgd->bg_inode_bitmap);
    printf("    inode table: %d\n", blockgd->bg_inode_table);
    printf("    free blocks: %d\n", blockgd->bg_free_blocks_count);
    printf("    free inodes: %d\n", blockgd->bg_free_inodes_count);
    printf("    used_dirs: %d\n", blockgd->bg_used_dirs_count);
    printf("block bit map : ");
   
int index = 0;
  
unsigned char bitmap_inode[32];
bitmap_inode[0]=1;
int i;
for(i=0; i < 16; i++) 
{
    unsigned char byte = disk[((1024 << sb->s_log_block_size) * blockgd->bg_block_bitmap)+ i];
int j;
for(j = 0; j < 8; j++) {
    printf("%u",  (byte >> j) & 1);
}}
printf("\n");

printf("Inode bitmap: ");
int x;
for(x=0; x < 4; x++) 
{
    unsigned char byte = disk[((1024 << sb->s_log_block_size) * blockgd->bg_inode_bitmap) + x];
int y ;
for(y = 0; y < 8; y++) {
    int bitz =(byte >> y) & 1;
    bitmap_inode[index]=bitz;
    printf("%u",  bitz);
    index++;
}}
printf("\n");



struct ext2_inode *inodeTable =  (struct ext2_inode *) (disk + (blockgd->bg_inode_table * (1024 << sb->s_log_block_size)));

int dir_inode[32];
int dir_index=0;

//printf(" size %d\n", inodeTable[1].i_size);

for (int k = 0; k <= 32; k++)
{
    if((k > EXT2_ROOT_INO-1 && k < 11) ) //reserved
    {   
       // printf("%i\n", k);
        continue;
    }
    else
    {
        if(k== 0)
        {
            continue;
        }
        if (k == EXT2_ROOT_INO-1 || bitmap_inode[k] == 1  || k == 12)
        {
            //printf("%i\n", bitmap_inode[k]);
            printf("[%i]",k+1);
            unsigned int mode = inodeTable[k].i_mode;
            if (mode & EXT2_S_IFDIR)
            {   
                dir_inode[dir_index] = k;
                dir_index++;
                printf(" type: d");
            }
            else 
            {
                printf(" type: f");
            }
            printf(" sizeof: %d", inodeTable[k].i_size);
            printf(" links: %i", inodeTable[k].i_links_count);
            printf(" blocks: %d\n", inodeTable[k].i_blocks );
            printf("[%i] Blocks",k+1);
            int block_i;
           
            for(block_i = 0; block_i < inodeTable[k].i_blocks-1; block_i++)
            {   
                
                printf(" %u \t",inodeTable[k].i_block[block_i]);
            }
            printf("\n");
        }
    }
}


printf("\n\n");
printf("Directory Blocks:\n");

for (int i = 0; i < dir_index; i++)
{       
    printf("\tDIR BLOCK NUM: %u ", inodeTable[dir_inode[i]].i_block[0]);
    printf(" (for inode %d)\n", dir_inode[i]+1);
    int shift = 0;
    struct ext2_dir_entry_2 *entry = (struct ext2_dir_entry_2 *)(disk + inodeTable[dir_inode[i]].i_block[0] * (1024 << sb->s_log_block_size));
    while (shift < (1024 << sb->s_log_block_size)) 
    {
        printf("Inode: %d ",entry->inode);
        printf("rec_len: %d ",entry->rec_len);
        printf("name_len: %d ",entry->name_len);
        char type;
        if(entry->file_type == 1)
        {
            type= 'f';
        }
        else if(entry->file_type == 2)
        {
            type ='d';
        }
        else
        {
            type= 'u';
        }
        printf("type: %c ",type);
        printf("name=");
        for(int n= 0; n< entry->name_len; n++)
        {
            printf("%c", entry->name[n]);
        }
        printf("\n");
        shift=shift+ entry->rec_len; 
        printf("%d\n", shift);
        entry = (struct ext2_dir_entry_2 *)(disk + inodeTable[dir_inode[i]].i_block[0] * (1024 << sb->s_log_block_size)+ shift);
    }
}








    
    return 0;
}