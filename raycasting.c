/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   raycasting.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acastelb <acastelb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/19 15:43:21 by acastelb          #+#    #+#             */
/*   Updated: 2021/01/21 17:19:17 by acastelb         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

t_ray		*ray_init(double ray_angle, t_ray *ray)
{
	ray->ray_angle = ft_abs_angle(ray_angle);
	ray->distance = 10;
	ray->wall_hitX = -1;
	ray->wall_hitY = -1;
	ray->is_go_down = 0;
	ray->is_go_left = 0;
	ray->is_vrtcl_hit = 0;
	if (ray->ray_angle > 0 && ray->ray_angle < M_PI)
		ray->is_go_down = 1;
	if (ray->ray_angle > (0.5 * M_PI) && ray->ray_angle < 1.5 * M_PI)
		ray->is_go_left = 1;
	ray->object_hit = 0;
	ray->obj_ray = malloc(sizeof(t_ray));
	ray->obj_ray->distance = INT_MAX;
	return (ray);
}
/*
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
}*/
