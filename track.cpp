#ifndef DEBUGLOG
#define DEBUGLOG 1
#endif

#ifndef TAG
#define TAG "UNTAGGED"
#endif

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <queue>
#include <thread>
#include <memory>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <cmath>  

#include "tracking_queue.h"
#include "cpp-utils/Cmn.h"
#include "powergraphload.h"

#include "cxx-prettyprint/prettyprint.hpp"

namespace Compute {
	inline void __dot(const std::vector<double>& v, const std::vector<double>& u, int dim, double& d){
		d = 0;
		for(int k = 0; k < dim; k++){
			d += u[k]*v[k];
		}
		d = std::abs(d);
	}

	void selectedDot(const ImmutableVectorizedVectors* __byIndex, int start, int end, int threadId, int nTrack, std::string prefix, const std::unordered_set<int>* __tracking, const std::unordered_set<int>* __onlyRecordThese ){
		std::unordered_set<int> tracking = *__tracking;
		std::unordered_set<int> onlyRecordThese = *__onlyRecordThese;
		TrackingQueue::MaxQueue closest;
		std::ostringstream msgStream, distStream;
		msgStream << "#Thread[selectedDotAbs]:" << threadId;
		std::string filename = std::string(prefix) + "." + std::to_string(threadId);
		if(FUtils::exists(filename)){
			msgStream << "..failed:" << filename << " exists";
			Log::Mutexed::cerr(msgStream);	
			return;		
		}
		std::ofstream outfile(filename);
		msgStream << "..tracking:" << filename;
		Log::Mutexed::cerr(msgStream);
		auto& byIndex = *(__byIndex);
		MapOfUniqPtrs<int, std::vector<double>> distances; 
		int dim = byIndex[0]->size();
		int __start = std::max(0, start);
		int __end = std::min((int)byIndex.size(), end); 
		double d = 0;
		bool trackAll = tracking.size() == 0;
		for(int i = __start; i < __end; i++){
			if(tracking.find(i) != tracking.end() || trackAll){
				outfile << "TSELECT[" << i << "]\t";
				auto& u = *(byIndex[i].get());
				TrackingQueue::fillMaxSentinels(closest, byIndex.size());
				for(int j = 0; j < byIndex.size(); j++){
					auto& v = *(byIndex[j].get());
					Compute::__dot(u, v, dim, d);
					TrackingQueue::update(d, j, closest);
				}
				TrackingQueue::dumpfSelected(closest, onlyRecordThese, outfile);
			}
		}
		outfile.close();
		msgStream << "..OK:" << filename; 
		Log::Mutexed::cerr(msgStream);
	}

	ImmutableVectorizedVectors intoImmutable(VectorizedVectors& byIndex){
		ImmutableVectorizedVectors __byIndex;
		for(auto& e: byIndex){
			__byIndex.push_back(std::unique_ptr<std::vector<double>>(e.release()));
		}
		byIndex.clear();
		return __byIndex;  
	}

	typedef void (*vectorUpdateF)(std::unique_ptr<std::vector<double>>&); 
	void updateVectorInPlace(vectorUpdateF f, VectorizedVectors& byIndex){
		for(auto &v:byIndex){
			f(v);
		}
		Debug::log(byIndex);
	}
	void normalizeL2(std::unique_ptr<std::vector<double>>& v){
		double norm = 0.0;
		auto __v = v.get();
		for(auto e:*__v){
			norm += e*e;
		}
		norm = std::sqrt(norm);
		for(auto &e:*__v){
			e = e/norm;
		}
	} 
}


int main(int argc, char* argv[]){
	std::cerr << "compiled:" << TAG << "\n\n" << std::endl; 
	TrackingQueue::SanityCheck::queueOrdering<TrackingQueue::MinTrackingQueueCompare>(true);
	auto binaryname = std::string(argv[0]);

	Cmn::timestamp(binaryname);
	Cmn::Parsed parsed(argc, argv);

	bool debug = parsed.has2("-debug");
	auto prefix = parsed["-prefix"];
	auto fTrackThese = parsed["-only-these"];
	auto fRecordThese = parsed["-record-these"];

	if(parsed.has2("-h")){
		std::cerr << "usage:\n\t./load -nV:3 -eigen-values:./eigenvalues.test -vectors:./vectors.test -dim:3 -track:1 -block-size:2 -prefix:./closest_coords" << std::endl;
		std::exit(1);
	} 

	const int dim = std::stoi(parsed["-dim"]);
	const int track = std::stoi(parsed["-track"]); 
	const int nV = stoi(parsed["-nV"]);
	const int block = stoi(parsed["-block-size"]);


	std::cerr << parsed.json() << std::endl; 

	unsigned int nThreadsSupported = std::thread::hardware_concurrency();
	if(nThreadsSupported == 0){
		std::cerr << "__COULD_NOT_DETECT_HARDWARE_CONCURRENCY__" << std::endl; 
		if(!parsed.has2("-trust-block-size")){
			std::cerr << "..set '-trust-block-size' to ignore" << std::endl;
			std::exit(1);
		}
	} else {
		if(nV / block + 2 > nThreadsSupported){
			std::cerr << "__BLOCK_SIZE_MUST_SATISFY__:nV/BLOCK_SIZE+2<=HARDWARE_CONCURRENCY [" << nThreadsSupported << "]" << std::endl;
			std::exit(1);
		}
	}

	VectorizedVectors byIndex;
	for(int i = 0; i< nV; i++){
		byIndex.push_back(std::unique_ptr<std::vector<double>>(new std::vector<double>()));
		(*byIndex.at(i)).reserve(dim);
	} 
	std::vector<double> eigenValues;
	std::unordered_set<int> trackThese;
	std::unordered_set<int> recordThese;

	PowerGraphLoad::loadVectors(parsed["-vectors"], dim, byIndex);
	std::cerr << "#nVectors:" << byIndex.size() << std::endl;
	PowerGraphLoad::loadValues(parsed["-eigen-values"], eigenValues, dim);
	
	if(!parsed.has2("-track-all")){
		if(fTrackThese.empty()){
			std::cerr << "#ERR: missing tracking file" << std::endl;
			std::exit(1);
		}
		PowerGraphLoad::loadToTrack(fTrackThese, trackThese);
	}
	if(fRecordThese.empty()){
		std::cerr << "#ERR: missing recording file" << std::endl;
		std::exit(1);
	}
	PowerGraphLoad::loadToTrack(fRecordThese, recordThese);

	PowerGraphLoad::rescaleVectors(byIndex, eigenValues);
	Compute::updateVectorInPlace(Compute::normalizeL2, byIndex);

	const ImmutableVectorizedVectors iByIndex = Compute::intoImmutable(byIndex);
	//const ImmutableVectorizedVectors* __byIndex = &iByIndex;

	std::vector<std::thread> workers;
	for(int b = 0; b < iByIndex.size(); b += block){
		workers.push_back(std::thread(Compute::selectedDot, &iByIndex, b, b + block, b/block, track, prefix, &trackThese, &recordThese));
	}
	std::ostringstream msgStream;
	msgStream << "#__DISTRIBUTED_ACROSS_" << workers.size() << "_THREADS__";
	Log::Mutexed::cerr(msgStream);

	for(auto& w : workers){
		w.join();
	}

	
	std::cerr << "#OK" << std::endl;

}



