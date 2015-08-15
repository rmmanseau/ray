rm -rf *.o

g++ -c \
  main.cpp \
-I/usr/local/lib -std=c++11

g++ \
  main.o \
  -o ray \
-L/usr/local/lib -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio