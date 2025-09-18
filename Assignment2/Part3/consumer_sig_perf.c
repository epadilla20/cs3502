#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <time.h>
#include <signal.h>

#define BUFFER_SIZE 4096

// Global variables for signal handlers and performance tracking
volatile sig_atomic_t shutdown_flag = 0;
volatile sig_atomic_t stats_flag = 0;
int line_count = 0;
int char_count = 0;
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
    
    fprintf(stderr, "\n=== CONSUMER STATISTICS ===\n");
    fprintf(stderr, "Lines processed: %d\n", line_count);
    fprintf(stderr, "Characters processed: %d\n", char_count);
    fprintf(stderr, "Time elapsed: %.3f seconds\n", cpu_time);
    if (cpu_time > 0) {
        fprintf(stderr, "Lines per second: %.2f\n", line_count / cpu_time);
        fprintf(stderr, "Bytes per second: %.2f\n", char_count / cpu_time);
        fprintf(stderr, "MB per second: %.6f\n", (char_count / 1024.0 / 1024.0) / cpu_time);
    }
    fprintf(stderr, "============================\n");
}

int main(int argc, char *argv[]) 
{
    int max_lines = -1;
    int verbose = 0;
    int opt;
    
    // Start timing
    start_time = clock();
    
    // Parse arguments (-n max_lines, -v verbose)
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
    
    // Installing signal handlers
    struct sigaction sa;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    sa.sa_handler = handle_sigint;
    sigaction(SIGINT, &sa, NULL);
    sa.sa_handler = handle_sigusr1;
    sigaction(SIGUSR1, &sa, NULL);
    
    char buffer[BUFFER_SIZE];
    size_t bytes_read;
    int lines_found = 0;
    
    // Read data in chunks
    while (!shutdown_flag && (bytes_read = fread(buffer, 1, BUFFER_SIZE, stdin)) > 0)
    {
        char_count += bytes_read;
        
        if (verbose)
        {
            fwrite(buffer, 1, bytes_read, stdout);
            fflush(stdout);
        }
        
        // Count actual newlines in this chunk
        for (size_t i = 0; i < bytes_read; i++)
        {
            if (buffer[i] == '\n')
            {
                lines_found++;
                if (max_lines != -1 && lines_found >= max_lines)
                {
                    goto done;
                }
            }
        }
        
        // Handle stats signal
        if (stats_flag)
        {
            // Update line_count for current stats
            line_count = (char_count > 0) ? lines_found + 1 : 0;
            if (char_count > 0 && bytes_read > 0 && buffer[bytes_read-1] == '\n')
            {
                line_count = lines_found;
            }
            
            display_stats();
            stats_flag = 0;
        }
    }
    
    done:
    // Final line count calculation
    if (char_count > 0)
    {
        line_count = lines_found + 1;
        if (char_count > 0 && bytes_read > 0 && buffer[bytes_read-1] == '\n')
        {
            line_count = lines_found;
        }
    }
    else
    {
        line_count = 0;
    }
    
    // Final output
    if (shutdown_flag) {
        fprintf(stderr, "\nReceived SIGINT - shutting down gracefully...\n");
    }
    display_stats();
    
    return 0;
}
