/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   draw_map.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acastelb <acastelb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/19 15:47:03 by acastelb          #+#    #+#             */
/*   Updated: 2021/01/27 16:42:02 by acastelb         ###   ########.fr       */
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

void		switch_sprites(t_sprite **s1, t_sprite **s2)
{
	t_sprite *tmp;

	tmp = *s1;
	*s1 = *s2;
	*s2 = tmp;
}

void		sort_sprites(int nb, t_sprite **sprites)
{
	int		i;
	int		j;
	t_sprite *tmp;

	i = -1;	
	while (i++ < nb - 1)
	{
		j = i;
		while (++j < nb)
			if (sprites[i]->distance < sprites[j]->distance)
			{
				tmp = sprites[i];
				sprites[i] = sprites[j];
				sprites[j] = tmp;
			}
	}
}

double		get_sprite_angle(t_player *player, double rot_angle, double sx, double sy)
{
	double	x_vector;
	double y_vector;
	double	angle;
	double	s_angle;

	x_vector = sx - player->x;
	y_vector = sy - player->y ;
	angle = atan2(y_vector, x_vector);
	s_angle = angle - rot_angle;
	return (s_angle);
}

int	ya = 0;

double		ft_abs(double n)
{
	if (n < 0)
		return (n * -1);
	return (n);
}

int			is_visible(double sprite_angle, double fov)
{
	if (sprite_angle < -M_PI)
		sprite_angle += 2 * M_PI;
	if (sprite_angle >= M_PI)
		sprite_angle -= 2 * M_PI;
	return (ft_abs(sprite_angle) < ((fov / 2.0)));
return (1);
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

int c;

void		draw_sprite(t_infos *cub, t_sprite *sprite, int size, int x, int y)
{
	int	i;
	int	j;
	int	start;
	int	offset_x;
	int	offset_y;
	int color;

	j = -1;
	while (++j + x < 0)
		;
	i = -1;
	while (++i + y < 0)
		;
	start = i;
	while (j + x < cub->r[0] && j < size)
	{
		offset_x = (j) * ((double) tile_size / size);
		i = start - 1;
		while (++i + y < cub->r[1] && i < size)
		{
			offset_y = ((i) * ((double) tile_size / size));
			color =  get_text_color((int *)cub->s->addr, tile_size * offset_y + offset_x);
			if (sprite->distance < cub->rays[x + j].distance && color != cub->s_transparency)
				my_mlx_pixel_put(cub->img, x + j, y + i, color);
		}
		j++;
	}
}

void		get_sprites_place(t_infos *cub, t_sprite **sprites)
{
	int		i;
	double	h1;
	int		start_top;
	int		start_left;

	i = -1;
	while (++i < cub->sprites_nb)
	{
		if (sprites[i]->is_visible)
		{
			h1 = (tile_size / sprites[i]->distance) * cub->proj_plane_dist;
			start_top = (cub->r[1] / 2) - (h1 / 2);
			start_left = tan(sprites[i]->angle) * cub->proj_plane_dist + (cub->r[0] / 2) - (h1 / 2);
			draw_sprite(cub, sprites[i], h1, start_left, start_top);
		}
	}
}

void		sprite_casting(t_infos *cub)
{
	int i;

	i = -1;
	while (++i < cub->sprites_nb)
	{
		cub->sprites[i]->distance = get_distance(cub->player->x,
				cub->player->y,
				cub->sprites[i]->x * tile_size,
				cub->sprites[i]->y * tile_size);
		cub->sprites[i]->angle = get_sprite_angle(cub->player, ft_abs_angle(cub->player->rotation_angle), cub->sprites[i]->x * tile_size, cub->sprites[i]->y * tile_size
);
		cub->sprites[i]->is_visible = is_visible(cub->sprites[i]->angle, cub->player->fov_angle);

	}
	sort_sprites(cub->sprites_nb, cub->sprites);
	get_sprites_place(cub, cub->sprites);
}

void		draw_3d_map(t_infos *cub, t_data *img)
{
	int		i;
	t_ray	*ray;
	double	proj_plane_dist;
	double	wall_height;

	i = -1;
	while (++i < cub->num_rays)
	{
		ray = &(cub->rays[i]);
		wall_height = (tile_size / ray->distance) * cub->proj_plane_dist;
		draw_wall(cub, i, (cub->r[1] / 2) -
				(wall_height / 2), wall_height, ray);
	}
	sprite_casting(cub);
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
	cub->proj_plane_dist = (cub->r[0] / 2) / tan(cub->player->fov_angle / 2);
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
