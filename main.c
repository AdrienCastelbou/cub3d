/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acastelb <acastelb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/21 11:17:47 by acastelb          #+#    #+#             */
/*   Updated: 2021/01/05 13:32:23 by acastelb         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "minilibx_opengl_20191021/mlx.h"

const int	tile_size = 32;
const int map_rows = 11;
const int map_cols = 15;
const int win_rows = map_rows * tile_size;
const int win_cols = map_cols * tile_size;
const double fov_angle = 60 * (M_PI / 180);
const int wall_strip_width = 1;
const int num_rays = win_cols / wall_strip_width;
const	int grid[map_rows][map_cols] = {
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1},
		{1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1},
		{1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1},
		{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1},
		{1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1},
		{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
		{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
		{1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 1},
		{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
	};

typedef struct s_ray {
	double ray_angle;
	int			distance;
	int			wall_hitX;
	int			wall_hitY;
	int			is_go_down;
	int			is_go_left;
}				t_ray;


typedef struct s_player {
	double			x;
	double			y;
	int			radius;
	int			turn_direction;
	int			walk_direction;
	double		rotation_angle;
	int			move_speed;
	double		rotation_speed;
}				t_player;

typedef struct	s_data {
	void		*img;
	char		*addr;
	int			bits_per_pixel;
	int			line_length;
	int			endian;
}				t_data;

typedef struct  s_vars {
    void        *mlx;
    void        *win;
	t_player	*player;
	t_data		*img;
}				t_vars;


void		my_mlx_pixel_put(t_data *data, int x, int y, int color)
{
    char	*dst;

	dst = data->addr + (y * data->line_length + x * (data->bits_per_pixel / 8));
	*(unsigned int*)dst = color;
}

void		put_tile(int TileX, int TileY, int Wall, t_data *img)
{
	int i;
	int j;

	i = 0;
	j = 0;

	while (i < tile_size)
	{
		j = 0;
		while (j < tile_size)
		{
			if (!Wall && ((i != 0 && i != tile_size -1) && (j != 0 && j != tile_size - 1)))
				my_mlx_pixel_put(img, TileX + i, TileY + j, 0x00FFFFFF);
			else
				my_mlx_pixel_put(img, TileX + i, TileY + j, 0x00000000);
			j++;
		}
		i++;
	}
}

void		draw_grid(t_vars vars, t_data *img)
{
	int i;
	int j;
	int TileX;
	int TileY;

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
			put_tile(TileX, TileY, grid[i][j], img);
			j++;
		}
		i++;
	}
}

t_player	*player_init(void)
{
	t_player *player;

	if (!(player = malloc(sizeof(t_player))))
		return (NULL);
	player->x = win_cols / 2;
	player->y = win_rows / 2;
	player->radius = 3;
	player->turn_direction = 0;
	player->walk_direction = 0;
	player->rotation_angle = M_PI_2;
	player->move_speed = 3.0;
	player->rotation_speed = 0.5 * (M_PI / 180);
	return (player);
}


void		draw_player(t_vars vars, t_data *img)
{
	int x;
	int y;
	int height;
	t_player *player;

	player = vars.player;
	x = -player->radius;
	while (x < player->radius)
	{
		height = (int)sqrt(player->radius * player->radius - x * x);
		y = -height;
		while (y < height)
		{
			my_mlx_pixel_put(img, player->x + x, player->y + y, 0x00FF0000);
			y++;
		}
		x++;
	}
	my_mlx_pixel_put(img, player->x + cos(player->rotation_angle) * 10, player->y + sin(player->rotation_angle) * 10, 0x00FFFF00);
}

double ft_dmod(double x, double y)
{
	return (x - (int)(x/y) * y);
}

int ft_abs(int nb)
{
	if (nb >= 0)
		return (nb);
	return (nb * -1);
}

double	ft_abs_angle(double ray_angle)
{
	ray_angle = ft_dmod(ray_angle, (2 * M_PI));
	if (ray_angle < 0)
		ray_angle += (2 * M_PI);
	return (ray_angle);
}

int		is_in_the_grid(yintercept, xintercept)
{
	if (yintercept > map_rows * tile_size || yintercept < 0)
		return (0);
	if (xintercept > map_cols * tile_size || xintercept < 0)
		return (0);
	return (1);
}

int get_int_value(double nb)
{
	int inted;
	
	inted = (int)nb;
	if (nb - inted < (inted + 1) - nb)
		return (inted);
	return (inted + 1);
}
int		check_horizontal_hit(t_ray *ray, t_player *player, t_vars vars)
{
	int	ystep;
	int xstep;
	int xintercept;
	int yintercept;

	yintercept = (get_int_value(player->y) / tile_size) * tile_size;
	xintercept = player->x + (player->y - yintercept) / tan(ray->ray_angle);
	ystep = tile_size;
	if (ray->is_go_down == 0)
	{
		ystep *= -1;
		yintercept -= 1;
	}
	else
		yintercept += tile_size;
	xstep = tile_size / tan(ray->ray_angle);
	if ((ray->is_go_left && xstep > 0) || (!ray->is_go_left && xstep < 0))
		xstep *= -1;
	while (is_in_the_grid(yintercept, xintercept))
	{
		if (grid[yintercept / tile_size][xintercept / tile_size] == 1)
		{
			my_mlx_pixel_put(vars.img, xintercept, yintercept, 0x00FF0000);
			return (1);
		}
		yintercept += ystep;
		xintercept += xstep;
	}
	return (-1);
}

int		check_vertical_hit(double ray_angle, t_player *player)
{
	int	ystep;
	int xstep;
	int xintercept;
	int yintercept;

	xstep = tile_size;
	ystep = tile_size * tan(ray_angle);
	xintercept = player->x / tile_size * tile_size;
	yintercept = player->y + (player->x - xintercept) / tan(ray_angle);
	while (1)
	{
		if (grid[yintercept / tile_size][xintercept / tile_size] == 1)
			return(ft_abs(player->x - xintercept));
		xintercept += xstep;
		yintercept += ystep;
	}
	return (0);
}

int		is_wall(t_ray *ray, t_player *player, t_vars vars)
{
	int horizontal_hit;
	int	vertical_hit;

	horizontal_hit = check_horizontal_hit(ray, player, vars);
	//vertical_hit = check_vertical_hit(ray_angle, player);
	//if (horizontal_hit > vertical_hit)
	//	return (horizontal_hit);
	//return (vertical_hit);
	ray->distance = horizontal_hit;
	return (1);
}

t_ray	*ray_init(double ray_angle)
{
	t_ray *ray;

	if (!(ray = malloc(sizeof(t_ray))))
		return (NULL);
	ray->ray_angle = ft_abs_angle(ray_angle);;
	ray->distance = 0;
	ray->wall_hitX = 0;
	ray->wall_hitY = 0;
	ray->is_go_down = 0;
	ray->is_go_left = 0;
	if (ray->ray_angle > 0 && ray->ray_angle < M_PI)
		ray->is_go_down = 1;
	if (ray->ray_angle < (3 * M_PI) / 2 && ray->ray_angle > M_PI / 2)
		ray->is_go_left = 1;
	return (ray);
}

int		draw_rays(t_vars vars, t_data *img)
{
	t_ray rays[num_rays];
	t_ray *ray;
	double ray_angle;

	ray_angle = vars.player->rotation_angle - (fov_angle / 2);
	ray = ray_init(ray_angle);
	rays[0] = *ray;
	is_wall(ray, vars.player, vars);
	//my_mlx_pixel_put(img, vars.player->x + cos(ray_angle) * ray->distance, vars.player->y + sin(ray_angle) * ray->distance, 0x00FF0000);
	free(ray);
	return (1);
}

void		draw_map(t_vars vars, t_data *img)
{
	draw_grid(vars, img);
	draw_player(vars, img);
	draw_rays(vars, img);
	mlx_put_image_to_window(vars.mlx, vars.win, img->img, 0, 0);
}

int		key_hook(int keycode, t_vars *vars)
{
	if (keycode == 13)
		vars->player->walk_direction = 1;
	else if (keycode == 1)
		vars->player->walk_direction = -1;
	else if (keycode == 2)
		vars->player->turn_direction = -1;
	else if (keycode == 0)
		vars->player->turn_direction = 1;
	return (1);
}

int		key_release_hook(int keycode, t_vars *vars)
{
	if (keycode == 13)
		vars->player->walk_direction = 0;
	else if (keycode == 1)
		vars->player->walk_direction = 0;
	else if (keycode == 2)
		vars->player->turn_direction = 0;
	else if (keycode == 0)
		vars->player->turn_direction = 0;
	return (1);
}

int		check_collisions(int movestep, t_player *player)
{
	int		next_x;
	int		next_y;

	next_x = player->x + cos(player->rotation_angle) * movestep;
	next_y = player->y + sin(player->rotation_angle) * movestep;
	
	if (grid[next_y / tile_size][next_x / tile_size] == 1)
		return (1);
	return (0);
}



int		render_next_frame(t_vars *vars)
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
	draw_map(*vars, (vars->img));
	return (1);
}

int			main(void)
{
	t_vars	vars;
    t_data  img;

	vars.mlx = mlx_init();
	vars.win = mlx_new_window(vars.mlx, win_cols, win_rows, "Hello world!");
	vars.player = player_init();
	vars.img = &img;
	img.img = mlx_new_image(vars.mlx, win_cols, win_rows);
    img.addr = mlx_get_data_addr(img.img, &img.bits_per_pixel, &img.line_length,
          &img.endian);
	draw_map(vars, &img);
	mlx_loop_hook(vars.mlx, render_next_frame, &vars);
	mlx_loop(vars.mlx);
}
