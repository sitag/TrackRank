#include "Cmn.h"
#include <vector>

/*
	expected output
	$ ./cpp-utils/cpputilstest -infile:./cpp-utils/vec.txt 
	#@ Sun Jan 1 22:36:24 2017
	 #MSG.. ./cpp-utils/cpputilstest
	[{"-infile":"./cpp-utils/vec.txt"},["./cpp-utils/cpputilstest"],{}]
	[{"-infile":"./cpp-utils/vec.txt"},["./cpp-utils/cpputilstest"],{}]
	#MSG.. closed:./cpp-utils/vec.txt 2 lines read
	["1 2 3 5","1 5"]
	...............................................
	1:2 3 !
	truncated
	1:5 !
	EOF
	# used direct file access, line count left unmodified 
	#MSG.. closed:./cpp-utils/vec.txt 0 lines read
*/

void testIgnoreLine(std::string& filename){
	FUtils::ByLine reader(filename);
	int coord;
	std::vector<double> v;
	std::cerr << "\n..............................................." << std::endl;
	while(true){
		reader.infile.clear();
		reader.infile >> coord;
		if(reader.onEndOfFile()){
			std::cerr << "EOF" << std::endl;
			break;
		}
		else{
			auto ok = reader.loadVector(v, 2);
			if(!ok){
				std::cout << "truncated" << std::endl;
			}
			reader.ignoreTillNewLine();
			std::cout << coord << ":" ;
			for(auto& e:v){ std::cout << e << " "; }
			std::cout << "!\n";
			v.clear();
		}

	}
	std::cerr << "# used direct file access, line count left unmodified " << std::endl;
}


int main(int argc, char* argv[]){
	auto id = std::string(argv[0]);
    Cmn::timestamp(id);
	if(argc < 2){
        return 1;
    }
    Cmn::Parsed parsed(argc, argv);
	std::cerr << parsed.json() << std::endl; 
	std::string target = parsed["-infile"];
	std::cerr << parsed.json() << std::endl;
	if(target.empty()){
		std::cerr << "need -infile argument with atleast 3 ints per line\n"
			<< " ./cpp-utils/cpputilstest -infile:./cpp-utils/vec.txt" << std::endl;
		return 1;
	}
	auto lines = FUtils::contents(target);
	std::cerr << Json::json(lines);
	testIgnoreLine(target);
	return 0; 
}
