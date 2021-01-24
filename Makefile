CC = g++
SRC = src/*.cpp
INCLUDE = -I include/
OUT = bin/process

$(OUT):
	${CC} ${SRC} ${INCLUDE} -o $(OUT)
