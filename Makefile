SRCS		=	$(addprefix srcs/, check_wall_hit.c ft_free.c main.c utils.c \
				draw_map.c infos_parsing.c map_parsing.c \
				utils_2.c draw_minimap.c init_player.c \
				raycasting.c)

OBJS		=	$(SRCS:.c=.o)

LIBFT		=	./libft/libft.a

HEADERS		=	$(addprefix includes/, cub3d.h)

INCLUDES	=	-I./includes

NAME		=	cub3d

CC			=	gcc

CFLAGS		=	-c -Wall -Wextra -Werror

$(NAME):		$(OBJS)
				$(MAKE) bonus -C libft
				$(CC) -o $(NAME) $(SRCS) $(LIBFT) -Lmlx  -lmlx -framework OpenGL -framework AppKit $(FLAGS) $(INCLUDES)

all:			$(NAME)

bonus:			$(NAME)

clean:
				$(MAKE) clean -C libft
				rm -f $(OBJS)

fclean:			clean
				$(MAKE) fclean -C libft
				rm -f $(NAME)

re:				fclean all

.PHONY:			all clean fclean re
