/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfleitas <lautarofleitas@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/13 18:18:51 by lfleitas          #+#    #+#             */
/*   Updated: 2026/04/17 16:22:23 by lfleitas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	ft_parse_int_safe(const char *str, int *out_val)
{
	long long	res;
	int			i;

	res = 0;
	i = 0;
	while (str[i] == ' ' || (str[i] >= 9 && str[i] <= 13))
		i++;
	if (str[i] == '+')
		i++;
	while (str[i] >= '0' && str[i] <= '9')
	{
		res = (res * 10) + (str[i] - '0');
		if (res > 2147483647)
			return (0);
		i++;
	}
	*out_val = (int)res;
	return (1);
}

long long	get_time_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
}

int	check_sim_stop(t_config *config)
{
	int	stop;

	pthread_mutex_lock(&config->stop_mutex);
	stop = config->sim_stop;
	pthread_mutex_unlock(&config->stop_mutex);
	return (stop);
}

void	safe_print(t_config *config, int id, char *msg)
{
	long long	timestamp;

	if (check_sim_stop(config))
		return ;
	pthread_mutex_lock(&config->print_mutex);
	if (!config->sim_stop)
	{
		timestamp = get_time_ms() - config->start_time;
		printf("%lld %d %s\n", timestamp, id, msg);
	}
	pthread_mutex_unlock(&config->print_mutex);
}

void	ft_usleep(long long time_in_ms, t_config *config)
{
	long long	start;

	start = get_time_ms();
	while ((get_time_ms() - start) < time_in_ms)
	{
		if (check_sim_stop(config))
			break ;
		usleep(500);
	}
}
