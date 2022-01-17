#include "microshell.h"

int ft_strlen(char *str)
{
	int length = 0;

	while (str[length])
		length++;
	return (length);
}

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

void exit_error(char *message)
{
	display_error(NULL, message, NULL);
	exit(1);
}

int builtin_cd(char *path)
{
	if (!path)
		return (display_error("cd", ERR_MSG_CD_ARGS, NULL));
	if (chdir(path))
		return (display_error("cd", ERR_MSG_CD_PATH, path));
	return (0);
}

int execute_single_command(char **arguments, char *envp[])
{
	pid_t pid;

	// handle pipes
	pid = fork();
	if (!pid)
	{
		// handle pipes
		// printf("Args: [%s]\n", arguments[1]);
		// dprintf(STDERR_FILENO, "TAMER [%s]\n", arguments[1]);
		if (execve(*arguments, arguments, envp))
		{
			display_error(NULL, ERR_MSG_EXECVE, *arguments);
			exit(1);
		}
	}
	return (0);
}

char *ft_strdup(char *str)
{
	int i;
	int size = ft_strlen(str);
	char *dup = malloc(sizeof(char) * size + 1);

	if (!dup)
		return (NULL);
	for (i = 0; i < size + 1; i++)
		dup[i] = str[i];
	return (dup);
}

int get_next_command(int starting_index, char **argv, t_command **next_command)
{
	int index			= 0;
	int cmd_length		= 0;
	char **fast_ptr		= argv;

	if (*fast_ptr == NULL)
		return (0);
	if (!(*next_command = malloc(sizeof(t_command))))
		exit_error(ERR_MSG_SYSCALL);
	(*next_command)->type = COMMAND;
	if (!strcmp(*fast_ptr, "|") || !strcmp(*fast_ptr, ";"))
	{
		(*next_command)->type = !strcmp(*fast_ptr, "|") ? PIPE : SEMICOLON;
		(*next_command)->arguments = NULL;
		// printf("PIPE\n");
		return (starting_index + 1);
	}
	while (*fast_ptr && strcmp(*fast_ptr, "|") && strcmp(*fast_ptr, ";"))
		fast_ptr++;
	cmd_length = fast_ptr - argv;
	if (cmd_length == 0)
		return (0);
	if (!((*next_command)->arguments = malloc(cmd_length * sizeof(char *) + 1)))
		exit_error(ERR_MSG_SYSCALL);
	while (index < cmd_length)
	{
		(*next_command)->arguments[index] = argv[index];
		index++;
	}
	(*next_command)->arguments[index] = NULL;
	return (cmd_length + starting_index);
}

t_list *ft_lstnew(t_command *command)
{
	t_list *node = NULL;

	if (!(node = malloc(sizeof(t_list))))
		return NULL;
	node->next				= NULL;
	node->command			= command;

	return node;
}


void	ft_lstadd_back(t_list **head, t_list *tail)
{
	if (!tail)
		return ;
	if (!*head)
	{
		(*head) = tail;
		return ;
	}
	if (!(*head)->next)
	{

		(*head)->next = tail;
		return ;
	}
	ft_lstadd_back(&((*head)->next), tail);
}

t_list *parse_commands(int argc, char *argv[])
{
	t_list *head 			= NULL;
	t_list *next_node	 	= NULL;
	t_command *next_command = NULL;
	int command_index 		= 1;

	while (command_index > 0 && command_index != argc)
	{
		next_command = NULL;
		command_index = get_next_command(command_index, &argv[command_index], &next_command);
		if (!(next_node = ft_lstnew(next_command)))
			exit_error(ERR_MSG_SYSCALL);
		ft_lstadd_back(&head, next_node);
	}
	return head;
}


void execute_commands(t_list *commands, char *envp[])
{
	t_list *current_command = commands;
	while (current_command)
	{
		execute_single_command(current_command->command->arguments, envp);
		// free(next_command);
		current_command = current_command->next;
	}
}

int main(int argc, char *argv[], char *envp[])
{
	(void)envp;
	if (argc == 1)
		return (0);
	t_list *commands = parse_commands(argc, argv);
	execute_commands(commands, envp);
	return (0);
}
