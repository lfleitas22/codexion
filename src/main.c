/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfleitas <lautarofleitas@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/13 19:32:49 by lfleitas          #+#    #+#             */
/*   Updated: 2026/04/17 19:12:33 by lfleitas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	launch_threads(t_config *config)
{
	int			i;
	pthread_t	monitor;

	config->start_time = get_time_ms();
	i = -1;
	while (++i < config->num_coders)
	{
		config->coders[i].last_compile_start = config->start_time;
		if (pthread_create(&config->coders[i].thread, NULL,
				&coder_routine, &config->coders[i]) != 0)
			return (0);
	}
	if (pthread_create(&monitor, NULL, &monitor_routine, config) != 0)
		return (0);
	pthread_join(monitor, NULL);
	i = -1;
	while (++i < config->num_coders)
		pthread_join(config->coders[i].thread, NULL);
	return (1);
}

int	main(int argc, char **argv)
{
	t_config	config;

	memset(&config, 0, sizeof(t_config));
	if (!parse_args(argc, argv, &config))
		return (1);
	if (config.req_compiles == 0)
		return (0);
	if (!init_simulation(&config))
	{
		fprintf(stderr, "Error: Memory lapse\n");
		return (1);
	}
	if (!launch_threads(&config))
	{
		fprintf(stderr, "Error: Creating threads (pthread_create).\n");
		pthread_mutex_lock(&config.stop_mutex);
		config.sim_stop = 1;
		pthread_mutex_unlock(&config.stop_mutex);
	}
	destroy_simulation(&config);
	return (0);
}
