#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <time.h>
#include <signal.h>

#define MAX_LINE_SIZE 1024

//Global variables for signal handlers
volatile sig_atomic_t shutdown_flag = 0;
volatile sig_atomic_t stats_flag = 0;

void handle_sigint(int sig) 
{
	shutdown_flag = 1;
}

void handle_sigusr1(int sig) 
{
        stats_flag = 1;
}

//Main
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

	//Installing signal handlers
	struct sigaction sa;
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);
	sa.sa_handler = handle_sigint;
	sigaction(SIGINT, &sa, NULL);
	sa.sa_handler = handle_sigusr1;
	sigaction(SIGUSR1, &sa, NULL);

	//Read from stdin line by line
	char line[MAX_LINE_SIZE];
	int line_count = 0;
	int char_count = 0;

	while (!shutdown_flag && fgets(line, sizeof(line), stdin) != NULL)
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

		if(stats_flag)
		{
			fprintf(stderr, "Consumer stats: lines=%d chars=%d\n", line_count, char_count);
			stats_flag = 0;
		}
	}
	//Print statistics to stderr
	fprintf(stderr, "Consumer exiting: lines=%d chars=%d\n", line_count, char_count);
	return 0;
}
