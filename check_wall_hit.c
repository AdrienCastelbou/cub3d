/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   check_wall_hit.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acastelb <acastelb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/19 16:31:56 by acastelb          #+#    #+#             */
/*   Updated: 2021/01/19 16:31:57 by acastelb         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

double		check_hrztl_hit_by_step(t_infos *cub, t_player *player,
		t_ray *ray, t_hrztl_hit_checker checker)
{
	while (is_in_the_grid(cub, checker.yintercept, checker.xintercept))
	{
		if (cub->map[checker.yintercept / tile_size]
				[(int)checker.xintercept / tile_size] == '1')
		{
			ray->wall_hitX = checker.xintercept;
			ray->wall_hitY = checker.yintercept;
			return (get_distance(player->x, player->y,
						checker.xintercept, checker.yintercept));
		}
		checker.yintercept += checker.ystep;
		checker.xintercept += checker.xstep;
	}
	return (INT_MAX);
}

double		check_horizontal_hit(t_ray *ray, t_player *player, t_infos *cub)
{
	t_hrztl_hit_checker	checker;

	checker.yintercept = floor((player->y) / tile_size) * tile_size;
	if (ray->is_go_down == 1)
		checker.yintercept += tile_size;
	checker.xintercept = (player->x + ((double)checker.yintercept - player->y)
			/ tan(ray->ray_angle));
	checker.ystep = tile_size;
	if (ray->is_go_down == 0)
	{
		checker.ystep *= -1;
		checker.yintercept -= 1;
	}
	checker.xstep = (double)tile_size / tan(ray->ray_angle);
	if ((ray->is_go_left && checker.xstep > 0) ||
			(!ray->is_go_left && checker.xstep < 0))
		checker.xstep *= -1;
	return (check_hrztl_hit_by_step(cub, player, ray, checker));
}

double		check_vrtcl_hit_by_step(t_infos *cub, t_player *player,
		t_ray *ray, t_vrtcl_hit_checker checker)
{
	while (is_in_the_grid(cub, checker.yintercept, checker.xintercept))
	{
		if (cub->map[(int)checker.yintercept / tile_size]
				[checker.xintercept / tile_size] == '1')
		{
			checker.distance = get_distance(player->x, player->y,
					checker.xintercept, checker.yintercept);
			if (checker.distance < ray->distance)
			{
				ray->wall_hitX = checker.xintercept;
				ray->wall_hitY = checker.yintercept;
			}
			return (checker.distance);
		}
		checker.yintercept += checker.ystep;
		checker.xintercept += checker.xstep;
	}
	return (INT_MAX);
}

double		check_vertical_hit(t_ray *ray, t_player *player, t_infos *cub)
{
	t_vrtcl_hit_checker	checker;

	checker.xintercept = floor((player->x) / tile_size) * tile_size;
	if (ray->is_go_left == 0)
		checker.xintercept += tile_size;
	checker.yintercept = (player->y + ((double)checker.xintercept - player->x)
			* tan(ray->ray_angle));
	checker.xstep = tile_size;
	if (ray->is_go_left)
	{
		checker.xstep *= -1;
		checker.xintercept -= 1;
	}
	checker.ystep = tile_size * tan(ray->ray_angle);
	if ((!ray->is_go_down && checker.ystep > 0) ||
			(ray->is_go_down && checker.ystep < 0))
		checker.ystep *= -1;
	return (check_vrtcl_hit_by_step(cub, player, ray, checker));
}

void		get_wall_position(t_ray *ray, t_player *player, t_infos *cub)
{
	double	horizontal_hit;
	double	vertical_hit;

	horizontal_hit = check_horizontal_hit(ray, player, cub);
	ray->distance = horizontal_hit;
	vertical_hit = check_vertical_hit(ray, player, cub);
	if (horizontal_hit < vertical_hit)
		ray->distance = horizontal_hit;
	else
		ray->distance = vertical_hit;
}
