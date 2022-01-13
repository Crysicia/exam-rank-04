
// Satcheen says:
// 	Copy argv
// 	Replace every "|" by NULL
// 	Send &argv[command_index] to execve

#include "microshell.h"

void fatal_error(void)
{
	write(STDERR_FILENO, "fatal\n", 6);
	exit(1);
}

size_t ft_min(size_t a, size_t b)
{
	return a > b ? b : a;
}

size_t ft_strlen(char *str)
{
	size_t length = 0;

	while (str[length])
		length++;
	return length;
}

char *ft_strdup(char *str)
{
	size_t index = 0;
	size_t length = ft_strlen(str);
	char *dup = malloc(sizeof(char) * length);

	while (index <= length)
	{
		dup[index] = str[index];
		index++;
	}
	return dup;
}

size_t distance_from(char *argv[], char *str)
{
	size_t distance = 0;
	size_t index = 0;

	if (!argv)
		return 0;
	while (argv[index] && strcmp(argv[index], str))
		index++;
	return index;
}

char **get_next_command(char *argv[], int *next_index)
{
	size_t command_length = ft_min(distance_from(argv, ";"), distance_from(argv, "|"));
	char **next_command;

	if (command_length == 0)
		return NULL;
	if (!(next_command = malloc(sizeof(char *) * command_length)))
		fatal_error();
	for (size_t i = 0; i < command_length; i++)
		if (!(next_command[i] = ft_strdup(argv[i])))
			fatal_error();
	*next_index += command_length;
	return next_command;
}

int main(int argc, char *argv[], char *envp[])
{
	(void) envp;

	int index = 1;
	char **cmd;
	printf("OG: [%s]\nDUP: [%s]\n", argv[1], ft_strdup(argv[1]));
	printf("Distance from pipe: %lu\n", distance_from(&argv[1], "|"));
	printf("NULL Distance from pipe: %lu\n", distance_from(NULL, "|"));

	while ((cmd = get_next_command(&argv[index], &index)))
	{
		for (int i = 0; i < index - 1; i++)
			printf("%02d: CMD [%s]\n", i, cmd[i]);
		index++;
	}
}
