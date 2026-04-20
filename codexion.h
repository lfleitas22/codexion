/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfleitas <lautarofleitas@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/03 19:14:43 by lfleitas          #+#    #+#             */
/*   Updated: 2026/04/13 20:33:16 by lfleitas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>
# include <sys/time.h>
# include <unistd.h>
# include <stdlib.h>
# include <stdio.h>
# include <string.h>

# ifndef INT_MAX
#  define INT_MAX 2147483647
# endif

typedef enum e_scheduler
{
	FIFO,
	EDF
}	t_scheduler;

typedef struct s_config	t_config;

typedef struct s_request
{
	int			coder_id;
	long long	request_time;
	long long	deadline;
}	t_request;

typedef struct s_heap
{
	t_request	*requests;
	int			size;
	int			capacity;
}	t_heap;

typedef struct s_dongle
{
	int				id;
	pthread_mutex_t	mutex;
	pthread_cond_t	cond;
	int				is_available;
	long long		available_at;
	t_heap			wait_queue;
}	t_dongle;

typedef struct s_coder
{
	int				id;
	pthread_t		thread;
	t_dongle		*left_dongle;
	t_dongle		*right_dongle;
	pthread_mutex_t	coder_mutex;
	long long		last_compile_start;
	int				compiles_count;
	t_config		*config;
}	t_coder;

struct s_config
{
	int				num_coders;
	long long		time_to_burnout;
	long long		time_to_compile;
	long long		time_to_debug;
	long long		time_to_refactor;
	int				req_compiles;
	long long		dongle_cooldown;
	t_scheduler		scheduler;
	long long		start_time;
	int				sim_stop;
	pthread_mutex_t	stop_mutex;
	pthread_mutex_t	print_mutex;
	t_coder			*coders;
	t_dongle		*dongles;
};

/* parse.c */
int			parse_args(int argc, char **argv, t_config *config);

/* init.c */
int			init_simulation(t_config *config);
void		destroy_simulation(t_config *config);

/* routine.c */
void		*coder_routine(void *arg);

/* routine_actions.c */
int			take_dongle(t_coder *coder, t_dongle *dongle);
void		release_dongle(t_coder *coder, t_dongle *dongle);

/* monitor.c */
void		*monitor_routine(void *arg);

/* scheduler.c (Gestión del Heap) */
int			heap_push(t_heap *heap, t_request req, t_scheduler type);
t_request	heap_pop(t_heap *heap, t_scheduler type);
int			heap_init(t_heap *heap, int capacity);
void		heap_free(t_heap *heap);

/* utils.c */
long long	get_time_ms(void);
int			ft_parse_int_safe(const char *str, int *out_val);
void		ft_usleep(long long time_in_ms, t_config *config);
void		safe_print(t_config *config, int coder_id, char *msg);
int			check_sim_stop(t_config *config);

#endif
