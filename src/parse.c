/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfleitas <lautarofleitas@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/13 17:57:12 by lfleitas          #+#    #+#             */
/*   Updated: 2026/04/20 17:08:20 by lfleitas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	is_valid_number(const char *str)
{
	int	i;

	i = 0;
	if (str[i] == '+' || str[i] == '-')
		i++;
	if (str[i] == '\0')
		return (0);
	while (str[i])
	{
		if (str[i] < '0' || str[i] > '9')
			return (0);
		i++;
	}
	return (1);
}

static int	check_numeric_args(int argc, char **argv)
{
	int	i;

	i = 1;
	while (i < argc - 1)
	{
		if (!is_valid_number(argv[i]))
		{
			fprintf(stderr, "Error: Invalid numeric argument: %s\n", argv[i]);
			return (0);
		}
		i++;
	}
	return (1);
}

static int	set_scheduler(char *sched_str, t_config *config)
{
	if (strcmp(sched_str, "fifo") == 0)
		config->scheduler = FIFO;
	else if (strcmp(sched_str, "edf") == 0)
		config->scheduler = EDF;
	else
	{
		fprintf(stderr, "Error: The scheduler must be ‘fifo’ or 'edf'\n");
		return (0);
	}
	return (1);
}

static int	assign_and_check_limits(char **argv, t_config *config)
{
	int	values[7];
	int	i;

	i = 0;
	while (i < 7)
	{
		if (!ft_parse_int_safe(argv[i + 1], &values[i]) || values[i] < 0
			|| (i <= 4 && values[i] == 0))
		{
			fprintf(stderr, "Error: Invalid argument [%s]\n", argv[i + 1]);
			return (0);
		}
		i++;
	}
	config->num_coders = values[0];
	config->time_to_burnout = values[1];
	config->time_to_compile = values[2];
	config->time_to_debug = values[3];
	config->time_to_refactor = values[4];
	config->req_compiles = values[5];
	config->dongle_cooldown = values[6];
	return (1);
}

int	parse_args(int argc, char **argv, t_config *config)
{
	if (argc != 9)
	{
		fprintf(stderr, "Error: Usage: ./codexion [coders] [burn] [comp] ");
		fprintf(stderr, "[debug] [refact] [req_comp] [cool] [sched]\n");
		return (0);
	}
	if (!check_numeric_args(argc, argv) || !set_scheduler(argv[8], config))
		return (0);
	if (!assign_and_check_limits(argv, config))
		return (0);
	return (1);
}
