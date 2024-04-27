#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

int main(int argc, char *argv[])
{
	//check if no command is given
	if (argc < 2){
		errno = EINVAL;
		perror("Error: Invalid number of arguments");
		exit(EXIT_FAILURE);
	}

	if (argc == 2){
		//special case when there's only one command
		execlp(argv[1], argv[1], NULL);
		perror("Error: execvp");
		exit(1);
	}
	
	int num_cmds = argc -1;
	//make an array of pids and an array of pipes(each pipe is an array of two file descriptors)
	pid_t *pids = (pid_t*)malloc(num_cmds * sizeof(pid_t));
	int **pipes = (int**)malloc((num_cmds-1) * sizeof(int*));

	//allocate memory for pieps
	for (int i = 0; i < num_cmds-1; i++) {
		pipes[i] = (int*)malloc(2 * sizeof(int));

		if (pipe(pipes[i]) == -1) {
			perror("pipe");
			exit(1);
		}
	}

	//create child processes and instructions they each execute
	for (int j = 0; j < num_cmds; j++) {
		pids[j] = fork();
		if(pids[j] == -1){
			perror("fork");
			exit(1);
		}

		if (pids[j] == 0) {
			//if child process
			if (j == 0){
				//if the first command, set write fd only
				dup2(pipes[0][1], 1);
				close(pipes[0][0]);
				close(pipes[0][1]);
			}
			else if (j == num_cmds-1){
				//if the last command, set read fd only
				dup2(pipes[j-1][0], 0);
				close(pipes[j-1][0]);
			}
			else{
				//if middle command, set both read and write fd
				dup2(pipes[j-1][0], 0);//set read fd to previous pipe
				dup2(pipes[j][1], 1); //set write fd to current pipe
				close(pipes[j-1][0]); //close the unused read fd to the previous pipe
				close(pipes[j][1]); //close the unused write fd to the current pipe
			}
			//close all remaining unused fds
			int max_fd = getdtablesize();
			for (int k = 3; k < max_fd; k++) {
				close(k);
			}
			
			//execute the command
			execlp(argv[j+1], argv[j+1], NULL);
			perror("Error: execvp");
			exit(1);
			
		}
		else{
			int status;
			waitpid(pids[j], &status, 0); //wait for child process to finish and check child status
			if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
				perror("Error: Child process exited with non-zero status");
				exit(1);
			}

			if (j == 0){
				//if first command, close write fd of pipe only
				close(pipes[0][1]);
			}
			else if (j == num_cmds-1){
				//if last command, close read fd of the last pipe only
				close(pipes[j-1][0]);
			}
			else{
				//if middle command, close the previous read and the current write fd of the pipe
				//leaveing the current read fd of the pipe open for the next child to use
				close(pipes[j-1][0]);
				close(pipes[j][1]);
			}
		}
	
	}
	//close all remaining unused fds
	int max_fd = getdtablesize();
	for (int k = 3; k < max_fd; k++) {
		close(k);
	}

	//free memory
	free(pids);
	for (int i = 0; i < num_cmds-1; i++) {
		free(pipes[i]);
	}
	free(pipes);
	return 0;
}
