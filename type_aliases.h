#include <vector>
#include <unordered_map>
#include <memory>


// a vector of pointers to vectors
typedef std::vector<std::unique_ptr<std::vector<double>>> VectorizedVectors;
// a vector of pointers to constant vectors
typedef std::vector<std::unique_ptr<const std::vector<double>>> ImmutableVectorizedVectors; 
// map containing unique pointers
template <typename KT, typename VT> using MapOfUniqPtrs = std::unordered_map<KT, std::unique_ptr<VT>>;
// unique pointer to a vector
template <typename T> using UniqPtrToVector = std::unique_ptr<std::vector<T>>;