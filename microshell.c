#include "microshell.h"
#include <string.h>

char **envp;

int	count_arg(char **av)
{
	int i = 0;
	int	counter = 0;

	while (av[i])
	{
		if (strcmp(av[i], "|") && strcmp(av[i] , ";"))
		{
			i++;
			counter++;
			while (av[i] && strcmp(av[i], "|") && strcmp(av[i] , ";"))
				i++;
		}
		else 
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
		while (av[i] && (!strcmp(av[i], ";") || !strcmp(av[i], "|")))
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

void	exec_allcmd(t_tab tab, int *fd, int *tmp_in)
{
	close(fd[0]);
	if (tab.out == 2)
		dup2(fd[1], 1);
	if (tab.in == 2)
		dup2(*tmp_in ,0);
	close(*tmp_in);
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
	int	tmp_in;

	tmp_in = dup(0);
	while (i < counter)
	{
		if (tab[i].in == 0)
			while (wait(NULL) != -1);
		if (!strcmp(tab[i].args[0], "cd"))
			check_cd(tab[i]);
		else
		{
			if (pipe(fd))
				exit(1);
			pid = fork();
			if (pid == -1)
				exit(1);
			if (pid == 0)
				exec_allcmd(tab[i], fd, &tmp_in);
			close(fd[1]);
			if (tab[i].out == 2)
				dup2(fd[0], tmp_in);
			close(fd[0]);
		}
		i++;
	}
	close(tmp_in);
	while (wait(NULL) != -1);
}

void	free_all(t_tab *tab, int counter)
{
	int i = 0;

	while (i < counter)
		free(tab[i++].args);
	
	free(tab);
}

char	**skip(char **av)
{
	int i = 0;

	while (av[i])
	{
		if (!strcmp(av[i], ";") || !strcmp(av[i], "|"))
			i++;
		else 
		break ;
	}
	if (!av[i])
		exit (1);
	return (av + i);
}

int	main(int ac, char **av, char **env)
{
	t_tab	*tab;
	int		counter;
	
	if (ac == 1)
		return 0;
	av = skip(av + 1);
	envp = env;
	tab = fill_tab(av, &counter);

	exec_cmd(tab, counter);	
	free_all(tab, counter);
}
