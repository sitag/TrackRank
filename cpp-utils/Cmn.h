#ifndef CMN_H
#define CMN_H

#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <memory>
#include <vector>
#include <set>
#include <sstream>
#include <algorithm> 
#include <functional> 
#include <unordered_map>
#include <limits>

namespace Json {
	std::string json(const std::unordered_map<std::string, std::string>& m){
		if(m.size() == 0) { return "{}";} 
		std::string encoded = "{";
		for(auto k = m.begin();;){ 
			encoded.append("\"");
			encoded.append(k -> first);
			encoded.append("\"");
			encoded.append(":\"");
			encoded.append(k -> second);
			encoded.append("\"");
			k++ ;
			if( k != m.end() ){ encoded.append(","); } else {
				encoded.append("}");
				return encoded;
			} 
		}
		
	}

	std::string json(const std::vector<std::string>& v){
		if(v.size() == 0) { return "[]";}  
		std::string encoded = "[";
		for(unsigned int k = 0;;){ 
			encoded.append("\"");
			encoded.append(v[k]);
			if( ++k  !=  v.size()){ encoded.append("\","); } else {
				encoded.append("\"]");
				return encoded;
			} 
		}
	} 
	std::string json(const std::set<std::string>& s){
		if(s.size() == 0) { return "{}";}  
		std::string encoded = "{";
		for(auto k = s.begin();;){
			encoded.append("\"");
			encoded.append(*k);
			encoded.append("\":\"\"");
			k++;
			if( k != s.end() ){ encoded.append(","); } else {
				encoded.append("}");
				return encoded;
			} 
		}
	}  
}

namespace Cmn{
	// timestamped msg logger
	inline void timestamp(std::string& msg) {
		std::time_t now = std::time(0);
		char* dt = ctime(&now);
		std::cerr << "#@ " + std::string(dt) + " #MSG.. " + msg + "\n";
	}
	
	// string splitter
	inline void split(const std::string &target, char delimiter, std::vector<std::string> &words){
		std::stringstream ss(target);
		std::string entry;
		while (std::getline(ss, entry, delimiter)) {
			words.push_back(entry);
		}
	}
	
	// nicer split wrapper
	std::vector<std::string> split(const std::string&, char);

	// left strip a string
	inline std::string& lstrip(std::string &arg) {
		arg.erase(arg.begin(), std::find_if(arg.begin(), arg.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
    	return arg;
	}

	// right strip a string
	inline std::string& rstrip(std::string &arg) {
		arg.erase(std::find_if(arg.rbegin(), arg.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), arg.end());
	  return arg;
	}

	// strip a string
	inline std::string& strip(std::string& arg) {
		return Cmn::rstrip(Cmn::lstrip(arg));
	}

	class Parsed{
	public:
		bool has(std::string& arg){
			return flags.find(arg) != flags.end();
		}
		bool has2(std::string arg){
			return flags.find(arg) != flags.end();
		}
		std::unordered_map<std::string, std::string> keys;
		std::vector<std::string> values;
		std::set<std::string> flags;
		Parsed(int argc, char* argv[]){
			for(int i = 0; i<argc; i++){
				if(argv[i][0] == '-'){
					auto fields = split(std::string(argv[i]), ':');
					if(fields.size() < 2){
						flags.insert(fields[0]); 
					} else {
						keys.emplace(fields[0], fields[1]); 
					}
				} else {
					values.emplace_back(std::string(argv[i]));
				}
			}
		}

		std::string operator[](const std::string& k){
			return keys[k];
		}

		
		std::string json(){
			std::stringstream encoded;
			encoded << "[" << Json::json(keys) << "," << Json::json(values) << "," << Json::json(flags) << "]";
			return encoded.str();
		} 

		virtual ~Parsed(){}

	};


}



namespace FUtils {


	inline bool exists(const std::string& filename){
		std::ifstream testexists(filename);
		return (bool)testexists;
	}



	class ByLine {
		public:
		std::string file;
		std::string line;
		std::vector<std::string> fields;
		std::ifstream infile;
		std::istringstream iss;
		bool status;
		int verbose = 1;
		int count = 0;

		bool onEndOfFile() throw(int) {
			if(infile.eof()){
				return true;
			} else if(infile.fail()){
				std::cerr << "READFAILED:" << file << ":" << count << std::endl;
				throw 1;
			} else {
				return false;
			} 
		}	
		void ignoreTillNewLine(){
			const char NEWLINE = '\n';
			infile.ignore(std::numeric_limits<std::streamsize>::max(), NEWLINE);	
		}

		bool loadVector(std::vector<double>& v, int n){
			double entry = 0;
			int i = 0;
			while(i < n){
				infile.clear();
				infile >> entry;
				if(onEndOfFile()){
					break;
				}
				v.push_back(entry);
				i++;
			}
			return i == n;
		}


		ByLine(std::string& filename, int logging=1) throw(int){
			status = true;
			verbose = logging;
			if(!exists(filename)){
				std::cerr << "ERR.. file does not exist: <" + filename + ">\n";
				throw -1;
			};
			infile.open(filename);
			file = filename;
		}

		inline bool next(){
			status = (bool)std::getline(infile, line);
			if(!status) {
				return false;
			} else {
				count++;
				iss.str(std::string());
				iss.clear();
				iss.str(line);
				return true;
			}
		}

		

		inline bool next(char splitBy){
			if(!next()){
				return false;
			} else {
				//fields = Cmn::split(line, splitBy);
				fields.clear();
				Cmn::split(line, splitBy, fields);
				return true;
			}
		}

		virtual ~ByLine(){
			if(infile.is_open()) { 
				infile.close(); 
				if(verbose>0){ std::cerr << "#MSG.. closed:" << file << " " << count << " lines read" <<  std::endl; }
			}
		}
	};

	std::vector<std::string> contents(std::string source){
			std::vector<std::string> lines;
			ByLine reader (source);
			while(reader.next()){
				lines.push_back(reader.line);
			}
			return lines;
	}
}


#endif

