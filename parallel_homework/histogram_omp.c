#include <string.h>	

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <omp.h>

#include "tbb/tick_count.h"
using tbb::tick_count;

char* map_file(char *filename, int *length_out) 
{
	struct stat file_stat;
	int fd = open(filename, O_RDONLY);
	if (fd == -1) 
	{
		printf("failed to open file: %s\n", filename); 
		exit(1);
	}
	if (fstat(fd, &file_stat) != 0) 
	{
		printf("failed to stat file: %s\n", filename); 
		exit(1);
	}
	off_t length = file_stat.st_size;
	void *file = mmap(0, length, PROT_WRITE, MAP_PRIVATE, fd, 0);
	if (file == (void *)-1) 
	{
		printf("failed to stat file: %s\n", filename); 
		exit(1);
	}

	*length_out = length;
	return (char *)file;
}

int main(int argc, char *argv[]) 
{
	//#pragma omp parallel num_threads(256)
	//omp_set_num_threads(256);
	
	int length = 0;
	bool print = false;
	if (strncmp(argv[1], "-print", 7) == 0) 
	{
		print = true;
		argv = &argv[1];
	}
	char *file = map_file(argv[1], &length);
	unsigned histogram[256] = {0};

	tick_count start = tick_count::now();

	// Your code here!
	//omp_lock_t lock;
	//omp_init_lock(&lock);
	
	#pragma omp parallel num_threads(256)//not sure why it doesnt run faster
	{
	#pragma omp for
	for(int i=0; i<length; i++){
		//omp_set_lock(&lock);
		//#pragma omp critical //really slows down code but doesnt work correctly unless used	
			histogram[file[i]]++;
		
		//omp_unset_lock(&lock);
			}
	}
		
	//omp_destroy_lock(&lock);

	tick_count end = tick_count::now();
	printf("time = %f seconds\n", (end - start).seconds());  

	if (print) 
	{
		for (int i = 0 ; i < 256 ; i ++) 
		{
			if (histogram[i] != 0) 
			{
				printf("%c (%d): %d\n", i, i, histogram[i]);
			}
		}
	}
}
