/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acastelb <acastelb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/19 15:59:03 by acastelb          #+#    #+#             */
/*   Updated: 2021/01/19 16:07:50 by acastelb         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/cub3d.h"

int			parsing_digits_statement(char c, int index, int len)
{
	if (c || index != len)
	{
		printf("[\033[0;31mFailure\033[0m]\n\033[0;31m");
		printf(")=> Wrong infos / number of infos\n");
		return (0);
	}
	printf("[\033[0;32mSuccess\033[0m]\n");
	return (1);
}

double		get_distance(double x1, double y1, double x2, double y2)
{
	return (sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1)));
}

int			get_int_value(double nb)
{
	double	residue;

	residue = nb - (int)nb;
	if (residue <= 0.5)
		return (floor(nb));
	return (floor(nb) + 1);
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
