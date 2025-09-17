#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main()
{
	int pipe1[2];
	int pipe2[2];
	pid_t pid;  
	
	//Create both pipes
	if(pipe(pipe1) == -1)
	{
		perror("pipe1");
		exit(1);
	}
	
	if(pipe(pipe2) == -1)
	{
		perror("pipe2");
		exit(1);
	}
	
	//Fork
	pid = fork();
	if (pid < 0)
	{
		perror("fork");
		exit(1);
	}
	
	if (pid == 0) 
	{
		// Child process
		close(pipe1[1]);
		close(pipe2[0]);
		
		char buffer[128];
		ssize_t n;
		while ((n = read(pipe1[0], buffer, sizeof(buffer)-1)) > 0)
		{
			buffer[n] = '\0';
			printf("Child received: %s", buffer);
			char response[200];
			snprintf(response, sizeof(response), "Child says: I got your message: %s", buffer);
			write(pipe2[1], response, strlen(response));
		}
		close(pipe1[0]);
		close(pipe2[1]);
		exit(0);
	}
	else
	{
		// Parent process
		close(pipe1[0]);
		close(pipe2[1]);
		
		const char *messages[] = {"Hello child!\n", "How are you?\n", "Goodbye!\n"};
		char buffer[256];
		for(int i = 0; i < 3; i++)
		{
			write(pipe1[1], messages[i], strlen(messages[i]));  
			printf("Parent sent: %s", messages[i]);
			ssize_t n = read(pipe2[0], buffer, sizeof(buffer) - 1);
			if (n > 0)
			{
				buffer[n] = '\0';
				printf("Parent received: %s\n", buffer);  
			}
		}
		close(pipe1[1]);
		close(pipe2[0]);
		wait(NULL);
	}
	return 0;
}
