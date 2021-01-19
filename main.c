/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acastelb <acastelb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/21 11:17:47 by acastelb          #+#    #+#             */
/*   Updated: 2021/01/19 11:39:04 by acastelb         ###   ########.fr       */
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
	while (i < tile_size *  minimap_scale)
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
			else
				put_tile(tile_x * minimap_scale, tile_y * minimap_scale,
					0, img);
			j++;
		}
		i++;
	}
}

double		get_start_angle(char c)
{
	if (c == 'S')
		return (M_PI_2);
	else if (c == 'W')
		return (M_PI);
	else if (c == 'E')
		return (0);
	return ((3 * M_PI) / 2);
}

void		get_player_infos(t_infos *cub, t_player *player)
{
	int		i;
	int		j;
	char	c;
	i = -1;
	while (cub->map[++i])
	{
		j = -1;
		while (cub->map[i][++j])
			if (ft_strchr("SNEW", cub->map[i][j]))
			{
				player->x = j * tile_size;
				player->y = i * tile_size;
				player->rotation_angle = get_start_angle(cub->map[i][j]);
				cub->map[i][j] = '0';
			}
	}
}

t_player	*player_init(t_infos *cub)
{
	t_player	*player;

	if (!(player = malloc(sizeof(t_player))))
		return (NULL);
	player->radius = 3;
	player->turn_direction = 0;
	player->walk_direction = 0;
	player->move_speed = 3.0;
	player->rotation_speed = 2 * (M_PI / 180);
	get_player_infos(cub, player);
	return (player);
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

int			is_in_the_grid(t_infos *cub, double yintercept, double xintercept)
{
	int y;
	int x;

	if (yintercept < 0 || xintercept < 0)
		return (0);
	x = -1;
	y = -1;
	while (cub->map[++y])
		;
	if (yintercept / tile_size >= y)
		return (0);
	y = (int)yintercept / tile_size;
	while (cub->map[y][++x])
		;
	if (xintercept / tile_size >= x)
		return (0);
	return (1);
}

double		get_distance(double x1, double y1, double x2, double y2)
{
	return (sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1)));
}

double		check_horizontal_hit(t_ray *ray, t_player *player, t_infos *cub)
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
	while (is_in_the_grid(cub, yintercept, xintercept))
	{
		if (cub->map[yintercept / tile_size][(int)xintercept / tile_size] == '1')
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

double		check_vertical_hit(t_ray *ray, t_player *player, t_infos *cub)
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
	while (is_in_the_grid(cub, yintercept, xintercept))
	{
		if (cub->map[(int)yintercept / tile_size][xintercept / tile_size] == '1')
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

void		get_wall_position(t_ray *ray, t_player *player, t_infos *cub)
{
	double	horizontal_hit;
	double	vertical_hit;

	horizontal_hit = check_horizontal_hit(ray, player, cub);
	ray->distance = horizontal_hit;
	vertical_hit = check_vertical_hit(ray, player, cub);
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

void		raycast(t_infos *cub)
{
	t_ray	*ray;
	double	ray_angle;
	int		i;

	i = -1;
	ray_angle = cub->player->rotation_angle - (fov_angle / 2);
	while (++i < cub->num_rays)
	{
		ray_init(ray_angle, &cub->rays[i]);
		ray = &(cub->rays[i]);
		get_wall_position(ray, cub->player, cub);
		ray_angle += fov_angle / (double)cub->num_rays;
	}
}

int			get_color(int color[])
{
	return (color[0] << 16 | color[1] << 8 | color[2]);
}

void		draw_wall(t_infos *cub, int x, int y, int height)
{
	int		i;

	i = -1;
	if (y < 0)
		y = 0;
	while (++i < y)
		my_mlx_pixel_put(cub->img, x,  i,  get_color(cub->c));
	i = -1;
	while (++i + y < cub->r[1])
		if (i < height)
			my_mlx_pixel_put(cub->img, x, y + i,  0x00FFFFFF);
		else
			my_mlx_pixel_put(cub->img, x, y +  i,  get_color(cub->f));
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
		proj_plane_dist = (cub->r[0] / 2) / tan(fov_angle / 2);
		wall_height = (tile_size / correct) * proj_plane_dist;
		draw_wall(cub, i * wall_strip_width, (cub->r[1] / 2) -
				(wall_height / 2), wall_height);
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

int		free_and_quit(t_infos *cub)
{
	free(cub->player);
	free_map(cub->map);
	mlx_destroy_image(cub->mlx, cub->no);
	mlx_destroy_image(cub->mlx, cub->so);
	mlx_destroy_image(cub->mlx, cub->we);
	mlx_destroy_image(cub->mlx, cub->ea);
	mlx_destroy_image(cub->mlx, cub->s);
	mlx_destroy_window(cub->mlx, cub->win);
	exit(0);
	return (0);
}

int			key_hook(int keycode, t_infos *cub)
{
	if (keycode == 126)
		cub->player->walk_direction = 1;
	else if (keycode == 125)
		cub->player->walk_direction = -1;
	else if (keycode == 123)
		cub->player->turn_direction = -1;
	else if (keycode == 124)
		cub->player->turn_direction = 1;
	else if (keycode == 53)
		free_and_quit(cub);
	return (1);
}

int			key_release_hook(int keycode, t_infos *cub)
{
	if (keycode == 126)
		cub->player->walk_direction = 0;
	else if (keycode == 125)
		cub->player->walk_direction = 0;
	else if (keycode == 123)
		cub->player->turn_direction = 0;
	else if (keycode == 124)
		cub->player->turn_direction = 0;
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

int			check_collisions(int movestep, t_infos *cub)
{
	int		next_x;
	int		next_y;

	next_x = cub->player->x + cos(cub->player->rotation_angle) * movestep;
	next_y = cub->player->y + sin(cub->player->rotation_angle) * movestep;
	if (cub->map[next_y / tile_size][next_x / tile_size] != '1')
		return (0);
	return (1);
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
	cub->player->x += cos(cub->player->rotation_angle) * movestep;
	cub->player->y += sin(cub->player->rotation_angle) * movestep;
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

int		get_texture(char *line, t_data **img, t_infos *cub)
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
		return (0);
	path = ft_strdup(line + i);
	if (!(*img = mlx_xpm_file_to_image(cub->mlx, path, &size, &size)))
	{
		perror("Error: ");
		return (0);
	}
	
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
		return (0);
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
			if (check_map_errors(map, i, j, size))
				return (0);
			if (ft_strchr("NSEW", map[i][j]))
				player_position += 1;
		}
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
	{
		cub->map = map;
		return (1);
	}
	printf("invalid map");
	return (free_map(map));
}

int		get_infos(t_infos *cub, char *line, int fd)
{
	if (ft_strnstr(line, "R ", 2) == line)
		return get_digits_infos(line + 2, cub->r, 2, cub);
	else if (ft_strnstr(line, "NO ", 3) == line)
		return get_texture(line + 3, &cub->no, cub);
	else if (ft_strnstr(line, "SO ", 3) == line)
		return get_texture(line + 3, &cub->so, cub);
	else if (ft_strnstr(line, "WE ", 3) == line)
		return get_texture(line + 3, &cub->we, cub);
	else if (ft_strnstr(line, "EA ", 3) == line)
		return get_texture(line + 3, &cub->ea, cub);
	else if (ft_strnstr(line, "S ", 2) == line)
		return get_texture(line + 2, &cub->s, cub);
	else if (ft_strnstr(line, "F ", 2) == line)
		return get_digits_infos(line + 2, cub->f, 3, cub);
	else if (ft_strnstr(line, "C ", 2) == line)
		return get_digits_infos(line + 2, cub->c, 3, cub);
	else if (is_map_start(line, cub))
		return (parse_map(line, cub, fd));
	return (0);
}

int		read_file(t_infos *cub, int fd)
{
	int		ret;
	char	*line;

	if (fd == -1)
	{
		perror("Error: ");
		return (0);
	}
	cub->game_infos = 0;
	cub->mlx = mlx_init();
	ret = get_next_line(fd, &line);
	while (ret > 0)
	{
		if (*line)
		{
			if (!get_infos(cub, line, fd))
				return (0);
		}
		free(line);
		ret = get_next_line(fd, &line);
	}
	return (1);
}

int		main(int ac, char **av)
{
	int		fd;
	t_infos	cub;

	fd = open(av[1], O_RDONLY);
	if (read_file(&cub, fd) == 0)
		return (0);
	launch_game(&cub);
}
