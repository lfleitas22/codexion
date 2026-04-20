/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfleitas <lautarofleitas@gmail.com>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/13 18:49:26 by lfleitas          #+#    #+#             */
/*   Updated: 2026/04/13 18:50:55 by lfleitas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	is_higher(t_request a, t_request b, t_scheduler type)
{
	if (type == FIFO)
		return (a.request_time < b.request_time);
	if (a.deadline == b.deadline)
		return (a.request_time < b.request_time);
	return (a.deadline < b.deadline);
}

int	heap_init(t_heap *heap, int capacity)
{
	heap->requests = malloc(sizeof(t_request) * capacity);
	if (!heap->requests)
		return (0);
	heap->size = 0;
	heap->capacity = capacity;
	return (1);
}

static void	heapify_down(t_heap *heap, int i, t_scheduler type)
{
	int			left;
	int			right;
	int			smallest;
	t_request	tmp;

	while (1)
	{
		left = 2 * i + 1;
		right = 2 * i + 2;
		smallest = i;
		if (left < heap->size)
			if (is_higher(heap->requests[left], heap->requests[smallest], type))
				smallest = left;
		if (right < heap->size)
			if (is_higher(heap->requests[right],
					heap->requests[smallest], type))
				smallest = right;
		if (smallest == i)
			break ;
		tmp = heap->requests[i];
		heap->requests[i] = heap->requests[smallest];
		heap->requests[smallest] = tmp;
		i = smallest;
	}
}

int	heap_push(t_heap *heap, t_request req, t_scheduler type)
{
	int			i;
	int			p;
	t_request	tmp;

	if (heap->size >= heap->capacity)
		return (0);
	i = heap->size++;
	heap->requests[i] = req;
	while (i != 0)
	{
		p = (i - 1) / 2;
		if (is_higher(heap->requests[p], heap->requests[i], type))
			break ;
		tmp = heap->requests[i];
		heap->requests[i] = heap->requests[p];
		heap->requests[p] = tmp;
		i = p;
	}
	return (1);
}

t_request	heap_pop(t_heap *heap, t_scheduler type)
{
	t_request	root;

	root = heap->requests[0];
	heap->size--;
	if (heap->size > 0)
	{
		heap->requests[0] = heap->requests[heap->size];
		heapify_down(heap, 0, type);
	}
	return (root);
}
