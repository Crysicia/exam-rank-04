#ifndef MICROSHELL_H
# define MICROSHELL_H

# define ERR_MSG_CD_ARGS "bad argumments"
# define ERR_MSG_CD_PATH "cannot change directory to"
# define ERR_MSG_SYSCALL "fatal"
# define ERR_MSG_EXECVE  "cannot execute"
# include <unistd.h>
# include <stdlib.h>
# include <string.h>
# include <stdio.h>

# define COMMAND	0
# define PIPE		1
# define SEMICOLON	2

typedef struct		s_command
{
	unsigned char	type;
	char			**arguments;
}					t_command;

typedef struct		s_list
{
	t_command		*command;
	struct s_list	*next;
}					t_list;

#endif
