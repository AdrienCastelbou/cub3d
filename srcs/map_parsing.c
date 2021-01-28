/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   map_parsing.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acastelb <acastelb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/19 14:59:38 by acastelb          #+#    #+#             */
/*   Updated: 2021/01/27 15:27:00 by acastelb         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/cub3d.h"

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
		printf("\033[0;31m Error : Wrong number of informations\n");
		return (0);
	}
	return (wall);
}

int		check_map_errors(t_infos *cub, int i, int j, int size)
{
	char c;
	char **map;

	map = cub->map;
	c = cub->map[i][j];
	if (!ft_strchr(" 012NSEW", c))
		return (1);
	if (c == '1' || c == ' ')
		return (0);
	if (i == 0 || j == 0 || i == size - 1 || !map[i][j + 1])
		return (1);
	if (map[i - 1][j] == ' ' || map[i + 1][j] == ' ' ||
			map[i][j - 1] == ' ' || map[i][j + 1] == ' ')
		return (1);
	if (map[i][j] == '2')
		cub->sprites_nb += 1;
	return (0);
}

int		ft_sptrlen(char **str)
{
	int i;

	i = -1;
	while (str[++i])
		;
	return (i);
}

int		map_is_valid(char **map, t_infos *cub)
{
	int i;
	int j;
	int size;
	int	player_position;

	cub->sprites_nb = 0;
	player_position = 0;
	size = ft_sptrlen(map);;
	i = -1;
	while (map[++i])
	{
		j = -1;
		while (map[i][++j])
		{
			if (check_map_errors(cub, i, j, size))
				return (0);
			if (ft_strchr("NSEW", map[i][j]))
				player_position += 1;
		}
		if (j > cub->max_len)
			cub->max_len = j;
	}
	if (!player_position || player_position > 1)
		return (0);
	return (1);
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
			free_strs(line, joined_map);
			printf("\033[0;31m invalid map");
			return (0);
		}
		tmp = joined_map;
		joined_map = ft_strjoin_sup(tmp, line, '\n');
		free(line);
	}
	map = ft_split(joined_map, '\n');
	cub->max_len = 0;
	cub->map = map;
	if (map_is_valid(map, cub))
		return (1);
	printf("\033[0;31m invalid map");
	return (free_map(map));
}
