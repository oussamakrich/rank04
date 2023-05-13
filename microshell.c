#include "microshell.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


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


int	main(int ac, char **av)
{
	t_tab	*tab;
	int		counter;
	
	tab = fill_tab(av + 1, &counter);

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
