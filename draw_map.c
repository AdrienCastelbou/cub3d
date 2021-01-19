/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   draw_map.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acastelb <acastelb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/19 15:47:03 by acastelb          #+#    #+#             */
/*   Updated: 2021/01/19 15:49:45 by acastelb         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

int			get_color(int color[])
{
	return (color[0] << 16 | color[1] << 8 | color[2]);
}

void		draw_wall(t_infos *cub, int x, int y, int height)
{
	int		i;

	i = -1;
	if (y < 0)
		y = 0;
	while (++i < y)
		my_mlx_pixel_put(cub->img, x, i, cub->c);
	i = -1;
	while (++i + y < cub->r[1])
		if (i < height)
			my_mlx_pixel_put(cub->img, x, y + i, 0x00FFFFFF);
		else
			my_mlx_pixel_put(cub->img, x, y + i, cub->f);
}

void		draw_3d_map(t_infos *cub, t_data *img)
{
	int		i;
	t_ray	*ray;
	double	proj_plane_dist;
	double	wall_height;
	double	correct;

	i = -1;
	while (++i < cub->num_rays)
	{
		ray = &(cub->rays[i]);
		correct = ray->distance * cos(ray->ray_angle -
				cub->player->rotation_angle);
		proj_plane_dist = (cub->r[0] / 2) / tan(cub->player->fov_angle / 2);
		wall_height = (tile_size / correct) * proj_plane_dist;
		draw_wall(cub, i, (cub->r[1] / 2) -
				(wall_height / 2), wall_height);
	}
}

void		draw_map(t_infos *cub, t_data *img)
{
	mlx_destroy_image(cub->mlx, cub->img->img);
	img->img = mlx_new_image(cub->mlx, cub->r[0], cub->r[1]);
	img->addr = mlx_get_data_addr(img->img, &img->bits_per_pixel,
			&img->line_length, &img->endian);
	raycast(cub);
	draw_3d_map(cub, img);
	draw_grid(*cub, img);
	draw_player(*cub, img);
	draw_rays(cub, img);
	mlx_put_image_to_window(cub->mlx, cub->win, img->img, 0, 0);
}

int			render_next_frame(t_infos *cub)
{
	int movestep;

	mlx_hook(cub->win, 17, 1L << 17, free_and_quit, cub);
	mlx_hook(cub->win, 2, 1L << 0, key_hook, cub);
	mlx_hook(cub->win, 3, 1L << 1, key_release_hook, cub);
	cub->player->rotation_angle += cub->player->turn_direction *
		cub->player->rotation_speed;
	movestep = cub->player->walk_direction * cub->player->move_speed;
	if (check_collisions(movestep, cub))
		return (1);
	cub->player->x += cos(cub->player->rotation_angle +
			cub->player->lateral_move) * movestep;
	cub->player->y += sin(cub->player->rotation_angle +
			cub->player->lateral_move) * movestep;
	draw_map(cub, (cub->img));
	return (1);
}
