#include "microshell.h"

int display_error(char *command, char *message, char *suffix)
{
	write(STDERR_FILENO, "error: ", 7);
	if (command)
	{
		write(STDERR_FILENO, command, ft_strlen(command));
		write(STDERR_FILENO, ": ", 2);
	}
	write(STDERR_FILENO, message, ft_strlen(message));
	if (suffix)
	{
		write(STDERR_FILENO, " ", 1);
		write(STDERR_FILENO, suffix, ft_strlen(suffix));
	}
	write(STDERR_FILENO, "\n", 1);
	return (1);
}

int builtin_cd(char *path)
{
	if (!path)
		return (display_error("cd", ERR_MSG_CD_ARGS, NULL));
	if (chdir(path))
		return (display_error("cd", ERR_MSG_CD_PATH, path));
}

int execute_single_command(char **arguments, char *envp[])
{
	pid_t pid;

	// handle pipes
	pid = fork()
	if (!pid)
	{
		// handle pipes
		if (execve(*arguments, arguments[1], envp))
		{
			display_error(NULL, ERR_MSG_EXECVE, *arguments);
			exit(1);
		}
	}
	return (0);
}



int get_next_command(int index, char **argv, char ***next_command)
{
	int index = 0;
	int cmd_length = 0;
	char **fast_ptr = argv;

	while (*fast_ptr && strcmp(*fast_ptr, "|") && strcmp(*fast_ptr, ";"))
		fast_ptr++;
	if (!strcmp(*fast_ptr, "|") || !strcmp(*fast_ptr, ";"))
		fast_ptr--;
	cmd_length = fast_ptr - argv;
	if (cmd_length == 0)
		return (0);
	*next_command = malloc(cmd_length * sizeof(char *));
	// check alloc return 
	while (index < cmd_length)
	{
		*next_command[index] = argv[index];
		index++;
	}
	return (cmd_length);
}

void execute_commands(int argc, char *argv[], char *envp[])
{
	char **next_command;
	int command_index = 0;

	while ((command_index = get_next_command(index, &argv[command_index], &next_command)) > 0)
	{
		execute_single_command(next_command, envp)
		free(&next_command);
	}
}

int main(int argc, char *argv[], char *envp[])
{
	if (argc == 1)
		return (0);
	execute_commands(argc, argv, envp);
	return (0);
}
