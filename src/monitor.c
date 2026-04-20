/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfleitas <lautarofleitas@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/13 20:10:26 by lfleitas          #+#    #+#             */
/*   Updated: 2026/04/17 20:16:19 by lfleitas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	check_burnout(t_coder *coder, long long time_to_burnout)
{
	long long	elapsed;

	pthread_mutex_lock(&coder->coder_mutex);
	elapsed = get_time_ms() - coder->last_compile_start;
	pthread_mutex_unlock(&coder->coder_mutex);
	if (elapsed >= time_to_burnout)
		return (1);
	return (0);
}

static int	check_finished(t_coder *coder, int req_compiles)
{
	int	finished;

	if (req_compiles <= 0)
		return (0);
	pthread_mutex_lock(&coder->coder_mutex);
	finished = (coder->compiles_count >= req_compiles);
	pthread_mutex_unlock(&coder->coder_mutex);
	return (finished);
}

static void	kill_simulation(t_config *c, int coder_id)
{
	pthread_mutex_lock(&c->print_mutex);
	pthread_mutex_lock(&c->stop_mutex);
	if (!c->sim_stop)
	{
		c->sim_stop = 1;
		printf("%lld %d burned out\n", get_time_ms() - c->start_time, coder_id);
	}
	pthread_mutex_unlock(&c->stop_mutex);
	pthread_mutex_unlock(&c->print_mutex);
}

static int	check_all_coders(t_config *c)
{
	int	i;
	int	finished_count;

	i = -1;
	finished_count = 0;
	while (++i < c->num_coders)
	{
		if (check_burnout(&c->coders[i], c->time_to_burnout))
		{
			kill_simulation(c, c->coders[i].id);
			return (1);
		}
		if (check_finished(&c->coders[i], c->req_compiles))
			finished_count++;
	}
	if (c->req_compiles > 0 && finished_count == c->num_coders)
	{
		pthread_mutex_lock(&c->stop_mutex);
		c->sim_stop = 1;
		pthread_mutex_unlock(&c->stop_mutex);
		return (1);
	}
	return (0);
}

void	*monitor_routine(void *arg)
{
	t_config	*c;
	int			i;

	c = (t_config *)arg;
	while (!check_sim_stop(c))
	{
		if (check_all_coders(c))
			break ;
		usleep(1000);
	}
	i = -1;
	while (++i < c->num_coders)
	{
		pthread_mutex_lock(&c->dongles[i].mutex);
		pthread_cond_broadcast(&c->dongles[i].cond);
		pthread_mutex_unlock(&c->dongles[i].mutex);
	}
	return (NULL);
}
