#指定编译器
CC = g++

#找出当前目录下，所有的源文件（以.cpp结尾）
SRCS := $(shell find ./* -type f | grep '\.cpp' | grep -v 'server\.cpp' | grep -v 'client\.cpp')
$(warning SRCS is ${SRCS})

#确定cpp源文件对应的目标文件
OBJS := $(patsubst %.cpp, %.o, $(filter %.cpp, $(SRCS)))
$(warning OBJS is ${OBJS})

#编译选项
CFLAGS = -g -O2 -Wall -Werror -Wno-unused -ldl -lpthread -fPIC -std=c++14
$(warning CFLAGS is ${CFLAGS})

#找出当前目录下所有的头文件
INCLUDE_TEMP = $(shell find ./* -type d)
INCLUDE = $(patsubst %,-I %, $(INCLUDE_TEMP))
$(warning INCLUDE is ${INCLUDE})

SRC_SERVER = server.cpp
OBJ_SERVER = ${SRC_SERVER:%.cpp=%.o}
EXE_SERVER = server

SRC_CLIENT = client.cpp
OBJ_CLIENT = ${SRC_CLIENT:%.cpp=%.o}
EXE_CLIENT = client

target: ${EXE_SERVER} ${EXE_CLIENT}

server: ${EXE_SERVER}

client: ${EXE_CLIENT}

$(EXE_SERVER): $(OBJ_SERVER) $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS) $(INCLUDE)

$(EXE_CLIENT): $(OBJ_CLIENT) $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS) $(INCLUDE)

clean:
	rm -f ${OBJS} ${OBJ_SERVER} ${OBJ_CLIENT} ${EXE_SERVER} ${EXE_CLIENT}

%.o: %.cpp
	${CC} ${CFLAGS} ${INCLUDE} -c $< -o $@
