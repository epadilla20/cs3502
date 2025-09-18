#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <signal.h>
#include <sys/types.h>
#include <time.h>

// Global variables for signal handlers and performance tracking
volatile sig_atomic_t shutdown_flag = 0;
volatile sig_atomic_t stats_flag = 0;
size_t total_bytes = 0;
clock_t start_time;

void handle_sigint(int sig) 
{
    shutdown_flag = 1;
}

void handle_sigusr1(int sig) 
{
    stats_flag = 1;
}

void display_stats() {
    clock_t end_time = clock();
    double cpu_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
    
    fprintf(stderr, "\n=== PRODUCER STATISTICS ===\n");
    fprintf(stderr, "Bytes processed: %zu\n", total_bytes);
    fprintf(stderr, "Time elapsed: %.3f seconds\n", cpu_time);
    if (cpu_time > 0) {
        fprintf(stderr, "Bytes per second: %.2f\n", total_bytes / cpu_time);
        fprintf(stderr, "MB per second: %.6f\n", (total_bytes / 1024.0 / 1024.0) / cpu_time);
    }
    fprintf(stderr, "============================\n");
    fflush(stderr);
}

int main(int argc, char *argv[]) 
{
    FILE *input = stdin;
    int buffer_size = 4096;
    char *filename = NULL;
    int opt;
    
    // Start timing
    start_time = clock();
    
    // Parse command line arguments
    while((opt = getopt(argc, argv, "f:b:")) != -1)
    {
        switch (opt) 
        {
            case 'f':
                filename = optarg;
                break;
            case 'b':
                buffer_size = atoi(optarg);
                if (buffer_size <= 0) buffer_size = 4096;
                break;
            default:
                fprintf(stderr, "Usage: %s [-f filename] [-b buffer_size]\n", argv[0]);
                return -1;
        }
    }
    
    // Open file if -f provided
    if (filename != NULL)
    {
        input = fopen(filename, "r");
        if (!input)
        {
            perror("fopen");
            return 1;
        }
    }
    
    // Allocate buffer
    char *buffer = malloc(buffer_size);
    if (!buffer)
    {
        perror("malloc");
        if (input != stdin) fclose(input);
        return 1;
    }
    
    // Installing signal handlers
    struct sigaction sa;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    sa.sa_handler = handle_sigint;
    sigaction(SIGINT, &sa, NULL);
    sa.sa_handler = handle_sigusr1;
    sigaction(SIGUSR1, &sa, NULL);
    
    // Main loop
    size_t n;
    while (!shutdown_flag && (n = fread(buffer, 1, buffer_size, input)) > 0)
    {
        fwrite(buffer, 1, n, stdout);
	fflush(stdout);
        total_bytes += n;
        
        if (stats_flag)
        {
            display_stats();
            stats_flag = 0;
        }
    }
    
    // Final output
    if (shutdown_flag) {
        fprintf(stderr, "\nReceived SIGINT - shutting down gracefully...\n");
    }
    display_stats();
    
    // Cleanup
    free(buffer);
    if (input != stdin) fclose(input);
    
    return 0;
}
