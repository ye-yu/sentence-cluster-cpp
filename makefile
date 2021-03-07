# YeYu Simple English Sentence Clustering Tool
# Copyright (C) 2020 Ye-Yu rafolwen98@gmail.com
# 
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

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
