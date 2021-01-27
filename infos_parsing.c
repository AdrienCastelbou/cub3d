/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   infos_parsing.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acastelb <acastelb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/19 15:27:37 by acastelb          #+#    #+#             */
/*   Updated: 2021/01/27 16:24:19 by acastelb         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

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
	printf("Try to load < %s > texture...", path);
	if (!(*img = mlx_xpm_file_to_image(cub->mlx, path, &size, &size)))
	{
		printf("[\033[0;31mFailure\033[0m]\n\033[0;31m");
		printf("=>  No such file or wrong file type, try with .xpm file\n");
		return (0);
	}
	printf("[\033[0;32mSuccess\033[0m]\n");
	return (1);
}

int		get_digits_infos(char *line, int set[], int len, t_infos *cub)
{
	int	i;
	int	index;

	cub->game_infos += 1;
	i = 0;
	index = 0;
	printf("Try to get resolution in < %s > ...", line);
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
	return (parsing_digits_statement(line[i], index, len));
}

int		get_field_and_sky_infos(char *line, int *color, int len, t_infos *cub)
{
	int	i;
	int	index;
	int	bits;

	cub->game_infos += 1;
	i = -1;
	index = 0;
	*color = 0;
	bits = 16;
	printf("Try to get colors infos in < %s > ...", line);
	while (line[++i] && index < len)
	{
		if ((line[i] >= '0' && line[i] <= '9'))
		{
			*color = *color | (ft_atoi(line + i) << bits);
			bits -= 8;
			index++;
			while (line[i] && (line[i] >= '0' && line[i] <= '9'))
				i++;
			i -= 1;
		}
	}
	return (parsing_digits_statement(line[i], index, len));
}

int		get_infos(t_infos *cub, char *line, int fd)
{
	if (ft_strnstr(line, "R ", 2) == line)
		return (get_digits_infos(line + 2, cub->r, 2, cub));
	else if (ft_strnstr(line, "NO ", 3) == line)
		return (get_texture(line + 3, &cub->textures[0], cub));
	else if (ft_strnstr(line, "SO ", 3) == line)
		return (get_texture(line + 3, &cub->textures[1], cub));
	else if (ft_strnstr(line, "WE ", 3) == line)
		return (get_texture(line + 3, &cub->textures[2], cub));
	else if (ft_strnstr(line, "EA ", 3) == line)
		return (get_texture(line + 3, &cub->textures[3], cub));
	else if (ft_strnstr(line, "S ", 2) == line)
		return (get_texture(line + 2, &cub->s, cub));
	else if (ft_strnstr(line, "F ", 2) == line)
		return (get_field_and_sky_infos(line + 2, &cub->f, 3, cub));
	else if (ft_strnstr(line, "C ", 2) == line)
		return (get_field_and_sky_infos(line + 2, &cub->c, 3, cub));
	else if (is_map_start(line, cub))
		return (parse_map(line, cub, fd));
	return (0);
}

t_sprite	**get_sprites_pos(t_infos *cub, char **map)
{
	int			i;
	int			j;
	int			index;
	t_sprite	**sprites;

	if (!(sprites = malloc(sizeof(t_sprite *) * (cub->sprites_nb + 1))))
		return (NULL);
	index = 0;
	i = -1;
	while (map[++i])
	{
		j = -1;
		while (map[i][++j])
			if (map[i][j] == '2')
			{
				sprites[index] = malloc(sizeof(t_sprite));
				sprites[index]->x = j + 0.5;
				sprites[index]->y = i + 0.5;
				index += 1;
			}
	}
	sprites[index] = NULL;
	return (sprites);
}

char		*resize_line(char *s, int len, int size)
{
	char	*new;
	char	*dst;
	int		i;
	int		new_len;

	new_len = size - len;
	new = malloc(sizeof(char) * (new_len + 1));
	if (!new)
		return (NULL);
	i = -1;
	while (++i < new_len)
		new[i] = ' ';
	new[i] = 0;
	dst = ft_strjoin(s, new);
	free(new);
	return (dst);
}

void		resize_map(t_infos *cub, char **map)
{
	int		i;
	int		j;
	char	*tmp;
	i = -1;
	while (map[++i])
	{
		j = ft_strlen(map[i]);
		if (j < cub->max_len)
		{
			tmp = map[i];
			map[i] = resize_line(map[i], j, cub->max_len);
			free(tmp);
		}
	}
}

int			read_file(t_infos *cub, int fd)
{
	int		ret;
	char	*line;

	if (fd == -1)
	{
		perror("\033[0;31m Error: ");
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
	resize_map(cub, cub->map);
	if (cub->sprites_nb > 0)
		cub->sprites = get_sprites_pos(cub, cub->map);
	cub->s_transparency = get_text_color((int*)cub->s->addr, 0);
	return (1);
}
