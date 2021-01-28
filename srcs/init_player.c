/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_player.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acastelb <acastelb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/19 15:56:03 by acastelb          #+#    #+#             */
/*   Updated: 2021/01/28 14:05:34 by acastelb         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/cub3d.h"

double		get_start_angle(char c)
{
	if (c == 'S')
		return (M_PI_2);
	else if (c == 'W')
		return (M_PI);
	else if (c == 'E')
		return (0);
	return ((3 * M_PI) / 2);
}

void		get_player_infos(t_infos *cub, t_player *player)
{
	int		i;
	int		j;

	i = -1;
	while (cub->map[++i])
	{
		j = -1;
		while (cub->map[i][++j])
			if (ft_strchr("SNEW", cub->map[i][j]))
			{
				player->x = (j + 0.5) * tile_size;
				player->y = (i + 0.5) * tile_size;
				player->rotation_angle = get_start_angle(cub->map[i][j]);
				cub->map[i][j] = '0';
			}
	}
}

t_player	*player_init(t_infos *cub)
{
	t_player	*player;

	if (!(player = malloc(sizeof(t_player))))
		return (NULL);
	player->radius = 3;
	player->turn_direction = 0;
	player->walk_direction = 0;
	player->move_speed = 3.0;
	player->rotation_speed = 2 * (M_PI / 180);
	get_player_infos(cub, player);
	player->lateral_move = 0;
	player->fov_angle = 60 * (M_PI / 180);
	return (player);
}
