# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/04/30 03:02:56 by yzhang2           #+#    #+#              #
#    Updated: 2025/07/19 02:04:55 by yzhang2          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME        := libft.a
CC          := cc
CFLAGS		:= -Wall -Wextra -Werror -I. -Iget_next_line -Ift_printf
AR          := ar -rcs

SRC         := $(filter-out %_bonus.c, $(wildcard *.c))

GNL_DIR     := get_next_line
GNL_SRC     := $(filter-out %_bonus.c, $(wildcard $(GNL_DIR)/*.c))

FTPF_DIR    := ft_printf
FTPF_SRC    := $(wildcard $(FTPF_DIR)/*.c)

BONUS_SRC   := $(wildcard *_bonus.c) \
               $(wildcard $(GNL_DIR)/*_bonus.c)

OBJ         := $(SRC:.c=.o) \
               $(GNL_SRC:%.c=%.o) \
               $(FTPF_SRC:%.c=%.o)
BONUS_OBJ   := $(BONUS_SRC:.c=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(AR) $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

bonus: $(OBJ) $(BONUS_OBJ)
	$(AR) $(NAME) $^

clean:
	rm -f $(OBJ) $(BONUS_OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all bonus clean fclean re