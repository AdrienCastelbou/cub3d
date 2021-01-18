/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acastelb <acastelb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/21 11:17:47 by acastelb          #+#    #+#             */
/*   Updated: 2021/01/18 12:54:48 by acastelb         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include "gnl/get_next_line.h"
#include "libft/libft.h"

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

	i = 0;
	j = 0;
	while (i < tile_size * minimap_scale)
	{
		j = 0;
		while (j < tile_size * minimap_scale)
		{
			if (wall)
				my_mlx_pixel_put(img, tile_x + i, tile_y + j, 0x002B0F89);
			else
				my_mlx_pixel_put(img, tile_x + i, tile_y + j, 0x00000000);
			j++;
		}
		i++;
	}
}

void		draw_grid(t_vars vars, t_data *img)
{
	int	i;
	int	j;
	int	tile_x;
	int	tile_y;

	tile_x = 0;
	tile_y = 0;
	i = 0;
	while (i < map_rows)
	{
		j = 0;
		while (j < map_cols)
		{
			tile_x = j * tile_size;
			tile_y = i * tile_size;
			put_tile(tile_x * minimap_scale, tile_y * minimap_scale,
					grid[i][j], img);
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
	player->y = win_height / 7;
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
			my_mlx_pixel_put(img, minimap_scale * (player->x + x),
					minimap_scale * (player->y + y), 0x00FF0000);
			y++;
		}
		x++;
	}
}

double		ft_dmod(double x, double y)
{
	return (x - (int)(x / y) * y);
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
	return (sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1)));
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
	xintercept = (player->x + ((double)yintercept - player->y)
			/ tan(ray->ray_angle));
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
	yintercept = (player->y + ((double)xintercept - player->x)
			* tan(ray->ray_angle));
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
			distance = get_distance(player->x, player->y,
					xintercept, yintercept);
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
	ray->ray_angle = ft_abs_angle(ray_angle);
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
		my_mlx_pixel_put(img, minimap_scale * ray->wall_hitX, minimap_scale
				* ray->wall_hitY, 0x00FF0000);
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
		ray = &(vars->rays[i]);
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
			my_mlx_pixel_put(img, x + j, y + i,  0x00FFFFFF);
	}
}

void		draw_3d_map(t_vars *vars, t_data *img)
{
	int		i;
	t_ray	*ray;
	double	proj_plane_dist;
	double	wall_height;
	double	correct;

	i = -1;
	while (++i < num_rays)
	{
		ray = &(vars->rays[i]);
		correct = ray->distance * cos(ray->ray_angle -
				vars->player->rotation_angle);
		proj_plane_dist = (win_width / 2) / tan(fov_angle / 2);
		wall_height = (tile_size / correct) * proj_plane_dist;
		draw_wall(img, i * wall_strip_width, (win_height / 2) -
				(wall_height / 2), wall_strip_width, wall_height);
	}
}

void		draw_map(t_vars *vars, t_data *img)
{
	mlx_destroy_image(vars->mlx, vars->img->img);
	img->img = mlx_new_image(vars->mlx, win_width, win_height);
	img->addr = mlx_get_data_addr(img->img, &img->bits_per_pixel,
			&img->line_length, &img->endian);
	raycast(vars);
	draw_3d_map(vars, img);
	draw_grid(*vars, img);
	draw_player(*vars, img);
	draw_rays(vars, img);
	mlx_put_image_to_window(vars->mlx, vars->win, img->img, 0, 0);
}

int			key_hook(int keycode, t_vars *vars)
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
		mlx_destroy_window(vars->mlx, vars->win);
		exit(0);
	}
	return (1);
}

int			key_release_hook(int keycode, t_vars *vars)
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

	mlx_hook(vars->win, 2, 1L << 0, key_hook, vars);
	mlx_hook(vars->win, 3, 1L << 1, key_release_hook, vars);
	vars->player->rotation_angle += vars->player->turn_direction *
		vars->player->rotation_speed;
	movestep = vars->player->walk_direction * vars->player->move_speed;
	if (check_collisions(movestep, vars->player))
		return (1);
	vars->player->x += cos(vars->player->rotation_angle) * movestep;
	vars->player->y += sin(vars->player->rotation_angle) * movestep;
	draw_map(vars, (vars->img));
	return (1);
}
/*
int			main(void)
{
	t_vars	vars;
	t_data	img;

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
}*/

typedef struct	s_infos {
		int		r[2];
		t_data	*no;
		t_data	*so;
		t_data	*we;
		t_data	*ea;
		t_data	*s;
		int		game_infos;
		int		f[3];
		int		c[3];
		t_vars vars;
}				t_infos;


int		get_texture(char *line, t_data *img, t_infos *cub)
{
	int		i;
	int		size;
	char	*path;

	cub->game_infos += 1;
	i = 0;
	size = 32;
	while (line[i] && line[i] == ' ')
		i++;
	if (!line[i])
		return (-1);
	path = ft_strdup(line + i);
	if (!(img = mlx_xpm_file_to_image(cub->vars.mlx, path, &size, &size)))
		return (-2);
	return (1);
}

int		get_digits_infos(char *line, int set[], int len, t_infos *cub)
{
	int	i;
	int	index;

	cub->game_infos +=1;
	i = 0;
	index = 0;
	while (line[i] && index < len)
	{
		if ((line[i] >= '0' && line[i] <= '9'))
		{
			set[index] = ft_atoi(line + i);
			index++;
			while (line[i] && (line[i] >= '0' && line[i] <= '9'))
				i++;
		}
		else
			i++;
	}
	if (line[i])
		return (-1);
	return (1);
}

int		is_map_start(char *line, t_infos *cub)
{
	int i;
	int wall;

	i = -1;
	wall = 0;
	while (line[++i])
	{
		if (line[i] != ' ' && line[i] != '1')
			return (0);
		if (line[i] == '1')
			wall = 1;
	}
	if (cub->game_infos != 8)
	{
		printf("Error : Wrong number of informations\n");
		return (0);
	}
	return (wall);
}

char	*ft_strjoin_sup(char const *s1, char const *s2, char c)
{
	char	*str;
	int		i;

	if (s1 == NULL || !s2)
		return (NULL);
	if (!(str = (char *)malloc(sizeof(char) *
					(ft_strlen(s1) + ft_strlen(s2) + 2))))
		return (NULL);
	i = 0;
	while (*s1)
	{
		str[i] = *s1;
		s1++;
		i++;
	}
	str[i] = c;
	i++;
	while (*s2)
	{
		str[i] = *s2;
		s2++;
		i++;
	}
	str[i] = '\0';
	return (str);
}

int		check_map_errors(char **map, int i, int j, int size)
{
	char c;

	c = map[i][j];
	if (!ft_strchr(" 012NSEW", c))
		return (1);
	if (c == '1' || c == ' ')
		return (0);
	if (i == 0 || j == 0 || i == size - 1 || !map[i][j + 1])
		return (1);
	if (map[i - 1][j] == ' ' || map[i + 1][j] == ' ' || map[i][j - 1] == ' ' || map[i][j + 1] == ' ')
		return (1);
	return (0);
}

int		map_is_valid(char **map, t_infos *cub)
{
	int i;
	int j;
	int size;
	int	player_position;
	
	player_position = 0;
	i = -1;
	while (map[++i])
		;
	size = i;
	i = -1;
	while (map[++i])
	{
		j = -1;
		while (map[i][++j])
		{
			printf("%c", map[i][j]);
			if (check_map_errors(map, i, j, size))
				return (0);
			if (ft_strchr("NSEW", map[i][j]))
				player_position += 1;
		}
		printf("\n");
	}
	if (!player_position || player_position > 1)
		return (0);
	return (1);
}

int		free_map(char **map)
{
	int i;

	i = -1;
	while (map[++i])
		free(map[i]);
	free(map);
	return (0);
}

int		parse_map(char *line, t_infos *cub, int fd)
{
	char	*joined_map;
	char	**map;
	char	*tmp;

	joined_map = line;
	while (get_next_line(fd, &line) > 0)
	{
		if (!(*line) || !ft_strchr(line, '1'))
		{
			free(line);
			free(joined_map);
			printf("invalid map");
			return (0);
		}
		tmp = joined_map;
		joined_map = ft_strjoin_sup(tmp, line, '\n');
		free(line);
	}
	map = ft_split(joined_map, '\n');
	if (map_is_valid(map, cub))
		return (1);
	printf("invalid map");
	return (free_map(map));
}

int		get_infos(t_infos *cub, char *line, int fd)
{
	if (ft_strnstr(line, "R ", 2) == line)
		return get_digits_infos(line + 2, cub->r, 2, cub);
	else if (ft_strnstr(line, "NO ", 3) == line)
		return get_texture(line + 3, cub->no, cub);
	else if (ft_strnstr(line, "SO ", 3) == line)
		return get_texture(line + 3, cub->so, cub);
	else if (ft_strnstr(line, "WE ", 3) == line)
		return get_texture(line + 3, cub->we, cub);
	else if (ft_strnstr(line, "EA ", 3) == line)
		return get_texture(line + 3, cub->ea, cub);
	else if (ft_strnstr(line, "S ", 2) == line)
		return get_texture(line + 2, cub->s, cub);
	else if (ft_strnstr(line, "F ", 2) == line)
		return get_digits_infos(line + 2, cub->f, 3, cub);
	else if (ft_strnstr(line, "C ", 2) == line)
		return get_digits_infos(line + 2, cub->c, 3, cub);
	else if (is_map_start(line, cub))
		return (parse_map(line, cub, fd));
	return (0);
}

int		main(int ac, char **av)
{
	int		fd;
	int		ret;
	char	*line;
	t_infos	cub;

	fd = open(av[1], O_RDONLY);
	if (fd == -1)
	{
		perror("Error: ");
		return (-1);
	}
	cub.game_infos = 0;
	cub.vars.mlx = mlx_init();
	ret = get_next_line(fd, &line);
	while (ret > 0)
	{
		if (*line)
		{
			if (!get_infos(&cub, line, fd))
				return (0);
		}
		free(line);
		ret = get_next_line(fd, &line);
	}
}
