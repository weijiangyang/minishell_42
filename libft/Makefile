NAME        := libft.a
CC          := cc
CFLAGS      := -Wall -Wextra -Werror -I. -Iget_next_line -Ift_printf
AR          := ar -rcs

# 1. 查找源文件（排除 bonus）
SRC         := $(filter-out %_bonus.c, $(wildcard *.c))
GNL_SRC     := $(filter-out %_bonus.c, $(wildcard get_next_line/*.c))
FTPF_SRC    := $(wildcard ft_printf/*.c)

# 2. 查找 Bonus 文件
BONUS_SRC   := $(wildcard *_bonus.c) $(wildcard get_next_line/*_bonus.c)

# 3. 将所有 .c 转换为 .o
# 使用 $(patsubst ...) 确保路径转换准确
OBJ         := $(SRC:.c=.o) $(GNL_SRC:.c=.o) $(FTPF_SRC:.c=.o)
BONUS_OBJ   := $(BONUS_SRC:.c=.o)

# 强制禁用 Make 的内置隐式规则，防止它尝试把 .o 链接成可执行文件
.SUFFIXES:

all: $(NAME)

$(NAME): $(OBJ)
	@echo "正在打包 $(NAME)..."
	$(AR) $@ $^

# 显式定义 .o 的生成规则
# 这里使用了通配符，可以匹配当前目录和子目录下的文件
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

bonus: $(OBJ) $(BONUS_OBJ)
	@echo "正在打包 $(NAME) (含 Bonus)..."
	$(AR) $(NAME) $^

clean:
	@echo "正在清理对象文件..."
	rm -f $(OBJ) $(BONUS_OBJ)

fclean: clean
	@echo "正在清理 $(NAME)..."
	rm -f $(NAME)

re: fclean all

.PHONY: all bonus clean fclean re