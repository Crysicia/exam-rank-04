#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdbool.h>

int ft_strlen(char *str)
{
	int length = 0;
	while (str[length])
		length++;
	return length;
}

void ft_putstr_err(char *str)
{
	write(STDERR_FILENO, str, strlen(str));
}

int ft_execute(int stdin, int index, char *argv[], char *envp[])
{
	argv[index] = NULL;
	close(stdin);
	execve(argv[0], argv, envp);
	ft_putstr_err("error: cannot execute ");
	ft_putstr_err(argv[0]);
	ft_putstr_err("\n");
	return 1;

}

int ft_execute_pipe(int *stdin, int index, char *argv[], char *envp[])
{
	int fds[2];
	pipe(fds);
	int current_pid = fork();

	if (!current_pid)
	{
		dup2(*stdin, STDIN_FILENO);
		dup2(fds[1], STDOUT_FILENO);
		close(fds[0]);
		close(fds[1]);
		if (ft_execute(*stdin, index, argv, envp))
			return 1;
	}
	else
	{
		close(fds[1]);
		close(*stdin);
		waitpid(-1, NULL, 0);
		*stdin = dup(fds[0]);
		close(fds[0]);
	}
	return 0;
}

int ft_execute_last_cmd(int stdin, int index, char *argv[], char *envp[])
{
	int current_pid = fork();
	if (!current_pid)
	{
		dup2(stdin, STDIN_FILENO);
		if (ft_execute(stdin, index, argv, envp))
			return 1;
	}
	else
	{
		close(stdin);
		waitpid(-1, NULL, 0);
	}
	return 0;
}

bool is_last_cmd(int index, char* argv[])
{
	if (argv != &argv[index] && (!argv[index] || !strcmp(argv[index], ";")))
		return true;
	return false;
}

bool is_pipe(int index, char *argv[])
{
	if (argv != &argv[index] && !strcmp(argv[index], "|"))
		return true;
	return false;
}

void cd_builtin(int args_length, char *argv[])
{
	if (args_length > 2)
	{
		ft_putstr_err("error: cd: bad arguments\n");
		return ;
	}
	if (chdir(argv[1]))
	{
		ft_putstr_err("error: cd: cannot change directory to ");
		ft_putstr_err(argv[1]);
		ft_putstr_err("\n");
	}
}

int cmd_length(char *argv[])
{
	int length = 0;
	while (argv[length] && strcmp(argv[length], ";") && strcmp(argv[length], "|"))
		length++;
	return length;
}

int main(int argc, char *argv[], char *envp[])
{
	(void)argc;
	int index = 0;
	int fds[2] = { 0 };
	int stdin = dup(STDIN_FILENO);
	int current_pid = 0;

	while (argv[index] && argv[index + 1])
	{
		argv = &argv[index + 1];
		index = cmd_length(argv);
		if (!strcmp(argv[0], "cd")) {
			cd_builtin(index, argv);
		} else if (is_last_cmd(index, argv)) {
			if (ft_execute_last_cmd(stdin, index, argv, envp))
				return 1;
			// current_pid = fork();
			// if (!current_pid)
			// {
			// 	dup2(stdin, STDIN_FILENO);
			// 	if (ft_execute(stdin, index, argv, envp))
			// 		return 1;
			// }
			// else
			// {
			// 	close(stdin);
			// 	waitpid(-1, NULL, 0);
			// }
		} else if (is_pipe(index, argv)) {
			if (ft_execute_pipe(&stdin, index, argv, envp))
				return 1;
			// pipe(fds);
			// current_pid = fork();
			// if (!current_pid)
			// {
			// 	dup2(stdin, STDIN_FILENO);
			// 	dup2(fds[1], STDOUT_FILENO);
			// 	close(fds[0]);
			// 	close(fds[1]);
			// 	if (ft_execute(stdin, index, argv, envp))
			// 		return 1;
			// }
			// else
			// {
			// 	close(fds[1]);
			// 	close(stdin);
			// 	waitpid(-1, NULL, 0);
			// 	stdin = dup(fds[0]);
			// 	close(fds[0]);
			// }
		}
	}
	close(stdin);
	return 0;
}
