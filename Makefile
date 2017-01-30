track_rank: getprettyprint cpputils track.cpp
	g++ ./track.cpp -Wall -O3 -o ./track_rank -static-libgcc -static-libstdc++ -lpthread -std=c++11 -l:Cmn.so -L./cpp-utils -DDEBUGLOG=0 -DTAG="\"$(shell date)\""
	echo "LD_LIBRARY_PATH will need to be updated to have ./cpp-utils"

cpputils: ./cpp-utils/Cmn.h ./cpp-utils/Cmn.cpp
	g++ -c -Wall -Werror -fPIC -shared ./cpp-utils/Cmn.cpp -o ./cpp-utils/Cmn.o -std=c++11 -O3
	g++ -shared -o ./cpp-utils/Cmn.so ./cpp-utils/Cmn.o

cpputilstest: cpputils
	cd cpp-utils && g++ -Wall ./test.cpp -l:Cmn.so -L. -std=c++11 -o ./cpputilstest

clean: 
	rm ./cpp-utils/Cmn.so ./cpp-utils/Cmn.o ./track_rank | true 

getprettyprint: 
	if [ -e ./cxx-prettyprint/prettyprint.hpp ]; then \
	  echo "./cxx-prettyprint/prettyprint.hpp exixts";\
	else \
	  git clone https://github.com/louisdx/cxx-prettyprint.git; \
	fi; 
	

