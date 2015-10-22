#include <stdio.h>
#include <stdlib.h>
int main(int argc, char *argv[])
{
	FILE * inputf;
	FILE * outputf;
	char buffer[44];
	short left;
	short right;
	short combined;

	if (argc != 3)
	{
		fprintf(stderr, "Wrong number of arguments.\n");
		exit(1);
	}
	
	else 
	{
		inputf= fopen(argv[1], "r");
		outputf=fopen(argv[2], "w");
	}

	if(inputf == NULL)
	{
		perror(argv[1]);
		exit(1);
	}
	fread(&buffer, 44, 1, inputf);
	fwrite(&buffer, 44, 1, outputf);

	while ((fread (&left, sizeof(short), 1, inputf))>0)
	{
		fread (&right, sizeof(short), 1, inputf);
		combined= ((left-right)/2);
		fwrite(&combined, sizeof(short), 1, outputf);
		fwrite(&combined, sizeof(short), 1, outputf);

	}
	fclose(inputf);
	fclose(outputf);



return 0;

}