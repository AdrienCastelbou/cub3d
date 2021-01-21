/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   draw_minimap.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acastelb <acastelb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/19 15:51:50 by acastelb          #+#    #+#             */
/*   Updated: 2021/01/21 15:57:18 by acastelb         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

void		my_mlx_pixel_put(t_data *data, int x, int y, int color)
{
	char	*dst;

	dst = data->addr + (y * data->line_length + x * (data->bits_per_pixel / 8));
	*(unsigned int*)dst = color;
}

void		put_tile(double tile_x, double tile_y, int wall, t_data *img)
{
	int	i;
	int	j;
	int	color;

	i = 0;
	j = 0;
	if (wall == 1)
		color = 0x002B0F89;
	else if (wall == 2)
		color = 0x000A6B0A;
	else
		color = 0x00000000;
	while (i < tile_size * minimap_scale)
	{
		j = -1;
		while (++j < tile_size * minimap_scale)
			my_mlx_pixel_put(img, tile_x + i, tile_y + j, color);
		i++;
	}
}

void		draw_grid(t_infos cub, t_data *img)
{
	int	i;
	int	j;
	int	tile_x;
	int	tile_y;

	tile_x = 0;
	tile_y = 0;
	i = 0;
	while (cub.map[i])
	{
		j = 0;
		while (cub.map[i][j])
		{
			tile_x = j * tile_size;
			tile_y = i * tile_size;
			if (cub.map[i][j] == '1')
				put_tile(tile_x * minimap_scale, tile_y * minimap_scale,
					1, img);
			else if (cub.map[i][j] != ' ')
				put_tile(tile_x * minimap_scale, tile_y * minimap_scale,
					cub.map[i][j] - '0', img);
			j++;
		}
		i++;
	}
}

void		draw_rays(t_infos *cub, t_data *img)
{
	int		i;
	t_ray	*ray;

	i = -1;
	while (++i < cub->num_rays)
	{
		ray = &(cub->rays[i]);
		my_mlx_pixel_put(img, minimap_scale * ray->wall_hitX, minimap_scale
				* ray->wall_hitY, 0x00FF0000);
	}
}

void		draw_player(t_infos cub, t_data *img)
{
	int			x;
	int			y;
	int			height;
	t_player	*player;

	player = cub.player;
	x = -player->radius;
	while (x < player->radius)
	{
		height = (int)sqrt(player->radius * player->radius - x * x);
		y = -height;
		while (y < height)
		{
			my_mlx_pixel_put(img, minimap_scale * (player->x + x),
					minimap_scale * (player->y + y), 0x00FF0000);
			y++;
		}
		x++;
	}
}
