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


int find_free_block()
{

    unsigned char *bitmap;
    bitmap = (disk + EXT2_BLOCK_SIZE*blockgd->bg_block_bitmap);
    int blockBitmap[sb->s_blocks_count];
    printf("Block bitmap: ");
    int k =0;
    int i;
    for(i=0;i<sb->s_blocks_count/8;i++){
        int j;
        for(j=0;j<8;j++){
           // printf("%u",((bitmap[i]>>j)&1));
            blockBitmap[k] = ((bitmap[i]>>j)&1);
            k++;
        }
       // printf(" ");
    }

    //printf("\n");
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

void write_data_to_block(FILE *fd, int inode)
{
    struct stat st;
    if (fd == NULL)
    {
        fprintf(stderr, "can't read file\n" );
        exit(1);
    } 
    
    if ((fstat(fileno(fd), &st) != 0)) {
        fprintf(stderr, "fstat error\n");
        exit(1);
    }   

    if(!S_ISREG(st.st_mode)){
        fprintf(stderr, "not a regular file\n");
        exit(1);
    }

    unsigned int size_of_file = st.st_size;
   // printf("size of file: %d\n",size_of_file);
    
    

    int num_blocks_neeeded = 0;
    if(size_of_file>0){
        if(size_of_file%EXT2_BLOCK_SIZE==0){
            num_blocks_neeeded = (size_of_file/EXT2_BLOCK_SIZE);
        }else{
            num_blocks_neeeded = ((size_of_file/EXT2_BLOCK_SIZE)+1);
        }
    }

   // printf("Num blocks needed: %d\n", num_blocks_neeeded);

    inodeTable[inode].i_size=size_of_file;

    int num_free_blocks= sb->s_free_blocks_count - sb->s_r_blocks_count;
  //  printf(" free block%d\n", num_free_blocks );
    if (num_free_blocks - num_blocks_neeeded < 0)
    {
        fprintf(stderr, "Not enough space\n");
        exit(ENOSPC);
    }
    inodeTable[inode].i_blocks = num_blocks_neeeded * 2; 
    int i;
    for (i = 0; i < num_blocks_neeeded; i++)
    {
        if(i < 12)
        {
            int free_block = find_free_block();
            if(free_block == -1)
            {
                fprintf(stderr, "Not enough space no block found in data\n");
                exit(ENOSPC);   
            }
            inodeTable[inode].i_block[i] = free_block+1;
            void *data = (void *)(disk + ((free_block+1)*EXT2_BLOCK_SIZE));
            int a = fread(data, 1, EXT2_BLOCK_SIZE, fd);
            if (a < 0)
            {
                fprintf(stderr, "Unable to read\n");
            }
            set_bit(blockgd->bg_block_bitmap,free_block);
           // printf("written %d \n", a );
        // add cases here 
        }
        else if (i == 12) // add the block with id of other blocks
        {
            int id_block = find_free_block(); // get the block for Ids of indirect block
            inodeTable[inode].i_block[i] = id_block+1;
            int *other_block = (int *)(disk + (id_block * EXT2_BLOCK_SIZE));
            set_bit(blockgd->bg_inode_bitmap,id_block);
            int free_block = find_free_block(); 
            if(free_block == -1)
            {
                fprintf(stderr, "Not enough space no block found in data\n");
                exit(ENOSPC);   
            }
            other_block[0] = free_block+1;//
            void *data  = (void *)(disk + (free_block+1)*EXT2_BLOCK_SIZE);
            fread(data, 1, EXT2_BLOCK_SIZE, fd);
            set_bit(blockgd->bg_inode_bitmap,free_block);
            set_bit(blockgd->bg_inode_bitmap,id_block);
        }
        else if(i > 12)
        {
            int free_block = find_free_block();
            if(free_block == -1)
            {
                fprintf(stderr, "Not enough space no block found in data\n");
                exit(ENOSPC);   
            }
            (disk + (inodeTable[inode].i_block[12]-1) * EXT2_BLOCK_SIZE)[i- 12] = free_block+1;
            void *data = (void *)(disk + ((free_block+1)*EXT2_BLOCK_SIZE));
            fread(data, 1, EXT2_BLOCK_SIZE, fd);
            set_bit(blockgd->bg_inode_bitmap,free_block);
        }


    }
    fclose(fd);

}


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

void check_file_already(int inode, char *file_name)
{
    int shift = 0;
    int i;
    for(i= 0; i<inodeTable[inode].i_blocks/2; i++)
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
           // printf("name: %s\n",name);
            printf("why you npw work?%s    %s  %d \n", name, file_name,  strcmp(file_name, name) );
            if (entry->file_type == 1 && strcmp(file_name, name) == 0)
            {
                fprintf(stderr, "File already Exists\n");
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
    //printf("Inode bitmap: ");
    int i;
    for(i=0;i<sb->s_inodes_count/8;i++){
        int j;
        for(j=0;j<8;j++){
          //  printf("%u",((bitmap[i]>>j)&1));
            inodeBitmap[k] = ((bitmap[i]>>j)&1);
            k++;
        }
       // printf(" ");
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

void add_dir_entry(int parent_inode, unsigned int file_inode, char *file_name)
{
    int last_block = (inodeTable[parent_inode].i_blocks/2)-1;
    //printf("last block    %d\n", last_block );
    struct ext2_dir_entry_2 *entry = (struct ext2_dir_entry_2 *)(disk + inodeTable[parent_inode].i_block[last_block] * (EXT2_BLOCK_SIZE));
    //printf("name      %s\n", entry->name );
    //printf("file inode %d\n",file_inode);
    int shift = 0;
   // printf("hree\n");
    //printf("here before loop\n");


    if(last_block < 12)
    {
        while (shift < EXT2_BLOCK_SIZE)
        {
            printf("shift   %d\n", shift);
            shift += entry->rec_len; //1024
            if(shift == EXT2_BLOCK_SIZE)
            {
                shift = shift - entry->rec_len; // 1024 -980 = 44
                break;
            }
            else
            {
                //shift=shift + entry->rec_len; 
                entry = (struct ext2_dir_entry_2 *)(disk + inodeTable[parent_inode].i_block[last_block] * (EXT2_BLOCK_SIZE)+ shift);
            }
        }
       // printf("lopp out \n");
        unsigned short last_rec_len = entry->rec_len; //980
        entry->rec_len = (unsigned short) (sizeof(struct ext2_dir_entry_2) + entry->name_len); // 44 + size + name_len around 50 
        unsigned short  size_of_last = (unsigned short) (sizeof(struct ext2_dir_entry_2) + entry->name_len);
       // unsigned short last= entry->rec_len;
      //  printf("%d\n",entry->rec_len);
        entry = (struct ext2_dir_entry_2 *)(disk + inodeTable[parent_inode].i_block[last_block] * (EXT2_BLOCK_SIZE)+  shift + size_of_last);
        //strcpy(entry->name, file_name);
        unsigned short i =0;
        char c =file_name[i];
        while (c != '.')
        {
            c=file_name[i];
            entry->name[i]= c;
            i++;

        } 

       // entry->name[i]
        
        entry->file_type = 1;
        entry->name_len = (unsigned short) i-1;
        entry->inode= (unsigned int)file_inode;
        entry->rec_len=(unsigned short) (last_rec_len - size_of_last); 
        //memcpy(&newentry, entry, sizeof(newentry));

    }
}




int main(int argc, char **argv) 
{

    if(argc != 4) 
    {
        fprintf(stderr, "Usage: ext2_cp <image file name>  <Native absolute path> <Absolute on the disk image>\n");
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

   
    char *native_path = argv[2];
    char *abs_path = argv[3];
    
    if (abs_path[0] != '/' || native_path[0] != '/')
    {
    	fprintf(stderr,"Not an Absolute Path ");
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
    blockgd = (struct ext2_group_desc *)(disk + 1024 + (EXT2_BLOCK_SIZE)); //block gd
    sb = (struct ext2_super_block *)(disk + 1024); // super block
    inodeTable =  (struct ext2_inode *)(disk + (blockgd->bg_inode_table * (EXT2_BLOCK_SIZE))); // Inode table

    int free_blocks  =  sb->s_free_blocks_count - sb->s_r_blocks_count;
   // printf("free blocks%d\n", free_blocks);
    if (free_blocks <= 0)
    {
        fprintf(stderr, "No free Blocks left\n");
        exit(ENOSPC);
    }
    int free_inodes = sb->s_free_inodes_count;
    
    if (free_inodes <= 0)
    {
        fprintf(stderr, "No free Inodes left\n");
        exit(ENOSPC);
    }

    int inode = 1; //root node
    if (length != 0)
    {
        int i;
        for(i = 0; i < length; i++)
        {
            inode = find_inode(dir[i], inode);
            if (inode == -1)
            {
                fprintf(stderr, "No such Directory\n");
                exit(1);
            }

        }
    }
    FILE *file = fopen (argv[2], "r");
    char *native[100];
    int lengthn=0; 
    char *token = strtok(native_path, "/"); 
    while (token != NULL)
    {
        native[lengthn]= token;
        lengthn++;
        token = strtok(NULL, "/");
    }
    
    
    int len = 0;
    char fname [strlen(native[lengthn-1]) -3]; // get rid of the extention  
    char c = native[lengthn-1][len];
    while(c != '.')
    {
        c = native[lengthn-1][len];
        fname[len] = c;
        len++;
    }
    fname[len-1] = '\0';

    check_file_already(inode, fname);

    unsigned int free_inode = get_free_inode();
    if(free_inode == -1)
    {
        fprintf(stderr, "no inode found\n");
        exit(EEXIST);
    }

    struct ext2_inode *newinode = (struct ext2_inode *)(disk + (blockgd->bg_inode_table * (EXT2_BLOCK_SIZE)) + free_inode * sizeof(struct ext2_inode));
    sb->s_free_inodes_count -= 1;
    newinode->i_mode = (unsigned short)32768;
    newinode->i_links_count = (unsigned short)1;
    //printf("%s\n",argv[2]);
    struct stat st;
    
    if (file == NULL)
    {
        fprintf(stderr,"no such file\n");
        exit (1);
    }
    
    if ((fstat(fileno(file), &st) != 0)) {
        fprintf(stderr, "fstat error\n");
        exit(1);
    }   

    if(!S_ISREG(st.st_mode)){
        fprintf(stderr, "not a regular file\n");
        exit(1);
    }

    unsigned int size_of_file = st.st_size;
   // printf("size of file: %d\n",size_of_file);

    fseek(file, 0L, SEEK_SET);

    newinode->i_size = size_of_file;
    if(size_of_file>0){
        if(size_of_file%EXT2_BLOCK_SIZE==0){
            newinode->i_blocks = (size_of_file/EXT2_BLOCK_SIZE) * 2;
        }else{
            newinode->i_blocks = ((size_of_file/EXT2_BLOCK_SIZE)+1) * 2;
        }
    }

    //newinode->
    printf("file name   %s\n", native [lengthn-1]);
    add_dir_entry(inode, free_inode+1, native[lengthn-1]);


    write_data_to_block(file,free_inode);

    return 0;

}