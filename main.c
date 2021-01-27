/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acastelb <acastelb@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/21 11:17:47 by acastelb          #+#    #+#             */
/*   Updated: 2021/01/27 16:35:16 by acastelb         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

typedef struct		s_bmp_header {
		char		bitmap_signature_bytes[2];
		uint32_t	size_of_bitmap_file;
		uint32_t	reserved_bytes;
		uint32_t	pixel_data_offset;
}					t_bmp_header;

typedef struct		s_bmp_infos_header {
		uint32_t	sizeOfThisHeader;
		int32_t		width; // in pixels
		int32_t		height; // in pixels
		uint16_t	numberOfColorPlanes; // must be 1
		uint16_t	colorDepth;
		uint32_t	compressionMethod;
		uint32_t	rawBitmapDataSize; // generally ignored
		int32_t		horizontalResolution; // in pixel per meter
		int32_t		verticalResolution; // in pixel per meter
		uint32_t	colorTableEntries;
		uint32_t	importantColors;
}					t_bmp_infos_header;

typedef struct		s_pixel {
		uint8_t	red;
		uint8_t	green;
		uint8_t	blue;
}					t_pixel;

void		create_saved_img(t_infos *cub, t_data *img)
{
	img->img = mlx_new_image(cub->mlx, cub->r[0], cub->r[1]);
	img->addr = mlx_get_data_addr(img->img, &img->bits_per_pixel,
			&img->line_length, &img->endian);
	raycast(cub);
	draw_3d_map(cub, img);
	draw_grid(*cub, img);
	draw_player(*cub, img);
	draw_rays(cub, img);
}

void		bmp_header_init(t_bmp_header *header,
		t_bmp_infos_header *infos_header, t_infos *cub)
{
	header->bitmap_signature_bytes[0] = 'B';
	header->bitmap_signature_bytes[1] = 'M';
	infos_header->sizeOfThisHeader = 40;
	infos_header->width = 512;
	infos_header->height = 512;
	infos_header->numberOfColorPlanes = 1;
	infos_header->colorDepth = 24;
	infos_header->compressionMethod = 0;
	infos_header->rawBitmapDataSize = 0;
	infos_header->horizontalResolution = 3780;
	infos_header->verticalResolution = 3780;
	infos_header->colorTableEntries = 0;
	infos_header->importantColors = 0;
	header->size_of_bitmap_file = (infos_header->sizeOfThisHeader + 14)+
		(infos_header->width * infos_header->height * 3);

}

void		write_bmp_file(t_infos *cub, int fd)
{
	t_bmp_header		bmp_header;
	t_bmp_infos_header	bmp_infos_header;
	t_pixel				pixel;
	size_t				pixels_number;
	int					i;

	bmp_header_init(&bmp_header, &bmp_infos_header, cub);
	pixel.red = 255;
	pixel.green = 0;
	pixel.blue = 0;
	write(fd, (char *) &bmp_header, 14);
	write(fd, (char *) &bmp_infos_header, 40);
	pixels_number = bmp_infos_header.width * bmp_infos_header.height;
	i = -1;
	while (++i < pixels_number)
		write(fd, (char *)&pixel, 3);
}

void		save_first_img(t_infos *cub, char *arg)
{
	t_data	img;
	t_ray	rays[cub->r[0]];
	int		fd;

	if (ft_strncmp(arg, "--save", ft_strlen(arg)))
	{
		printf("\033[0;31mError\nBad argument, try with --save\n");
		return ;
	}
	cub->num_rays = cub->r[0];
	cub->rays = rays;
	cub->player = player_init(cub);
	cub->img = &img;
	create_saved_img(cub, cub->img);
	if ((fd = open("outpout.bmp", O_RDWR)) < 0)
	{
		fd = open("outpout.bmp", O_CREAT);
		close(fd);
		fd = open("outpout.bmp", O_RDWR);
	}
	write_bmp_file(cub, fd);
	close(fd);
	//free_and_quit(cub);
}


int		main(int ac, char **av)
{
	int		fd;
	t_infos	cub;
	
	if (ac > 3)
	{
		printf("\033[0;31mError\nToo many arguments\n");
		return (0);
	}
	fd = open(av[1], O_RDONLY);
	if (read_file(&cub, fd) == 0)
		return (0);
	if (ac == 3)
	{
		save_first_img(&cub, av[2]);
		return (0);
	}
	launch_game(&cub);
}
