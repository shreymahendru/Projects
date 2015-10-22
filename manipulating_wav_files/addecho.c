#include <stdio.h>
#include <stdlib.h>
#include<unistd.h>

int main(int argc, char *argv[])
{
	int delay=8000;
	int volume_scale=4;
	int opterr=0;
	int counter=0;
	FILE * input;
	FILE * output;
	
	int c;
	//printf("%d", getopt(argc, argv, "d:v:") );
	
	while((c=getopt(argc, argv, "d:v:"))!=-1)
	{
		
		switch(c)
		{
			case 'd':
					//delay= atoi(optarg);
					printf("del %d",optarg);
					break;
			case 'v':
					volume_scale=atoi(optarg);
					break;
			case '?':
					fprintf(stderr, "Wrong option selected.\n");
					exit(1);
					break;
		}

	}
	printf("d= %d   v= %d", delay,volume_scale);
	if (optind+1!=argc)
	{
		fprintf(stderr,"Wrong number of arguments.\n");
	}
	input= fopen(argv[5-2],"rb");
 	output= fopen(argv[5+1-2],"wb");
// 
// 	if(input==NULL)
// 	{
// 		perror(argv[optind]);
// 		exit(1);
// 	}




	unsigned int header[11];
	int buff;
	buff= fread(header, sizeof(int),11,input);
	//fwrite(&header, 22, 1, output);
	if (buff!=11)
	{
		perror("error");
		exit(1);
	}
	else
	{
	//unsigned int sizeptr[]=(unsigned int[header+2];
	//*sizeptr= *sizeptr + (delay*2);
	//fwrite(header, sizeof(int),buff, output);
		header[1]= header[1]+(delay*2);
		header[10]= header[10]+(delay*2);
		fwrite(header, sizeof(int),buff, output);
	//int i=0; 
	}
	short * echobuff =(short*)malloc(delay * sizeof(short));

	
	int y=0;
  	//while(y<delay)
  	//{	
	fread(echobuff, sizeof(short),delay, input);
	fwrite( echobuff, sizeof(short),delay, output);
	//y++;
	//}
	int i;
	int x;
	/*for(x=0; x< delay; x++)
	{
		printf("%d\n", echobuff[x]);
	}*/
	
	for(i=0;i <delay; i++)
	{
		echobuff[i]= echobuff[i]/volume_scale;
	}
	int k=0;
	short temp;
	short temp2;
	int num_of_samples=0;
	while(fread(&temp, sizeof(short), 1, input)>0)
	{
		num_of_samples++;
		if (k < delay)
		{
			//printf("%d  ", echobuff[k] );
			temp2= temp+ echobuff[k];
			//printf("%d\n",temp2 );
			echobuff[k]= temp/volume_scale; 
			fwrite(&temp2, sizeof(short), 1, output);
			k++;
		}
		else if(k >= delay)
		{			
			k=0;
		}	
	}

	int zeros= delay-num_of_samples;
	
	if (zeros>0)
	{
		int l;
		short zero=0;
		for(l=1;l<=zeros; l++)
		{
			fwrite(&zero, sizeof(short), 1, output);
		}
	}
	int z;
	for (z=0;z<=k;z++)
	{
		fwrite(&echobuff, sizeof(short),1 , output);
	}





	



















	return 0;
}