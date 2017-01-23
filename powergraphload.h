#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>

#include "log.h"
#include "type_aliases.h"
#include "cpp-utils/Cmn.h"

#include "cxx-prettyprint/prettyprint.hpp"


namespace PowerGraphLoad {
	void rescaleVectors(VectorizedVectors& byIndex, std::vector<double>& scaling){
		for(int i = 0; i < byIndex.size(); i++){
			std::vector<double>& t = *(byIndex[i].get()); 
			if(t.size() != scaling.size()){
				std::cerr << "__SCALING_ARRAY_VECTOR_DIM_MISMATCH__" << t.size() << "/" << scaling.size() << std::endl;
				std::exit(1); 
			}
			for(int j = 0; j < t.size(); j++){
				t[j] = scaling[j] * t[j];
			}
		}
	} 

	void loadToTrack(std::string source, std::unordered_set<int>& v){
		int val = 0;
		FUtils::ByLine infile(source);
		while(infile.next()){
			if(infile.line[0] == '%'){
				std::cerr << "#__IGNORED_COMMENT__:" << infile.line << std::endl;
			} else {
				infile.iss >> val;
				v.insert(val);
			}
		}
		//Debug::log(v);
	} 

	void loadValues(std::string source, std::vector<double>& v, int dim){
		double val = 0;
		FUtils::ByLine infile(source);
		int i = 0;
		while(infile.next() && i++ <= dim){
			if(infile.line[0] == '%'){
				std::cerr << "#__IGNORED_COMMENT__:" << infile.line << std::endl;
			} else {
				infile.iss >> val;
				v.push_back(val);
			}
		}
		Debug::log(v);
		if(v.size() !=  dim){
			std::cerr << "#ERR.. __DIM_MISMATCH__:" << dim << "/" << v.size() << ":" << source <<  std::endl;
			std::exit(1);
		}
	} 

	// load vectors from Graphlab output
	void loadVectors(std::string source, int dim, VectorizedVectors& byIndex){
		std::cerr << "#MSG.. loading:" << source << std::endl;
		FUtils::ByLine infile(source);
		double entry = 0;
		int coord = 0;
		while(infile.next()){
			infile.iss >> coord;
			if(coord >= byIndex.size()){
				std::cerr << "#ERR.. __INDEX_OUT_OF_RANGE__:" << coord  << "/" << byIndex.size() << std::endl;
				std::exit(1);
			}
			if(byIndex[coord]->size() != 0){
				std::cerr << "#ERR.. __REPEATED_VECTOR_INDEX__:" << coord  << std::endl;
				std::exit(1);
			}
			for(int i = 0; i < dim; i++){
				if(infile.iss.eof()){
					std::cerr << "#ERR.. __DIM_MISMATCH__:" << source << std::endl;
					std::exit(1);
				}
				infile.iss >> entry; 
				byIndex[coord] -> push_back(entry);
			}
			
		}
		for(int i = 0; i < byIndex.size(); i++){
			if(byIndex[i]->size() == 0){
				for(int k = 0; k < dim; k++){ byIndex[i]->push_back(0); }
				std::cerr << "__MISSING_ZEROED__:" << i << "[" << source << "]\n";
			}
		}
		Debug::log(byIndex);
	}
}