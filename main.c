/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acastelb <acastelb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/21 11:17:47 by acastelb          #+#    #+#             */
/*   Updated: 2021/01/14 15:29:49 by acastelb         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "cub3d.h"

void		my_mlx_pixel_put(t_data *data, int x, int y, int color)
{
    char	*dst;

	dst = data->addr + (y * data->line_length + x * (data->bits_per_pixel / 8));
	*(unsigned int*)dst = color;
}

void		put_tile(double TileX, double TileY, int Wall, t_data *img)
{
	int	i;
	int	j;

	i = 0;
	j = 0;

	while (i < tile_size * minimap_scale)
	{
		j = 0;
		while (j < tile_size * minimap_scale)
		{
			if (Wall)
				my_mlx_pixel_put(img, TileX + i, TileY + j, 0x002B0F89);
			else
				my_mlx_pixel_put(img, TileX + i, TileY + j, 0x00000000);
			j++;
		}
		i++;
	}
}

void		draw_grid(t_vars vars, t_data *img)
{
	int	i;
	int	j;
	int	TileX;
	int	TileY;

	TileX = 0;
	TileY = 0;
	i = 0;
	while (i < map_rows)
	{
		j = 0;
		while (j < map_cols)
		{
			TileX = j * tile_size;
			TileY = i * tile_size;
			put_tile(TileX  * minimap_scale, TileY * minimap_scale, grid[i][j], img);
			j++;
		}
		i++;
	}
}

t_player	*player_init(void)
{
	t_player	*player;

	if (!(player = malloc(sizeof(t_player))))
		return (NULL);
	player->x = win_width / 2;
	player->y = win_height / 2;
	player->radius = 3;
	player->turn_direction = 0;
	player->walk_direction = 0;
	player->rotation_angle = M_PI_2;
	player->move_speed = 3.0;
	player->rotation_speed = 2 * (M_PI / 180);
	return (player);
}


void		draw_player(t_vars vars, t_data *img)
{
	int			x;
	int			y;
	int			height;
	t_player	*player;

	player = vars.player;
	x = -player->radius;
	while (x < player->radius)
	{
		height = (int)sqrt(player->radius * player->radius - x * x);
		y = -height;
		while (y < height)
		{
			my_mlx_pixel_put(img, minimap_scale * (player->x + x), minimap_scale * (player->y + y), 0x00FF0000);
			y++;
		}
		x++;
	}
	my_mlx_pixel_put(img, minimap_scale * (player->x + cos(player->rotation_angle) * 10), minimap_scale * (player->y + sin(player->rotation_angle) * 10), 0x00FFFF00);
}

double		ft_dmod(double x, double y)
{
	return (x - (int)(x/y) * y);
}

double		ft_abs_angle(double ray_angle)
{
	ray_angle = ft_dmod(ray_angle, (2 * M_PI));
	if (ray_angle < 0)
		ray_angle += (2 * M_PI);
	return (ray_angle);
}

int			get_int_value(double nb)
{
	double	residue;

	residue = nb - (int)nb;
	if (residue <= 0.5)
		return (floor(nb));
	return (floor(nb) + 1);
}

int			is_in_the_grid(double yintercept, double xintercept)
{
	if (yintercept / tile_size > map_rows - 1 || yintercept < 0)
		return (0);
	if (xintercept / tile_size > map_cols - 1 || xintercept < 0)
		return (0);
	return (1);
}

double		get_distance(double x1, double y1, double x2, double y2)
{
	return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

double		check_horizontal_hit(t_ray *ray, t_player *player, t_vars *vars)
{
	int		ystep;
	double	xstep;
	double	xintercept;
	int		yintercept;
	double	distance;

	yintercept = floor((player->y) / tile_size) * tile_size;
	if (ray->is_go_down == 1)
		yintercept += tile_size;
	xintercept = (player->x + ((double)yintercept - player->y) / tan(ray->ray_angle));
	ystep = tile_size;
	if (ray->is_go_down == 0)
	{
		ystep *= -1;
		yintercept -= 1;
	}
	xstep = (double)tile_size / tan(ray->ray_angle);
	if ((ray->is_go_left && xstep > 0) || (!ray->is_go_left && xstep < 0))
		xstep *= -1;
	while (is_in_the_grid(yintercept, xintercept))
	{
		if (grid[yintercept / tile_size][(int)xintercept / tile_size] == 1)
		{
			ray->wall_hitX = xintercept;
			ray->wall_hitY = yintercept;
			return (get_distance(player->x, player->y, xintercept, yintercept));
		}
		yintercept += ystep;
		xintercept += xstep;
	}
	return (INT_MAX);
}

double		check_vertical_hit(t_ray *ray, t_player *player, t_vars *vars)
{
	double	ystep;
	int		xstep;
	int		xintercept;
	double	yintercept;
	double	distance;

	xintercept = floor((player->x) / tile_size) * tile_size;
	if (ray->is_go_left == 0)
		xintercept += tile_size;
	yintercept = (player->y + ((double)xintercept - player->x) * tan(ray->ray_angle));
	xstep = tile_size;
	if (ray->is_go_left)
	{
		xstep *= -1;
		xintercept -= 1;
	}
	ystep = tile_size * tan(ray->ray_angle);
	if ((!ray->is_go_down && ystep > 0) || (ray->is_go_down && ystep < 0))
		ystep *= -1;
	while (is_in_the_grid(yintercept, xintercept))
	{
		if (grid[(int)yintercept / tile_size][xintercept / tile_size] == 1)
		{
			distance = get_distance(player->x, player->y, xintercept, yintercept);
			if (distance < ray->distance)
			{
				ray->wall_hitX = xintercept;
				ray->wall_hitY = yintercept;
			}
			return (distance);
		}
		yintercept += ystep;
		xintercept += xstep;
	}
	return (INT_MAX);
}

void		get_wall_position(t_ray *ray, t_player *player, t_vars *vars)
{
	double	horizontal_hit;
	double	vertical_hit;

	horizontal_hit = check_horizontal_hit(ray, player, vars);
	ray->distance = horizontal_hit;
	vertical_hit = check_vertical_hit(ray, player, vars);
	if (horizontal_hit < vertical_hit)
		ray->distance = horizontal_hit;
	else
		ray->distance = vertical_hit;
}

t_ray		*ray_init(double ray_angle, t_ray *ray)
{
	ray->ray_angle = ft_abs_angle(ray_angle);;
	ray->distance = 10;
	ray->wall_hitX = -1;
	ray->wall_hitY = -1;
	ray->is_go_down = 0;
	ray->is_go_left = 0;
	if (ray->ray_angle > 0 && ray->ray_angle < M_PI)
		ray->is_go_down = 1;
	if (ray->ray_angle > (0.5 * M_PI) && ray->ray_angle < 1.5 * M_PI)
		ray->is_go_left = 1;
	return (ray);
}

void		draw_rays(t_vars *vars, t_data *img)
{
	int		i;
	t_ray	*ray;

	i = -1;
	while (++i < num_rays)
	{
		ray = &(vars->rays[i]);
		my_mlx_pixel_put(img, minimap_scale * ray->wall_hitX, minimap_scale * ray->wall_hitY, 0x00FF0000);
	}
}

void		raycast(t_vars *vars)
{
	t_ray	*ray;
	double	ray_angle;
	int		i;
	
	i = -1;
	ray_angle = vars->player->rotation_angle - (fov_angle / 2);
	while (++i < num_rays)
	{
		ray_init(ray_angle, &(vars->rays[i]));
		ray =  &(vars->rays[i]);
		get_wall_position(ray, vars->player, vars);
		ray_angle += fov_angle / (double)num_rays;
	}
}

void		draw_wall(t_data *img, int x, int y, int width, int height)
{
	int		i;
	int		j;

	i = -1;
	if (y < 0)
		y = 0;
	while (++i < height && y + i < win_height)
	{
		j = -1;
		while (++j < width)
			my_mlx_pixel_put(img, x + j, y + i, 0x00FFFFFF);
	}
}

void		draw_3D_map(t_vars *vars, t_data *img)
{
	int		i;
	t_ray	*ray;
	double	ray_distance;
	double	proj_plane_dist;
	double	wall_height;

	i = -1;
	while (++i < num_rays)
	{
		ray =  &(vars->rays[i]);
		ray_distance = ray->distance;
		proj_plane_dist = (win_width / 2) / tan(fov_angle / 2);
		wall_height = (tile_size / ray_distance) * proj_plane_dist;
		draw_wall(img, i * wall_strip_width, (win_height / 2) - (wall_height / 2), wall_strip_width, wall_height);
	}
}

void		draw_map(t_vars *vars, t_data *img)
{
	mlx_destroy_image(vars->mlx, vars->img->img);
	img->img = mlx_new_image(vars->mlx, win_width, win_height);
	img->addr = mlx_get_data_addr(img->img, &img->bits_per_pixel, &img->line_length,
			&img->endian);
	raycast(vars);
	draw_3D_map(vars, img);
	draw_grid(*vars, img);
	draw_player(*vars, img);
	draw_rays(vars, img);
	mlx_put_image_to_window(vars->mlx, vars->win, img->img, 0, 0);
}


int		key_hook(int keycode, t_vars *vars)
{
	if (keycode == 126)
		vars->player->walk_direction = 1;
	else if (keycode == 125)
		vars->player->walk_direction = -1;
	else if (keycode == 123)
		vars->player->turn_direction = -1;
	else if (keycode == 124)
		vars->player->turn_direction = 1;
	else if (keycode == 53)
	{
		free(vars->player);
		mlx_destroy_window( vars->mlx, vars->win);
		exit(0);
	}
	return (1);
}

int		key_release_hook(int keycode, t_vars *vars)
{
	if (keycode == 126)
		vars->player->walk_direction = 0;
	else if (keycode == 125)
		vars->player->walk_direction = 0;
	else if (keycode == 123)
		vars->player->turn_direction = 0;
	else if (keycode == 124)
		vars->player->turn_direction = 0;
	return (1);
}
/*
int			key_hook(int keycode, t_vars *vars)
{
	if (keycode == 13)
		vars->player->walk_direction = 1;
	else if (keycode == 1)
		vars->player->walk_direction = -1;
	else if (keycode == 0)
		vars->player->turn_direction = -1;
	else if (keycode == 2)
		vars->player->turn_direction = 1;
	else if (keycode == 53)
	{
		free(vars->player);
		mlx_destroy_image(vars->mlx, vars->img->img);
		mlx_destroy_window( vars->mlx, vars->win);
		exit(0);
	}
	return (1);
}

int			key_release_hook(int keycode, t_vars *vars)
{
	if (keycode == 13)
		vars->player->walk_direction = 0;
	else if (keycode == 1)
		vars->player->walk_direction = 0;
	else if (keycode == 0)
		vars->player->turn_direction = 0;
	else if (keycode == 2)
		vars->player->turn_direction = 0;
	return (1);
}
*/
int			check_collisions(int movestep, t_player *player)
{
	int		next_x;
	int		next_y;

	next_x = player->x + cos(player->rotation_angle) * movestep;
	next_y = player->y + sin(player->rotation_angle) * movestep;
	
	if (grid[next_y / tile_size][next_x / tile_size] == 1)
		return (1);
	return (0);
}



int			render_next_frame(t_vars *vars)
{
	int movestep;

	mlx_hook(vars->win, 2, 1L<<0, key_hook, vars);
	mlx_hook(vars->win, 3, 1L<<1, key_release_hook, vars);
	vars->player->rotation_angle += vars->player->turn_direction * vars->player->rotation_speed;
	movestep = vars->player->walk_direction * vars->player->move_speed;
	if (check_collisions(movestep, vars->player))
		return (1);
	vars->player->x += cos(vars->player->rotation_angle) * movestep;
	vars->player->y += sin(vars->player->rotation_angle) * movestep;
	draw_map(vars, (vars->img));
	return (1);
}

int			main(void)
{
	t_vars	vars;
    t_data  img;

	vars.mlx = mlx_init();
	vars.win = mlx_new_window(vars.mlx, win_width, win_height, "Hello world!");
	vars.player = player_init();
	vars.img = &img;
	img.img = mlx_new_image(vars.mlx, win_width, win_height);
    img.addr = mlx_get_data_addr(img.img, &img.bits_per_pixel, &img.line_length,
          &img.endian);
	draw_map(&vars, &img);
	mlx_loop_hook(vars.mlx, render_next_frame, &vars);
	mlx_loop(vars.mlx);
}
