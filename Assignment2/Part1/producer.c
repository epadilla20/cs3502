#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>

int main(int argc, char *argv[]) 
{
	FILE *input = stdin;
	int buffer_size = 4096;
	char *filename = NULL;
	int opt;

	//Parse commond line arguments
	while((opt = getopt(argc, argv, "f:b:")) != -1)
	{
		switch (opt) 
		{
			case 'f':
				filename = optarg;
				break;
			case 'b':
				buffer_size = atoi(optarg);
				if (buffer_size <= 0) buffer_size =  4096;
				break;
			default:
				fprintf(stderr, "Usage: %s [-f filename] [-b buffer_size]\n", argv[0]);
				return -1;
		}

	}

	//Open file if -f provided
	if (filename != NULL)
	{
		input = fopen(filename, "r");
		if (!input)
		{
			perror("fopen");
			return 1;
		}
	}

	//Allocate buffer
	char *buffer = malloc(buffer_size);
	if (!buffer)
	{
		perror("malloc");
		if (input != stdin) fclose(input);
		return 1;
	}

	//Read from input and write to stdout
	size_t bytes_read;
	while ((bytes_read = fread(buffer, 1, buffer_size, input)) >0)
	{
		fwrite(buffer, 1, bytes_read, stdout);
	}

	//Cleanup
	free(buffer);
	if (input != stdin) fclose(input);
	
	return 0;
}
