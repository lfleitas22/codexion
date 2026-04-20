/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfleitas <lautarofleitas@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/13 19:25:15 by lfleitas          #+#    #+#             */
/*   Updated: 2026/04/17 16:23:52 by lfleitas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	set_dongle_order(t_coder *coder,
			t_dongle **first, t_dongle **second)
{
	if (coder->id % 2 == 0)
	{
		*first = coder->right_dongle;
		*second = coder->left_dongle;
	}
	else
	{
		*first = coder->left_dongle;
		*second = coder->right_dongle;
	}
}

static void	do_coder_tasks(t_coder *coder, t_dongle *first, t_dongle *second)
{
	safe_print(coder->config, coder->id, "is compiling");
	pthread_mutex_lock(&coder->coder_mutex);
	coder->last_compile_start = get_time_ms();
	coder->compiles_count++;
	pthread_mutex_unlock(&coder->coder_mutex);
	ft_usleep(coder->config->time_to_compile, coder->config);
	release_dongle(coder, first);
	release_dongle(coder, second);
	safe_print(coder->config, coder->id, "is debugging");
	ft_usleep(coder->config->time_to_debug, coder->config);
	safe_print(coder->config, coder->id, "is refactoring");
	ft_usleep(coder->config->time_to_refactor, coder->config);
}

static void	execute_cycle(t_coder *coder)
{
	t_dongle	*first;
	t_dongle	*second;

	set_dongle_order(coder, &first, &second);
	if (!take_dongle(coder, first))
		return ;
	if (first == second)
	{
		ft_usleep(coder->config->time_to_burnout + 10, coder->config);
		release_dongle(coder, first);
		return ;
	}
	if (!take_dongle(coder, second))
	{
		release_dongle(coder, first);
		return ;
	}
	do_coder_tasks(coder, first, second);
}

void	*coder_routine(void *arg)
{
	t_coder	*coder;

	coder = (t_coder *)arg;
	if (coder->id % 2 == 0)
		usleep(1500);
	while (!check_sim_stop(coder->config))
	{
		execute_cycle(coder);
	}
	return (NULL);
}
