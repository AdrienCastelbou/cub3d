/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_2.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acastelb <acastelb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/19 16:03:55 by acastelb          #+#    #+#             */
/*   Updated: 2021/01/20 14:32:53 by acastelb         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

int			is_in_the_grid(t_infos *cub, double yintercept, double xintercept)
{
	int y;
	int x;

	if (yintercept < 0 || xintercept < 0)
		return (0);
	x = -1;
	y = -1;
	while (cub->map[++y])
		;
	if (yintercept / tile_size >= y)
		return (0);
	y = (int)yintercept / tile_size;
	while (cub->map[y][++x])
		;
	if (xintercept / tile_size >= x)
		return (0);
	return (1);
}

int			key_hook(int keycode, t_infos *cub)
{
	if (keycode == 126)
		cub->player->walk_direction = 1;
	else if (keycode == 125)
		cub->player->walk_direction = -1;
	else if (keycode == 123)
		cub->player->turn_direction = -1;
	else if (keycode == 124)
		cub->player->turn_direction = 1;
	else if (keycode == 53)
		free_and_quit(cub);
	else if (keycode == 0)
	{
		cub->player->walk_direction = 1;
		cub->player->lateral_move = -M_PI_2;
	}
	else if (keycode == 2)
	{
		cub->player->walk_direction = 1;
		cub->player->lateral_move = M_PI_2;
	}
	return (1);
}

int			key_release_hook(int keycode, t_infos *cub)
{
	if (keycode == 126)
		cub->player->walk_direction = 0;
	else if (keycode == 125)
		cub->player->walk_direction = 0;
	else if (keycode == 123)
		cub->player->turn_direction = 0;
	else if (keycode == 124)
		cub->player->turn_direction = 0;
	else if (keycode == 0)
	{
		cub->player->walk_direction = 0;
		cub->player->lateral_move = 0;
	}
	else if (keycode == 2)
	{
		cub->player->walk_direction = 0;
		cub->player->lateral_move = 0;
	}
	return (1);
}

int			check_collisions(int movestep, t_infos *cub)
{
	int		next_x;
	int		next_y;

	next_x = cub->player->x + cos(cub->player->rotation_angle +
			cub->player->lateral_move) * movestep;
	next_y = cub->player->y + sin(cub->player->rotation_angle +
			cub->player->lateral_move) * movestep;
	if (cub->map[next_y / tile_size][next_x / tile_size] != '1')
		return (0);
	return (1);
}
/*
int			launch_game(t_infos *cub)
{
	t_data	img;
	t_ray	rays[cub->r[0]];

	cub->win = mlx_new_window(cub->mlx, cub->r[0], cub->r[1], "cub3d");
	cub->player = player_init(cub);
	cub->num_rays = cub->r[0];
	cub->rays = rays;
	cub->img = &img;
	cub->texture = init_texture();
	img.img = mlx_new_image(cub->mlx, cub->r[0], cub->r[1]);
	img.addr = mlx_get_data_addr(img.img, &img.bits_per_pixel, &img.line_length,
			&img.endian);
	draw_map(cub, &img);
	mlx_loop_hook(cub->mlx, render_next_frame, cub);
	mlx_loop(cub->mlx);
	return (1);
}*/
