/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cub3d.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acastelb <acastelb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/13 11:25:14 by acastelb          #+#    #+#             */
/*   Updated: 2021/01/14 15:28:08 by acastelb         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CUB_H
#define CUB_H
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "minilibx_opengl_20191021/mlx.h"
#define INT_MAX 2147483647
const int		tile_size = 64;
const int		map_rows = 11;
const int		map_cols = 15;
const int		win_height = map_rows * tile_size;
const int		win_width = map_cols * tile_size;
const double	fov_angle = 120 * (M_PI / 180);
const int		wall_strip_width = 1;
const int		num_rays = win_width / wall_strip_width;
const double	minimap_scale = 0.2;
const int		grid[map_rows][map_cols] = {
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

typedef struct	s_ray {
	double		ray_angle;
	double		distance;
	int			wall_hitX;
	int			wall_hitY;
	int			is_go_down;
	int			is_go_left;
}				t_ray;

typedef struct	s_player {
	double		x;
	double		y;
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

typedef struct	s_vars {
    void		*mlx;
    void		*win;
	t_player	*player;
	t_data		*img;
	t_ray		rays[num_rays];
}				t_vars;

void			my_mlx_pixel_put(t_data *data, int x, int y, int color);
void			put_tile(double TileX, double TileY, int Wall, t_data *img);
void			draw_grid(t_vars vars, t_data *img);
t_player		*player_init(void);
void			draw_player(t_vars vars, t_data *img);
double			ft_dmod(double x, double y);
double			ft_abs_angle(double ray_angle);
int				get_int_value(double nb);
int				is_in_the_grid(double yintercept, double xintercept);
double			get_distance(double x1, double y1, double x2, double y2);
double			check_horizontal_hit(t_ray *ray, t_player *player, t_vars *vars);
double			check_vertical_hit(t_ray *ray, t_player *player, t_vars *vars);
void			get_wall_position(t_ray *ray, t_player *player, t_vars *vars);
t_ray			*ray_init(double ray_angle, t_ray *ray);
void			draw_rays(t_vars *vars, t_data *img);
void			raycast(t_vars *vars);
void			draw_wall(t_data *img, int x, int y, int width, int height);
void			draw_3D_map(t_vars *vars, t_data *img);
void			draw_map(t_vars *vars, t_data *img);
int				key_hook(int keycode, t_vars *vars);
int				key_release_hook(int keycode, t_vars *vars);
int				check_collisions(int movestep, t_player *player);
int				render_next_frame(t_vars *vars);
#endif
