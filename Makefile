

CC = gcc

SRCS = myhttpd.c \
	   sql.c \
	   mysqlhelper.c \
	   cJSON.c  \
	   http_handle.c	\
	   data_handle.c

OBJS = $(SRCS:.c=.o)

EXEC = myhttpd

CFLAGS = -g -Wall -O2 

LIBS = -levent -lmysqlclient -lm 

start:$(OBJS)
	$(CC) -o $(EXEC) $(OBJS) $(LIBS)

.cpp.o:
	$(CC) -o $@ -c $<

.c.o:
	$(CC) -o $@ -c $<

#创建库文件
#命名规则强制libXXX.so
#在.o的语句中加上-fPIC    位置无关代码
#在库的生成语句中添加  -shared

clean:
	rm -rf $(OBJS) $(EXEC)
