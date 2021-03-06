clusterer: clusterer.cpp lancaster.cpp similarity.cpp
	g++ -c -g clusterer.cpp -o ./build/clusterer.o
	g++ -c -g lancaster.cpp -o ./build/lancaster.o 
	g++ -c -g similarity.cpp -o ./build/similarity.o
	g++ ./build/lancaster.o ./build/similarity.o ./build/clusterer.o -o ./build/clusterer.exe