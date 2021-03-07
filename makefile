ccmp=gcc
carg=--std=c17 -O3
cppcmp=g++
cpparg=--std=c++20 -O3

cargo: cargs.c
	$(ccmp) $(carg) -O3 -c $< -o ./build/cargs.o

lancastero: lancaster.cpp
	$(cppcmp) $(cpparg) -c -g $< -o ./build/lancaster.o 

similarityo: similarity.cpp
	$(cppcmp) $(cpparg) -c -g $< -o ./build/similarity.o 

clustero: cluster.cpp
	$(cppcmp) $(cpparg) -c -g $< -o ./build/cluster.o

libcargyeyu: ./build/cargs.o
	ar rvs ./build/libcargyeyu.a $?

libclustyeyu: ./build/lancaster.o  ./build/similarity.o ./build/cluster.o
	ar rvs ./build/libclustyeyu.a $?

clusterer: clusterer.cpp
	make cargo
	make libcargyeyu

	make lancastero
	make similarityo
	make clustero
	make libclustyeyu

	$(cppcmp) $(cpparg) -c -g $< -o ./build/clusterer.o
	$(cppcmp) $(cpparg) ./build/clusterer.o \
	-Lbuild \
	-lclustyeyu \
	-lcargyeyu \
	-o ./build/clusterer.exe

clean:
	rm -rf ./build/*
