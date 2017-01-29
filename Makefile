cpputils: ./cpp-utils/Cmn.h ./cpp-utils/Cmn.cpp
	g++ -c -Wall -Werror -fPIC -shared ./cpp-utils/Cmn.cpp -o ./cpp-utils/Cmn.o -std=c++11 -O3
	g++ -shared -o ./cpp-utils/Cmn.so ./cpp-utils/Cmn.o

cpputilstest: cpputils 
	g++ -Wall ./cpp-utils/test.cpp -l:Cmn.so -L./cpp-utils -std=c++11 -o ./cpp-utils/test

cleancpputils: 
	rm ./cpp-utils/Cmn.so ./cpp-utils/Cmn.o 

getprettyprint: 
	git clone https://github.com/louisdx/cxx-prettyprint.git

track_rank:
	g++ ./track.cpp -Wall -O3 -o ./track_rank -static-libgcc -static-libstdc++ -lpthread -std=c++11 -l:Cmn.so -L./cpp-utils -DDEBUGLOG=0 -DTAG="\"$(shell date)\""

