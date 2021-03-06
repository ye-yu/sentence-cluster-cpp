clusterer: clusterer.cpp lancaster.cpp similarity.cpp
	gcc -c -g cargs.c -o ./build/cargs.o
	g++ -c -g clusterer.cpp -o ./build/clusterer.o
	g++ -c -g lancaster.cpp -o ./build/lancaster.o 
	g++ -c -g similarity.cpp -o ./build/similarity.o
	g++ -c -g cluster.cpp -o ./build/cluster.o
	g++ ./build/lancaster.o \
	./build/similarity.o \
	./build/clusterer.o \
	./build/cluster.o \
	./build/cargs.o \
	-o ./build/clusterer.exe