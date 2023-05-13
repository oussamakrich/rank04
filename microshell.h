# ifndef MICROSHELL_H
#define MICROSHELL_H

# include <stdlib.h>
# include <unistd.h>
# include <signal.h>
#include <string.h>
# include <fcntl.h>
# include <sys/wait.h>



typedef struct s_tab
{
		char	**args;	
		int		in;
		int		out;
} 		t_tab;

#endif
