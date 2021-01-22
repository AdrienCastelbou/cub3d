/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   check_wall_hit.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acastelb <acastelb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/19 16:31:56 by acastelb          #+#    #+#             */
/*   Updated: 2021/01/22 11:25:41 by acastelb         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

void		hrztl_object_hit(t_infos *cub, t_player *player,
		t_ray *ray, t_hrztl_hit_checker checker)
{
	t_ray *obj_ray;

	obj_ray = ray->obj_ray;
	obj_ray->wall_hitX = checker.xintercept;
	obj_ray->wall_hitY = checker.yintercept;
	obj_ray->distance = get_distance(player->x, player->y,
						checker.xintercept, checker.yintercept);
	obj_ray->is_vrtcl_hit = 0;
	ray->object_hit = 1;

}

void		vrtcl_object_hit(t_infos *cub, t_player *player,
		t_ray *ray, t_vrtcl_hit_checker checker)
{
	t_ray *obj_ray;

	checker.distance = get_distance(player->x, player->y,
						checker.xintercept, checker.yintercept);
	if ((checker.distance < ray->obj_ray->distance &&
			checker.distance < ray->distance) || !ray->object_hit)
	{
		obj_ray = ray->obj_ray;
		obj_ray->wall_hitX = checker.xintercept;
		obj_ray->wall_hitY = checker.yintercept;
		obj_ray->distance = checker.distance;
		obj_ray->is_vrtcl_hit = 1;
		ray->object_hit = 1;
	}
}

double		check_hrztl_hit_by_step(t_infos *cub, t_player *player,
		t_ray *ray, t_hrztl_hit_checker checker)
{
	int	y_to_check;

	while (is_in_the_grid(cub, checker.yintercept, checker.xintercept))
	{
		y_to_check = checker.yintercept;
		if (!ray->is_go_down)
			y_to_check -= 1;
		if (cub->map[y_to_check / tile_size]
				[(int)checker.xintercept / tile_size] == '1')
		{
			ray->wall_hitX = checker.xintercept;
			ray->wall_hitY = checker.yintercept;
			return (get_distance(player->x, player->y,
						checker.xintercept, checker.yintercept));
		}
		else if (cub->map[y_to_check / tile_size]
				[(int)checker.xintercept / tile_size] == '2')
			hrztl_object_hit(cub, player, ray, checker);
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
		checker.ystep *= -1;
	checker.xstep = (double)tile_size / tan(ray->ray_angle);
	if ((ray->is_go_left && checker.xstep > 0) ||
			(!ray->is_go_left && checker.xstep < 0))
		checker.xstep *= -1;
	return (check_hrztl_hit_by_step(cub, player, ray, checker));
}

double		check_vrtcl_hit_by_step(t_infos *cub, t_player *player,
		t_ray *ray, t_vrtcl_hit_checker checker)
{
	int	x_to_check;

	while (is_in_the_grid(cub, checker.yintercept, checker.xintercept))
	{
		x_to_check = checker.xintercept;
		if (ray->is_go_left)
			x_to_check -= 1;
		if (cub->map[(int)checker.yintercept / tile_size]
				[x_to_check / tile_size] == '1')
		{
			checker.distance = get_distance(player->x, player->y,
					checker.xintercept, checker.yintercept);
			if (checker.distance < ray->distance)
			{
				ray->wall_hitX = checker.xintercept;
				ray->wall_hitY = checker.yintercept;
				ray->is_vrtcl_hit = 1;
			}
			return (checker.distance);
		}
		else if (cub->map[(int)checker.yintercept / tile_size]
				[x_to_check / tile_size] == '2')
			vrtcl_object_hit(cub, player, ray, checker);
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
		checker.xstep *= -1;
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
	if (vertical_hit < horizontal_hit && ray->object_hit && ray->obj_ray->is_vrtcl_hit)
		ray->object_hit = 0;
	if (ray->is_vrtcl_hit && ray->is_go_left)
		ray->side_hit = 3;
	else if (ray->is_vrtcl_hit && !ray->is_go_left)
		ray->side_hit = 2;
	else if (!ray->is_vrtcl_hit && ray->is_go_down)
		ray->side_hit = 1;
	else
		ray->side_hit = 0;
}

void		raycast(t_infos *cub)
{
	t_ray	*ray;
	double	ray_angle;
	int		i;

	i = -1;
	ray_angle = cub->player->rotation_angle - (cub->player->fov_angle / 2);
	while (++i < cub->num_rays)
	{
		ray_init(ray_angle, &cub->rays[i]);
		ray = &(cub->rays[i]);
		get_wall_position(ray, cub->player, cub);
		ray_angle += cub->player->fov_angle / (double)cub->num_rays;
	}
}
