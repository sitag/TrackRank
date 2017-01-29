#ifndef DEBUGLOG
#define DEBUGLOG 1
#endif

#include <fstream>
#include <ostream>
#include <vector>
#include <memory>
#include <iostream>
#include <sstream>
#include <mutex>
#include <random>
#include "cxx-prettyprint/prettyprint.hpp"
#include "cpp-utils/Cmn.h"


namespace Log {
	class Mutexed {
		private:
			static std::mutex logger_mutex;
		public:
			static void logger(std::ostringstream& msgStream){
				std::lock_guard<std::mutex> lck(Mutexed::logger_mutex);
				std::time_t now = std::time(0);
				char* dt = ctime(&now);
				std::cout << msgStream.str() <<  "\t@";
				if(dt != nullptr){
					std::cout << std::string(dt) << std::endl;
				} else {
					std::cout << "__" << std::endl;
				}
			}
			template <typename T>
			static void logger(std::vector<T>& v){
				if(DEBUGLOG){ 
					std::lock_guard<std::mutex> lck(Mutexed::logger_mutex);
					std::cout << v << std::endl;
				}
			}
		};
	std::mutex Mutexed::logger_mutex;
	void reset(std::ostringstream& stream){
		stream.str("");
		stream.clear();
	}
}

namespace Debug {
	template <typename T>
	void log(std::vector<std::unique_ptr<std::vector<T>>> & vectorOfUniquePtrs){
		if(DEBUGLOG){ 
			for(auto& e : vectorOfUniquePtrs){
				Log::Mutexed::logger(*(e.get()));
				//std::cerr << *(e.get()) << std::endl;
			}
		}
	}

	template <typename T>
	void log(std::vector<T>& v){
		if(DEBUGLOG){ 
			Log::Mutexed::logger(v);
		}
	}

	void log(std::ostringstream& msgStream){
		if(DEBUGLOG){ 
			Log::Mutexed::logger(msgStream);
		}
	}

	int stressTest(long n){
		std::mt19937 rng;
        rng.seed(std::random_device()());
        std::uniform_int_distribution<std::mt19937::result_type> dist17(1,17);
        int s = dist17(rng);
        int x = 1;
		for(int i = 1; i<n; i++){
			x *= i;
			x = x%s;
		}
		return x;
	}

}

