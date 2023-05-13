#include "microshell.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

char **envp;

int	count_arg(char **av)
{
	int i = 0;
	int	counter = 1;

	while (av[i])
	{
		if (!strcmp(av[i], "|") || !strcmp(av[i], ";"))
			counter++;
		i++;
	}
	return (counter);
}

int	count_word(char **av)
{
	int i = 0;
	while (av[i])
	{
		if (!strcmp(av[i], "|") || !strcmp(av[i], ";"))
			break ;
		i++;
	}
	return (i);
}

t_tab get_tab(char **av, int *ii)
{
	t_tab 	tab;
	int		i = *ii;
	int		j = 0;
	int		count = count_word(av + i);

	tab.args = malloc(sizeof(char *) * count + 1);
	tab.args[count] = NULL;
	tab.out = 1;
	while (av[i])
	{
		if (!strcmp(av[i], "|") || !strcmp(av[i], ";"))
		{
			tab.out = 1;
			if (!strcmp(av[i], "|"))
				tab.out = 2;
			break ;
		}
		tab.args[j++] = av[i++];
	}
	*ii = i;
	return (tab);
}

t_tab	*fill_tab(char **av ,int *counter)
{
	t_tab	*tab = NULL;
	int		i = 0;
	int		j = 0;
	int		next_in;
	*counter = count_arg(av);

	tab = malloc(sizeof(t_tab) * *counter);
	next_in = 0;
	while (av[i])
	{
		tab[j] = get_tab(av, &i);
		tab[j].in = next_in;
		next_in = 0;
		if (!av[i])
			break ;
		if (!strcmp(av[i], "|"))
			next_in = 2;
		i++;
		j++;
	}
	return (tab);
}

int	ft_strlen(char *line)
{
	int i= 0;
	if (line == NULL)
		return (0);
	while (line[i])
		i++;
	return (i);
}

void	check_cd(t_tab tab)
{
	if (!tab.args[1] || !strcmp(tab.args[1], "-") || tab.args[2])
	{
		write(2, "error: cd: bad arguments\n", 25);
		return ;
	}
	if (tab.out == 1)
	{
		if (chdir(tab.args[1]) == -1)
		{
			write (2, "error: cd: cannot change directory to ", 38);
			write (2, tab.args[1], ft_strlen(tab.args[1]));
			write (2, "\n", 1);
		}
		return ;
	}
	int pid = fork();
	if (pid == 0)
	{
		if (chdir(tab.args[1]) == -1)
		{
			write (2, "error: cd: cannot change directory to ", 38);
			write (2, tab.args[1], ft_strlen(tab.args[1]));
			write (2, "\n", 1);
		}
		exit(1);
	}
}

void	exec_allcmd(t_tab tab, int *fd)
{
	close(fd[0]);
	if (tab.out == 2)
		dup2(fd[1], 1);
	close(fd[1]);
	execve(tab.args[0], tab.args, envp);
	write(2, "error: cannot execute ", 22);
	write(2, tab.args[0], ft_strlen(tab.args[0]));
	write (2, "\n", 1);
	exit(1);
}


void	exec_cmd(t_tab *tab, int counter)
{
	int i = 0;
	int	pid;
	int	fd[2];

	while (i < counter)
	{
		if (!strcmp(tab[i].args[0], "cd"))
			check_cd(tab[i]);
		else
		{
			pipe(fd);
			pid = fork();
			if (pid == 0)
				exec_allcmd(tab[i], fd);
			close(fd[1]);
			if (tab[i].out == 2)
				dup2(fd[0], 0);
			close(fd[0]);
		}
		i++;
	}
}


int	main(int ac, char **av, char **env)
{
	t_tab	*tab;
	int		counter;
	
	envp = env;
	tab = fill_tab(av + 1, &counter);
	exec_cmd(tab, counter);	

	// int i = 0;
	//
	// while (i < counter)
	// {
	// 	int j =  -1;
	// 	printf ("in : %d  out : %d\t", tab[i].in, tab[i].out);
	// 	while (tab[i].args[++j])
	// 		printf ("%s\t",tab[i].args[j]);
	// 	printf("\n");
	// 	i++;
	// }
}
