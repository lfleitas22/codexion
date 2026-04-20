/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine_actions.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfleitas <lautarofleitas@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/13 20:25:59 by lfleitas          #+#    #+#             */
/*   Updated: 2026/04/13 20:33:53 by lfleitas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	wait_for_turn(t_coder *coder, t_dongle *dongle)
{
	long long	now;

	while (!check_sim_stop(coder->config))
	{
		if (dongle->is_available && dongle->wait_queue.requests[0].coder_id
			== coder->id)
		{
			now = get_time_ms();
			if (now >= dongle->available_at)
				break ;
			pthread_mutex_unlock(&dongle->mutex);
			usleep((dongle->available_at - now) * 1000);
			pthread_mutex_lock(&dongle->mutex);
			continue ;
		}
		pthread_cond_wait(&dongle->cond, &dongle->mutex);
	}
}

void	release_dongle(t_coder *coder, t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->mutex);
	dongle->is_available = 1;
	dongle->available_at = get_time_ms() + coder->config->dongle_cooldown;
	pthread_cond_broadcast(&dongle->cond);
	pthread_mutex_unlock(&dongle->mutex);
}

int	take_dongle(t_coder *coder, t_dongle *dongle)
{
	t_request	req;

	req.coder_id = coder->id;
	req.request_time = get_time_ms();
	pthread_mutex_lock(&coder->coder_mutex);
	req.deadline = coder->last_compile_start + coder->config->time_to_burnout;
	pthread_mutex_unlock(&coder->coder_mutex);
	pthread_mutex_lock(&dongle->mutex);
	heap_push(&dongle->wait_queue, req, coder->config->scheduler);
	wait_for_turn(coder, dongle);
	if (check_sim_stop(coder->config))
	{
		pthread_mutex_unlock(&dongle->mutex);
		return (0);
	}
	heap_pop(&dongle->wait_queue, coder->config->scheduler);
	dongle->is_available = 0;
	safe_print(coder->config, coder->id, "has taken a dongle");
	pthread_mutex_unlock(&dongle->mutex);
	return (1);
}
