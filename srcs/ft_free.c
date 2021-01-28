/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_free.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acastelb <acastelb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/19 15:22:49 by acastelb          #+#    #+#             */
/*   Updated: 2021/01/27 12:04:20 by acastelb         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/cub3d.h"

int		free_map(char **map)
{
	int i;

	i = -1;
	while (map[++i])
		free(map[i]);
	free(map);
	return (0);
}

int		free_textures(t_infos *cub, t_data *textures[])
{
	int i;

	i = -1;
	while (++i < 4)
		mlx_destroy_image(cub->mlx, textures[i]);
	return (0);
}

void	free_sprites(t_sprite **sprites)
{
	int i;

	i = -1;
	while (sprites[++i])
		free(sprites[i]);
	free(sprites);
}

int		free_and_quit(t_infos *cub)
{
	if (cub->player)
	free(cub->player);
	if (cub->map)
		free_map(cub->map);
	free_textures(cub, cub->textures);
	if (cub->sprites_nb)
		free_sprites(cub->sprites);
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
