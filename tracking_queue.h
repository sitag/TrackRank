#include <iostream>
#include <string>
#include <limits>
#include <queue>
#include <sstream>
#include <fstream>
#include <algorithm>
#include "cxx-prettyprint/prettyprint.hpp"
#include "cpp-utils/Cmn.h"

namespace TrackingQueue {
	struct CoordDist{
		int coord;
		double d;
	};

	struct MinTrackingQueueCompare { bool operator()(const CoordDist& a, const CoordDist& b) { return a.d > b.d; } };  
	struct MaxTrackingQueueCompare { bool operator()(const CoordDist& a, const CoordDist& b) { return a.d < b.d; } }; 

	typedef std::priority_queue<CoordDist, std::vector<CoordDist>, MinTrackingQueueCompare> MinQueue;
	typedef std::priority_queue<CoordDist, std::vector<CoordDist>, MaxTrackingQueueCompare> MaxQueue;

	template <typename T> class Sentinel;
	template <> class Sentinel<MinTrackingQueueCompare> {
		public:
			static CoordDist sentinel(){
				//double min = std::numeric_limits<double>::min(); 
				double min = -std::numeric_limits<double>::max();
				CoordDist __sentinel = {-1, min};
				return __sentinel;
			}
	};

	template <> class Sentinel<MaxTrackingQueueCompare> {
		public:
			static CoordDist sentinel(){
				double max = std::numeric_limits<double>::max(); 
				CoordDist __sentinel = {-1, max};
				return __sentinel;
			}
	};

	const CoordDist maxSentinel = Sentinel<MaxTrackingQueueCompare>::sentinel();
	const CoordDist minSentinel = Sentinel<MinTrackingQueueCompare>::sentinel();


	template <typename T, typename C>
	inline void fillMinSentinels(std::priority_queue<T, std::vector<T>, C>& queue,  int n){
		for(int s = 0; s < n; s++){
			queue.push(minSentinel);
		}
	}

	template <typename T, typename C>
        inline void fillMaxSentinels(std::priority_queue<T, std::vector<T>, C>& queue,  int n){
                for(int s = 0; s < n; s++){
                        queue.push(maxSentinel);
                }
        } 

	template <typename T>
	inline void update(double& d, int& coord, std::priority_queue<CoordDist, std::vector<CoordDist>, T>& closest){
		closest.pop();
		CoordDist updated = {coord, d};
		closest.push(updated);
	}

	template <typename T, typename OT>
	inline void dumpf(std::priority_queue<CoordDist, std::vector<CoordDist>, T>& closest, OT& msgStream){
		CoordDist updated = minSentinel;
		while (!closest.empty()){
			updated = closest.top();
			msgStream << updated.d << "["  << updated.coord << "],";  
			closest.pop();                    
		}
		msgStream << "\n";
	}
	template <typename T, typename OT>
	inline void dumpfSelected(std::priority_queue<CoordDist, std::vector<CoordDist>, T>& closest, std::unordered_set<int>& recordThese, OT& msgStream){
		CoordDist updated = minSentinel;
		int rank = 0;
		while (!closest.empty()){
			updated = closest.top();
			if(rank == 1 || rank == 0 || recordThese.find(updated.coord) != recordThese.end() ){
				msgStream << "[" << updated.d  << ","   << updated.coord << "," << rank << "],"; 
			}
			rank += 1;
			closest.pop();                    
		}
		msgStream << "\n";
	}




	namespace SanityCheck {
		template <typename T>
		int queueOrdering(bool min){
			std::priority_queue<CoordDist, std::vector<CoordDist>, T> q;
			q.push({1, 1});
			q.push({3, 3});
			q.push({2, 2});
			bool ok = true;
			if(min){
				for(int i = 1; i<=3; i++, q.pop()){ ok = ok && (q.top().d == i); }
			} else {
				for(int i = 3; i>=1; i--, q.pop()){ ok = ok && (q.top().d == i); }
			}
			int priority = ( ok ? ( min ? -1 : 1 ) : 0 );
			std::cerr << "#MSG.. priority:" <<  priority << std::endl; 
			return priority;
		}
	}
}
