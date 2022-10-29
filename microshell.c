#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <stdlib.h>

//tiene que es
static int ft_countcmd(char **argv){
	int i = -1;
	int count = 0;
	while(argv[++i]){
		if (!strcmp(argv[i], "|") || !strcmp(argv[i], ";"))
			count++;
	}
	return count;
}

int main(int argc, char **argv, char **env){
	int i = -1; //basic iterator
	int cmd = 1; //int that points to the cmd
	int	count = 0; //count the cmd thats being executed
	int	last = 1; //if the last command was a pipe is 1 and if it was a calling its 0
	int	fd[2][2];
	if (argc == 1)
		return 1;
	if (pipe(fd[0]))
		return 1;
	if (pipe(fd[1]))
		return 1;
	int	total = ft_countcmd(argv);
	(void) total;
	while (argv[++i]){
		close(fd[0][1]);
		if (!strcmp(argv[i],"|")){
			++count;
			int pid = fork();
			if (pid == 0){
				if (count != 1 || last == 1)
					dup2(fd[0][0], STDIN_FILENO);
				dup2(fd[1][1], STDOUT_FILENO);
				close(fd[0][0]);
				close(fd[0][1]);
				close(fd[1][0]);
				close(fd[1][1]);
				argv[i] = NULL;
				execve(argv[cmd], &argv[cmd], env);
				exit (0);
			}
			else
				waitpid(pid,NULL, 0);
			last = 1;
			cmd = i + 1;
			close(fd[0][0]);
			close(fd[0][1]);
			fd[0][0] = fd[1][0];
			fd[0][1] = fd[1][1];
			pipe(fd[1]);
		}
		else if (!strcmp(argv[i],";")){
			++count;
			int pid = fork();
			if (pid == 0){
				if (count != 1 || last == 1)
					dup2(fd[0][0], STDIN_FILENO);
				close(fd[0][0]);
				close(fd[0][1]);
				close(fd[1][0]);
				close(fd[1][1]);
				argv[i] = NULL;
				execve(argv[cmd], &argv[cmd], env);
				exit (0);
			}
			else
				waitpid(pid,NULL, 0);
			last = 0;
			cmd = i + 1;
			close(fd[0][0]);
			close(fd[0][1]);
			fd[0][0] = fd[1][0];
			fd[0][1] = fd[1][1];
			pipe(fd[1]);
		}
//		printf("fd[0][0] = %d || fd[0][1] = %d\n", fd[0][0],fd[0][1]);
//		printf("fd[1][0] = %d || fd[1][1] = %d\n", fd[1][0],fd[1][1]);
	}
	++count;
	int pid = fork();
	if (pid == 0){
		if (count != 1 || last == 1)
			dup2(fd[0][0], STDIN_FILENO);
		close(fd[0][0]);
		close(fd[0][1]);
		close(fd[1][0]);
		close(fd[1][1]);
		argv[i] = NULL;
		execve(argv[cmd], &argv[cmd], env);
		exit (0);
	}
	else
		waitpid(pid,NULL, 0);
	last = 1;
	cmd = i + 1;
	close(fd[0][0]);
	close(fd[0][1]);
	fd[0][0] = fd[1][0];
	fd[0][1] = fd[1][1];
	pipe(fd[1]);
	close(fd[0][0]);
	close(fd[0][1]);
	close(fd[1][0]);
	close(fd[1][1]);
	return 0;
}
