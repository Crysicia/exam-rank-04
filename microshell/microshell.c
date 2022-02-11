#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdbool.h>

void ft_putstr_err(char *str) {
	int index = 0;

	while (str[index])
		index++;
	write(STDERR_FILENO, str, index);
}

int ft_execute(char *argv[], int index, int ft_stdin, char *envp[]) {
	argv[index] = NULL;
	close(ft_stdin);
	execve(argv[0], argv, envp);
	ft_putstr_err("error: cannot execute ");
	ft_putstr_err(argv[0]);
	ft_putstr_err("\n");
	return 1;
}

void close_pipe(int fds[2]) {
	close(fds[0]);
	close(fds[1]);
}

void cd_builtin(int args_length, char *args[]) {
	if (args_length != 2) {
		ft_putstr_err("error: cd: bad arguments\n");
		return ;
	}
	if (chdir(args[1])) {
		ft_putstr_err("error: cd: cannot change directory to ");
		ft_putstr_err(args[1]);
		ft_putstr_err("\n");
	}
}

int cmd_length(char *argv[]) {
	int i = 0;
	while (argv[i] && strcmp(argv[i], ";") && strcmp(argv[i], "|"))
		i++;
	return i;
}

bool is_last_cmd(char *argv[], int index) {
	if (!argv[index] || !strcmp(argv[index], ";"))
		return true;
	return false;
}

int main(int argc, char *argv[], char *envp[]) {
	(void)argc;
	int index = 0;
	int current_pid = 0;
	int fds[2] = {0};
	int ft_stdin = dup(STDIN_FILENO);

	while (argv[index] && argv[index + 1])
	{
		argv = &argv[index + 1];
		index = cmd_length(argv);
		if (!strcmp(argv[0], "cd")) {
			cd_builtin(index, argv);
		} else if (is_last_cmd(argv, index)) {
			current_pid = fork();
			if (!current_pid) {
				if (argv[0][0] == ';')
					return 1;
				dup2(ft_stdin, STDIN_FILENO);
				if (ft_execute(argv, index, ft_stdin, envp))
					return (1);
			} else {
				close(ft_stdin);
				waitpid(current_pid, NULL, 0);
				ft_stdin = dup(STDIN_FILENO);
			}
		} else if (!strcmp(argv[index], "|")) {
			pipe(fds);
			current_pid = fork();
			if (!current_pid) {
				dup2(ft_stdin, STDIN_FILENO);
				dup2(fds[1], STDOUT_FILENO);
				close_pipe(fds);
				if (ft_execute(argv, index, ft_stdin, envp))
					return (1);
			} else {
				close(fds[1]);
				close(ft_stdin);
				waitpid(current_pid, NULL, 0);
				ft_stdin = dup(fds[0]);
				close(fds[0]);
			}
		}
	}
	close(ft_stdin);
	return 0;
}
