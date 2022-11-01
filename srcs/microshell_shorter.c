#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>

static int ft_strlen(const char *str){
	int i = -1;
	if (!str)
		return 0;
	while(str[++i])
		continue;
	return i;
}

static	int	error_message(char *str){
	write(2, str, ft_strlen(str));
	write(2, "\n", 1);
	return 1;
}

static int	exec_cd(char **args){
	int i = -1;
	while(args[++i])
		continue;
	if (i != 2)
		return (error_message("error: bad arguments"));
	if (chdir(args[1]) == -1){
		write(2, "error: cannot change directory to ", sizeof("error: cannot change directory to "));
		write(2, args[1], ft_strlen(args[1]));
		write(2, "\n", 1);
	}
	return 0;
}

static void	ejecu_troll(char **argv, int *i, int *count, int *cmd, int *last, int fd[2][2], char **env, const int type){
	++(*count);
	int pid = fork();
	if (pid == 0 && strcmp(argv[*cmd], "cd")){
		if (*count != 1 || *last == 1)
			dup2(fd[0][0], STDIN_FILENO);
		if (type == 1)
			dup2(fd[1][1], STDOUT_FILENO);
		close(fd[0][0]);
		close(fd[0][1]);
		close(fd[1][0]);
		close(fd[1][1]);
		argv[*i] = NULL;
		if (execve(argv[*cmd], &argv[*cmd], env) == -1){
			write(2, "error: cannot execute ", sizeof("error: cannot execute "));
			write(2, argv[*cmd] ,ft_strlen(argv[*cmd]));
			write(2, "\n", 1);
		}
		exit (1);
	}
	else
		waitpid(pid, NULL, 0);
	if (!strcmp(argv[*cmd], "cd")){
		if (pid == 0)
			exit (0);
		argv[*i] = NULL;
		exec_cd(&argv[*cmd]);
	}
	if (type == 1)
		*last = 1;
	else 
		*last = 0;
	*cmd = *i + 1;
	close(fd[0][0]);
	close(fd[0][1]);
	fd[0][0] = fd[1][0];
	fd[0][1] = fd[1][1];
	if (pipe(fd[1]) == -1){
		error_message("error: fatal");
		exit (1);
	}
}

int main(int argc, char **argv, char **env){
	int i = -1, cmd = 1, count = 0, last = 1;
	int fd[2][2];
	if (argc == 1)
		return (1);
	if (pipe(fd[0]) == -1 || pipe(fd[1]) == -1)
		return (error_message("error: fatal"));
	while (argv[++i]){
		close(fd[0][1]);
		if (!strcmp(argv[i], "|"))
			ejecu_troll(argv, &i, &count, &cmd, &last, fd, env, 1);
		else if (!strcmp(argv[i], ";"))
			ejecu_troll(argv, &i, &count, &cmd, &last, fd, env, 0);
	}
	if (argv[cmd] == NULL)
		return 0;
	ejecu_troll(argv, &i, &count, &cmd, &last, fd, env, 0);
	close(fd[0][0]);
	close(fd[0][1]);
	close(fd[1][0]);
	close(fd[1][1]);
	return 0;
}
