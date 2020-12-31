/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acastelb <acastelb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/21 11:17:47 by acastelb          #+#    #+#             */
/*   Updated: 2020/12/31 15:31:09 by acastelb         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "minilibx_opengl_20191021/mlx.h"
#define		Tile_Size	32
#define		MAP_ROWS	11
#define		MAP_COLS	15
#define		WIN_ROWS	MAP_ROWS * Tile_Size
#define		WIN_COLS	MAP_COLS *Tile_Size


const	int grid[MAP_ROWS][MAP_COLS] = {
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

typedef struct s_player {
	int			x;
	int			y;
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

	while (i < Tile_Size)
	{
		j = 0;
		while (j < Tile_Size)
		{
			if (!Wall && ((i != 0 && i != Tile_Size -1) && (j != 0 && j != Tile_Size - 1)))
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
	while (i < MAP_ROWS)
	{
		j = 0;
		while (j < MAP_COLS)
		{
			TileX = j * Tile_Size;
			TileY = i * Tile_Size;
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
	player->x = WIN_COLS / 2;
	player->y = WIN_ROWS / 2;
	player->radius = 3;
	player->turn_direction = 0;
	player->walk_direction = 0;
	player->rotation_angle = M_PI_2;
	player->move_speed = 3.0;
	player->rotation_speed = 3 * (M_PI / 180);
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

void		draw_map(t_vars vars, t_data *img)
{
	draw_grid(vars, img);
	draw_player(vars, img);
	mlx_put_image_to_window(vars.mlx, vars.win, img->img, 0, 0);
}

int		key_hook(int keycode, t_vars *vars)
{
	if (keycode == 6)
		vars->player->walk_direction = 1;
	else if (keycode == 2 )
		vars->player->walk_direction = -1;
	else if (keycode == 12)
		vars->player->turn_direction = -1;
	else if (keycode == 1)
		vars->player->turn_direction = 1;
	return (1);
}

int		key_release_hook(int keycode, t_vars *vars)
{
	if (keycode == 6)
		vars->player->walk_direction = 0;
	else if (keycode == 2 )
		vars->player->walk_direction = 0;
	else if (keycode == 12)
		vars->player->turn_direction = 0;
	else if (keycode == 1)
		vars->player->turn_direction = 0;
	return (1);
}

int		check_collisions(int movestep, t_player *player)
{
	int		next_x;
	int		next_y;

	next_x = player->x + cos(player->rotation_angle) * movestep;
	next_y = player->y + sin(player->rotation_angle) * movestep;
	
	if (grid[next_y / Tile_Size][next_x / Tile_Size] == 1)
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
	vars.win = mlx_new_window(vars.mlx, WIN_COLS, WIN_ROWS, "Hello world!");
	vars.player = player_init();
	vars.img = &img;
	img.img = mlx_new_image(vars.mlx, WIN_COLS, WIN_ROWS);
    img.addr = mlx_get_data_addr(img.img, &img.bits_per_pixel, &img.line_length,
          &img.endian);
	draw_map(vars, &img);
	mlx_loop_hook(vars.mlx, render_next_frame, &vars);
	mlx_loop(vars.mlx);
}
