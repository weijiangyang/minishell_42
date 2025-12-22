# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: yzhang2 <yzhang2@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/10/24 15:45:56 by yzhang2           #+#    #+#              #
#    Updated: 2025/12/22 19:00:00 by yzhang2          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME        := minishell
CC          := gcc
CFLAGS      := -Wall -Wextra -Werror -g -fno-pie
LDFLAGS     := -no-pie
CPPFLAGS    := -Iinclude
LDLIBS      := -lreadline -lhistory

LIBFT_DIR   := libft
LIBFT       := $(LIBFT_DIR)/libft.a

# 所有源文件目录
SRC_DIRS    := src \
               src/lexer \
               src/parse \
               src/exec \
               src/expansion \
               src/signal \
               src/error \
               src/cmds \
               src/loop \
               src/env \
               src/build_in \

# 自动扫描所有源文件
SRCS        := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.c))
OBJS        := $(patsubst src/%.c,obj/%.o,$(SRCS))
OBJDIRS     := $(sort $(dir $(OBJS)))

# =========================== Targets ============================

all: $(NAME)

$(NAME): $(LIBFT) $(OBJS)
	@echo "Linking $@ ..."
	$(CC) $(LDFLAGS) $(OBJS) $(LIBFT) $(LDLIBS) -o $@
	@echo "Built $(NAME)"

$(LIBFT):
	@echo "Building libft with bonus..."
	$(MAKE) -C $(LIBFT_DIR) bonus CFLAGS="$(CFLAGS)" || (echo "libft build failed" && false)

# ==================== Compile Rules =============================

obj/%.o: src/%.c | $(OBJDIRS)
	@echo "CC $<"
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

# 自动创建 obj 目录
$(OBJDIRS):
	@mkdir -p $@

# ========================== Clean ==============================

clean:
	@echo "Cleaning object files..."
	@rm -rf obj
	@$(MAKE) -C $(LIBFT_DIR) clean

fclean: clean
	@echo "Removing binaries..."
	@rm -f $(NAME)
	@$(MAKE) -C $(LIBFT_DIR) fclean || true

re: fclean all

show:
	@echo "SRC_DIRS = $(SRC_DIRS)"
	@echo "SRCS ="
	@printf "  %s\n" $(SRCS)
	@echo "OBJS ="
	@printf "  %s\n" $(OBJS)

.PHONY: all clean fclean re show
