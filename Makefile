similarity: ./similarity.cpp
	g++ -I ./eigen-eigen-10219c95fe65 ./similarity.cpp -o ./similarity -static-libgcc -static-libstdc++ -std=c++11 -l:Cmn.so -L./cpp-utils

similarity-no-debug: ./similarity.cpp
	g++ -I ./eigen-eigen-10219c95fe65 ./similarity.cpp -o ./similarity -DDEBUG=0 -static-libgcc -static-libstdc++ -std=c++11 -l:Cmn.so -L./cpp-utils -Wall -O3


rebuild: ./similarity.cpp cpputils
	g++ -I ./eigen-eigen-10219c95fe65 ./similarity.cpp -o ./similarity -static-libgcc -static-libstdc++ -std=c++11 -l:Cmn.so -L./cpp-utils -O3 -Wall

cpputils: ./cpp-utils/Cmn.h ./cpp-utils/Cmn.cpp
	g++ -c -Wall -Werror -fPIC -shared ./cpp-utils/Cmn.cpp -o ./cpp-utils/Cmn.o -std=c++11 -O3
	g++ -shared -o ./cpp-utils/Cmn.so ./cpp-utils/Cmn.o

cpputilstest: cpputils 
	g++ -Wall ./cpp-utils/test.cpp -l:Cmn.so -L./cpp-utils -std=c++11 -o ./cpp-utils/test

cleancpputils: 
	rm ./cpp-utils/Cmn.so ./cpp-utils/Cmn.o 


trackclosest:
	g++ ./track.cpp -O2 -o ./track -static-libgcc -static-libstdc++  -lpthread -std=c++11 -l:Cmn.so -L./cpp-utils

scratch:
	g++ -Wall -Werror ./scratch.cpp -std=c++11 -O3 -o scratch

getprettyprint: 
	git clone https://github.com/louisdx/cxx-prettyprint.git

trackclosest-release:
	g++ ./track.cpp -Wall -O3 -o ./track_release -static-libgcc -static-libstdc++ -lpthread -std=c++11 -l:Cmn.so -L./cpp-utils -DDEBUGLOG=0 -DTAG="\"$(shell tag)\""

