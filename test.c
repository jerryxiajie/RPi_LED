#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>

char * FILE_NAME = "./w1_slave";
unsigned char file_buffer[20];


void readFile(char* basePath)
{
	int i,j,read_len,data;
	FILE * file_fd;
	float temp;

	system("cat /sys/bus/w1/devices/28-020592461ab5/w1_slave > w1_slave");

	file_fd = fopen(basePath,"r");
	if(file_fd == NULL)
	{
		exit(0);
	}
	else
	{
		printf("File open success! \n");
	}

	fseek(file_fd, -6 , SEEK_END);
	
	read_len = fread(file_buffer,1,5,file_fd);
	
	if(read_len == -1)
	{
		printf("File Read Error! \n");
		exit(0);
	}
	else
	{
		printf("File Read Over! \n");

		printf("Read %d Byte From w1_slave.\n",read_len);

		temp=0;
		data=0;

		for(i=0; i<read_len; i++)
		{
			printf("file_buffer[%d] = %c,%d\n", i,file_buffer[i],file_buffer[i]);
			data = file_buffer[i]-48;
			for(j=0; j<4-i; j++)
			{
				data*=10;
			}
			temp+=data;
		}
		temp/=1000;

		printf("temp = %.1f \n", temp);

		printf(" \n");

		fclose(file_fd);
	}


}

int main(void)
{
	readFile(FILE_NAME);	

	return 0;
}
