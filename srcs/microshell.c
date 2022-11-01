#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <stdlib.h>

//tiene que es

static int	ft_strlen(char *str){
	int i = -1;
	if (!str)
		return 0;
	while(str[++i])
		continue;
	return i;
}

static int error_message(char *msg){
	write(2, msg, ft_strlen(msg));
	write(2, "\n", sizeof(char));
	return 1;
}

static int	execute_cd(char **args){
	int i = -1;
	while (args[++i] != NULL)
		continue;
	if (i != 2) // arreglar eso
		return (error_message("error: bad arguments"));
	if (chdir(args[1]) == -1){
		write(2, "error: cannot change directory to " , sizeof("error: cannot change directory to "));
		write(2, args[1], ft_strlen(args[1]));
		write(2, "\n", 1);
	}
	return 0;
}

int main(int argc, char **argv, char **env){
	int i = -1; //basic iterator
	int cmd = 1; //int that points to the cmd
	int	count = 0; //count the cmd thats being executed
	int	last = 1; //if the last command was a pipe is 1 and if it was a calling its 0
	int	fd[2][2];
	if (argc == 1)
		return (1);
	if (pipe(fd[0]))
		return (error_message("error: fatal"));
	if (pipe(fd[1]))
		return (error_message("error: fatal"));
	while (argv[++i]){
		close(fd[0][1]);
		if (!strcmp(argv[i],"|")){
			++count;
			int pid = fork();
			if (pid == 0 && strcmp(argv[cmd], "cd")){
				if (count != 1 || last == 1)
					dup2(fd[0][0], STDIN_FILENO);
				dup2(fd[1][1], STDOUT_FILENO);
				close(fd[0][0]);
				close(fd[0][1]);
				close(fd[1][0]);
				close(fd[1][1]);
				argv[i] = NULL;
				if (execve(argv[cmd], &argv[cmd], env) == -1){
					write(2, "error: cannot execute ", sizeof("error: cannot execute "));
					write(2, argv[cmd] ,ft_strlen(argv[cmd]));
					write(2, "\n", 1);
				}
				exit (1);
			}
			else
				waitpid(pid,NULL, 0);
			if (!strcmp(argv[cmd], "cd")){
				if (pid == 0)
					exit (0);
				argv[i] = NULL;
				execute_cd(&argv[cmd]);
			}
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
			if (pid == 0 && strcmp(argv[cmd], "cd")){
				if (count != 1 || last == 1)
					dup2(fd[0][0], STDIN_FILENO);
				close(fd[0][0]);
				close(fd[0][1]);
				close(fd[1][0]);
				close(fd[1][1]);
				argv[i] = NULL;
				if (argv[cmd] == NULL)
					exit (0);
				if (execve(argv[cmd], &argv[cmd], env) == -1){
					write(2, "error: cannot execute ", sizeof("error: cannot execute "));
					write(2, argv[cmd] ,ft_strlen(argv[cmd]));
					write(2, "\n", 1);
				}
				exit (1);
			}
			else
				waitpid(pid,NULL, 0);
			if (!strcmp(argv[cmd], "cd")){
				if (pid == 0)
					exit (0);
				argv[i] = NULL;
				execute_cd(&argv[cmd]);
			}
			last = 0;
			cmd = i + 1;
			close(fd[0][0]);
			close(fd[0][1]);
			fd[0][0] = fd[1][0];
			fd[0][1] = fd[1][1];
			pipe(fd[1]);
		}
	}
	if (argv[cmd] == NULL)
		return 0;
	++count;
	int pid = fork();
	if (pid == 0 && strcmp(argv[cmd], "cd")){
		if (count != 1 || last == 1)
			dup2(fd[0][0], STDIN_FILENO);
		close(fd[0][0]);
		close(fd[0][1]);
		close(fd[1][0]);
		close(fd[1][1]);
		argv[i] = NULL;
		if (execve(argv[cmd], &argv[cmd], env) == -1){
			write(2, "error: cannot execute ", sizeof("error: cannot execute "));
			write(2, argv[cmd] ,ft_strlen(argv[cmd]));
			write(2, "\n", 1);
		}
		exit (1);
	}
	else
		waitpid(pid,NULL, 0);
	if (!strcmp(argv[cmd], "cd")){
		if (pid == 0)
			exit (0);
		argv[i] = NULL;
		execute_cd(&argv[cmd]);
	}
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
