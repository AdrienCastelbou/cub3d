/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acastelb <acastelb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/21 11:17:47 by acastelb          #+#    #+#             */
/*   Updated: 2020/12/29 10:01:42 by acastelb         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "minilibx_opengl_20191021/mlx.h"
#define		H 1920
#define		V 1080

typedef struct	s_data {
	void		*img;
	char		*addr;
	int			bits_per_pixel;
	int			line_length;
	int			endian;
}				t_data;

typedef struct s_circle {
	int			xc;
	int			yc;
}				t_circle;

typedef struct  s_vars {
    void        *mlx;
    void        *win;
	t_circle	*circle;
}               t_vars;


void		my_mlx_pixel_put(t_data *data, int x, int y, int color)
{
    char	*dst;

	dst = data->addr + (y * data->line_length + x * (data->bits_per_pixel / 8));
	*(unsigned int*)dst = color;
}

int		color_screen(int color, void *mlx, void *mlx_win)
{
	int		x = 0;
	int		y = 0;
	char    *relative_path = "./test.xpm";
    int     img_width = 32;
    int     img_height = 32;
	void	*img;

	img = mlx_xpm_file_to_image(mlx, relative_path, &img_width, &img_height);
	while (y <= 1079)
	{
		x = 0;
		while (x <= 1919)
		{
			mlx_put_image_to_window(mlx, mlx_win, img, x, y);
			x+= 32;
		}
		y += 32;
	}
	return (1);
}

int			render_next_frame(t_vars *vars)
{
	static int color = 0xFFF;

	color_screen(color, vars->mlx, vars->win);
	color++;
	return (1);
}

int			main(void)
{
	t_vars	vars;

	vars.mlx = mlx_init();
	vars.win = mlx_new_window(vars.mlx, 920, 500, "Hello world!");
	mlx_loop(vars.mlx);
}
