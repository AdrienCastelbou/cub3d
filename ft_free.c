/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_free.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acastelb <acastelb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/19 15:22:49 by acastelb          #+#    #+#             */
/*   Updated: 2021/01/20 10:42:41 by acastelb         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

int		free_map(char **map)
{
	int i;

	i = -1;
	while (map[++i])
		free(map[i]);
	free(map);
	return (0);
}

int		free_and_quit(t_infos *cub)
{
	if (cub->player)
	free(cub->player);
	if (cub->map)
		free_map(cub->map);
	if (cub->no)
		mlx_destroy_image(cub->mlx, cub->no);
	if (cub->so)
		mlx_destroy_image(cub->mlx, cub->so);
	if (cub->we)
		mlx_destroy_image(cub->mlx, cub->we);
	if (cub->ea)
		mlx_destroy_image(cub->mlx, cub->ea);
	if (cub->s)
		mlx_destroy_image(cub->mlx, cub->s);
	if (cub->win)
		mlx_destroy_window(cub->mlx, cub->win);
	exit(0);
	return (0);
}

int		free_strs(char *s1, char *s2)
{
	if (s1)
		free(s1);
	if (s2)
		free(s2);
	return (0);
}
