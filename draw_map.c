/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   draw_map.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acastelb <acastelb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/19 15:47:03 by acastelb          #+#    #+#             */
/*   Updated: 2021/01/21 16:42:11 by acastelb         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

int			get_color(int color[])
{
	return (color[0] << 16 | color[1] << 8 | color[2]);
}

int			get_text_color(int *add, int pos)
{
	return (add[pos]);
}

void		draw_wall(t_infos *cub, int x, int y, int height, t_ray *ray)
{
	int			i;
	int			offset_x;
	int			offset_y;
	int			real_y;

	i = -1;
	real_y = y;
	if (y < 0)
		y = 0;
	while (++i < y)
		my_mlx_pixel_put(cub->img, x, i, cub->c);
	i = -1;
	if (ray->is_vrtcl_hit)
		offset_x = (int)ray->wall_hitY % tile_size;
	else
		offset_x = (int)ray->wall_hitX % tile_size;
	while (++i + y < cub->r[1])
		if (i < height)
		{
			if (real_y < 0)
				offset_y = ((i - real_y) * ((double) tile_size / height));
			else
				offset_y = ((i) * ((double) tile_size / height));
			my_mlx_pixel_put(cub->img, x, y + i,
					get_text_color((int *)cub->textures[ray->side_hit]->addr, tile_size * offset_y + offset_x));
		}
		else
			my_mlx_pixel_put(cub->img, x, y + i, cub->f);
}

void		draw_sprite(t_infos *cub, int x, int y, int height, t_ray *ray)
{
	int	i;
	int	real_y;
	int color;
	int			offset_x;
	int			offset_y;

	real_y = y;
	if (y < 0)
		y = 0;
	i = -1;
	if (ray->obj_ray->is_vrtcl_hit)
		offset_x = (int)ray->obj_ray->wall_hitY % tile_size;
	else
		offset_x = (int)ray->obj_ray->wall_hitX % tile_size;
	while (++i + y < cub->r[1] && i < height)
	{
		if (real_y < 0)
			offset_y = ((i - real_y) * ((double) tile_size / height));
		color = get_text_color((int *)cub->s->addr, tile_size * offset_y + offset_x);
		if (color != cub->s_transparency)
			my_mlx_pixel_put(cub->img, x, y + i, color);
	}
}

void		draw_3d_sprite(t_infos *cub, t_data *img, t_ray *ray, int position)
{
	double	proj_plane_dist;
	double	sprite_height;
	double	correct;

	correct = ray->obj_ray->distance * cos(ray->ray_angle -
			cub->player->rotation_angle);
	proj_plane_dist = (cub->r[0] / 2) / tan(cub->player->fov_angle / 2);
	sprite_height = (tile_size / correct) * proj_plane_dist;
	draw_sprite(cub, position, (cub->r[1] / 2) -
			(sprite_height / 2), sprite_height, ray);

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
				(wall_height / 2), wall_height, ray);
		if (ray->object_hit)
			draw_3d_sprite(cub, img, ray, i);
		free(ray->obj_ray);
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

int			launch_game(t_infos *cub)
{
	t_data	img;
	t_ray	rays[cub->r[0]];

	cub->win = mlx_new_window(cub->mlx, cub->r[0], cub->r[1], "cub3d");
	cub->player = player_init(cub);
	cub->num_rays = cub->r[0];
	cub->rays = rays;
	cub->img = &img;
	img.img = mlx_new_image(cub->mlx, cub->r[0], cub->r[1]);
	img.addr = mlx_get_data_addr(img.img, &img.bits_per_pixel, &img.line_length,
			&img.endian);
	draw_map(cub, &img);
	mlx_loop_hook(cub->mlx, render_next_frame, cub);
	mlx_loop(cub->mlx);
	return (1);
}
