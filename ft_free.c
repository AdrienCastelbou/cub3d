/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_free.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acastelb <acastelb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/19 15:22:49 by acastelb          #+#    #+#             */
/*   Updated: 2021/01/19 15:24:59 by acastelb         ###   ########.fr       */
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
	free(cub->player);
	free_map(cub->map);
	mlx_destroy_image(cub->mlx, cub->no);
	mlx_destroy_image(cub->mlx, cub->so);
	mlx_destroy_image(cub->mlx, cub->we);
	mlx_destroy_image(cub->mlx, cub->ea);
	mlx_destroy_image(cub->mlx, cub->s);
	mlx_destroy_window(cub->mlx, cub->win);
	exit(0);
	return (0);
}

int		free_strs(char *s1, char *s2)
{
	free(s1);
	free(s2);
	return (0);
}
