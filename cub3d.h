/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cub3d.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acastelb <acastelb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/13 11:25:14 by acastelb          #+#    #+#             */
/*   Updated: 2021/01/25 15:46:57 by acastelb         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CUB_H
# define CUB_H
# include <unistd.h>
# include <stdio.h>
# include <stdlib.h>
# include <math.h>
# include <string.h>
# include "minilibx_opengl_20191021/mlx.h"
# include <fcntl.h>
# include <stdint.h>
# include "gnl/get_next_line.h"
# include "libft/libft.h"
# define INT_MAX 2147483647
#define tile_size  64
#define	minimap_scale 0.2

typedef struct	s_ray {
	double			ray_angle;
	double			distance;
	int				wall_hitX;
	int				wall_hitY;
	int				is_go_down;
	int				is_go_left;
	int				is_vrtcl_hit;
	int				side_hit;
}					t_ray;

typedef struct	s_player {
	double		x;
	double		y;
	int			radius;
	int			turn_direction;
	int			walk_direction;
	double		rotation_angle;
	int			move_speed;
	double		rotation_speed;
	double		lateral_move;
	double		fov_angle;
}				t_player;

typedef struct	s_data {
	void		*img;
	char		*addr;
	int			bits_per_pixel;
	int			line_length;
	int			endian;
}				t_data;

typedef struct	s_sprite {
		double x;
		double y;
}				t_sprite;

typedef struct	s_infos {
		int			r[2];
		t_data		*textures[4];
		t_data		*s;
		int			game_infos;
		int			f;
		int			c;
		char		**map;
		void		*mlx;
		void		*win;
		t_player	*player;
		t_data		*img;
		int			num_rays;
		t_ray		*rays;
		int			s_transparency;
		t_sprite	**sprites;
		int			sprites_nb;
		double		*zbuffer;
		int			*sprite_order;
		double		*sprite_dst;
}				t_infos;

typedef	struct	s_hrzt_hit_checker {
		int			ystep;
		double		xstep;
		double		xintercept;
		int			yintercept;
		double		distance;
}				t_hrztl_hit_checker;

typedef	struct	s_vrtcl_hit_checker {
		double		ystep;
		int			xstep;
		int			xintercept;
		double		yintercept;
		double		distance;
}				t_vrtcl_hit_checker;


void			my_mlx_pixel_put(t_data *data, int x, int y, int color);
void			put_tile(double tile_x, double tile_y, int wall, t_data *img);
void			draw_grid(t_infos cub, t_data *img);
t_player		*player_init(t_infos *cub);
void			draw_player(t_infos cub, t_data *img);
double			ft_dmod(double x, double y);
double			ft_abs_angle(double ray_angle);
int				get_int_value(double nb);
int				is_in_the_grid(t_infos *cub, double yintercept, double xintercept);
double			get_distance(double x1, double y1, double x2, double y2);
double			check_horizontal_hit(t_ray *ray, t_player *player, t_infos *cub);
double			check_vertical_hit(t_ray *ray, t_player *player, t_infos *vars);
void			get_wall_position(t_ray *ray, t_player *player, t_infos *vars);
t_ray			*ray_init(double ray_angle, t_ray *ray);
void			draw_rays(t_infos *vars, t_data *img);
void			raycast(t_infos *vars);
void			draw_wall(t_infos *cub, int x, int y, int height, t_ray *ray);
void			draw_3d_map(t_infos *cub, t_data *img);
void			draw_map(t_infos *cub, t_data *img);
int				key_hook(int keycode, t_infos *cub);
int				key_release_hook(int keycode, t_infos *cub);
int				check_collisions(int movestep, t_infos *cub);
int				render_next_frame(t_infos *cub);
int				read_file(t_infos *cub, int fd);
int				get_infos(t_infos *cub, char *line, int fd);
int				parse_map(char *line, t_infos *cub, int fd);
int				free_map(char **map);
int				map_is_valid(char **map, t_infos *cub);
int				check_map_errors(t_infos *cub, int i, int j, int size);
char			*ft_strjoin_sup(char const *s1, char const *s2, char c);
int				is_map_start(char *line, t_infos *cub);
int				get_digits_infos(char *line, int set[], int len, t_infos *cub);
int				get_texture(char *line, t_data **img, t_infos *cub);
int				free_and_quit(t_infos *cub);
int				free_strs(char *s1, char *s2);
int				get_color(int color[]);
int				parsing_digits_statement(char c, int index, int len);
int				launch_game(t_infos *cub);
uint32_t		*init_texture(void);
int				get_text_color(int *add, int pos);
#endif
