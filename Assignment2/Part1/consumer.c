#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <time.h>

#define MAX_LINE_SIZE 1024

int main(int argc, char *argv[]) 
{
	int max_lines = -1;
	int verbose = 0;
	int opt;
	//Parse arguments (-n max_lines, -v verbose)
	while ((opt = getopt(argc, argv, "n:v")) != -1)
	{
		switch (opt)
		{
			case 'n':
				max_lines = atoi(optarg);
				break;
			case 'v':
				verbose = 1;
				break;
			default:
				fprintf(stderr, "Usage: %s [-n max_lines] [-v]\n", argv[0]);
				return 1;
		}
	}
	//Read from stdin line by line
	char line[MAX_LINE_SIZE];
	int line_count = 0;
	int char_count = 0;
	while (fgets(line, sizeof(line), stdin) != NULL)
	{
		line_count++;
		char_count += strlen(line);
		if(verbose)
		{
			fputs(line, stdout);
			fflush(stdout);
		}
		if(max_lines != -1 && line_count >= max_lines)
		{
			break;
		}
	}
	//Print statistics to stderr
	fprintf(stderr, "lines: %d\n", line_count);
	fprintf(stderr, "Characters: %d\n", char_count);
	return 0;
}
