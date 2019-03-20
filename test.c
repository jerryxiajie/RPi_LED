#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>

char * FILE_NAME = "./w1_slave";
unsigned char file_buffer[20];


float readFile(void)
{
	int i,read_len;
        FILE * file_fd;
        char * FILE_NAME = "sys/bus/w1/devices/28-020592461ab5/w1_slave";
        unsigned char file_buffer[128];
        float temp;

        file_fd = fopen(FILE_NAME,O_RDONLY);
        if(file_fd == NULL)
        {
                printf("File open failed! \n");
                exit(0);
        }
        else
        {
                printf("File open success! \n");
        }

        fseek(file_fd,0L,SEEK_END);
        read_len=ftell(file_fd);
        fseek(file_fd,0L,SEEK_SET);
        fread(file_buffer,read_len,1,file_fd);
        fclose(file_fd);
        file_buffer[read_len]=0;
        i = 0;
        while(file_buffer[i++]  != 't');
        temp = atof(&file_buffer[i+1]);
	printf("temp = %f \n",temp);

	return temp/1000;
}

int main(void)
{
	readFile();	

	return 0;
}
