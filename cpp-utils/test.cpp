#include "Cmn.h"
#include <vector>

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
				std::cout << "trunctaed" << std::endl;
			}
			reader.ignoreTillNewLine();
			std::cout << coord << ":" ;
			for(auto& e:v){ std::cout << e << " "; }
			std::cout << "!\n";
			v.clear();
		}

	}
}


int main(int argc, char* argv[]){
	auto id = std::string(argv[0]);
	Cmn::timestamp(id);
	Cmn::Parsed parsed(argc, argv);
	std::cerr << parsed.json() << std::endl; 
	auto target = std::string(parsed["-infile"]);
	auto lines = FUtils::contents(target);
	std::cerr << Json::json(lines);
	testIgnoreLine(target);
	return 0; 
}
