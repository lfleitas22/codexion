/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfleitas <lautarofleitas@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/13 18:46:34 by lfleitas          #+#    #+#             */
/*   Updated: 2026/04/13 18:46:35 by lfleitas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	destroy_simulation(t_config *c)
{
	int	i;

	if (c->dongles)
	{
		i = -1;
		while (++i < c->num_coders)
		{
			pthread_mutex_destroy(&c->dongles[i].mutex);
			pthread_cond_destroy(&c->dongles[i].cond);
			if (c->dongles[i].wait_queue.requests)
				free(c->dongles[i].wait_queue.requests);
		}
		free(c->dongles);
	}
	if (c->coders)
	{
		i = -1;
		while (++i < c->num_coders)
			pthread_mutex_destroy(&c->coders[i].coder_mutex);
		free(c->coders);
	}
	pthread_mutex_destroy(&c->print_mutex);
	pthread_mutex_destroy(&c->stop_mutex);
}

static int	init_global_mutexes(t_config *c)
{
	c->sim_stop = 0;
	if (pthread_mutex_init(&c->stop_mutex, NULL) != 0)
		return (0);
	if (pthread_mutex_init(&c->print_mutex, NULL) != 0)
	{
		pthread_mutex_destroy(&c->stop_mutex);
		return (0);
	}
	return (1);
}

static int	init_dongles(t_config *c)
{
	int	i;

	c->dongles = malloc(sizeof(t_dongle) * c->num_coders);
	if (!c->dongles)
		return (0);
	memset(c->dongles, 0, sizeof(t_dongle) * c->num_coders);
	i = -1;
	while (++i < c->num_coders)
	{
		c->dongles[i].id = i + 1;
		c->dongles[i].is_available = 1;
		c->dongles[i].available_at = 0;
		if (pthread_mutex_init(&c->dongles[i].mutex, NULL) != 0)
			return (0);
		if (pthread_cond_init(&c->dongles[i].cond, NULL) != 0)
			return (0);
		if (!heap_init(&c->dongles[i].wait_queue, c->num_coders))
			return (0);
	}
	return (1);
}

static int	init_coders(t_config *c)
{
	int	i;

	c->coders = malloc(sizeof(t_coder) * c->num_coders);
	if (!c->coders)
		return (0);
	memset(c->coders, 0, sizeof(t_coder) * c->num_coders);
	i = -1;
	while (++i < c->num_coders)
	{
		c->coders[i].id = i + 1;
		c->coders[i].compiles_count = 0;
		c->coders[i].config = c;
		c->coders[i].left_dongle = &c->dongles[i];
		c->coders[i].right_dongle = &c->dongles[(i + 1) % c->num_coders];
		if (pthread_mutex_init(&c->coders[i].coder_mutex, NULL) != 0)
			return (0);
	}
	return (1);
}

int	init_simulation(t_config *c)
{
	c->coders = NULL;
	c->dongles = NULL;
	if (!init_global_mutexes(c))
		return (0);
	if (!init_dongles(c))
	{
		destroy_simulation(c);
		return (0);
	}
	if (!init_coders(c))
	{
		destroy_simulation(c);
		return (0);
	}
	return (1);
}
