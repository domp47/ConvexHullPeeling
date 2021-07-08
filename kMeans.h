#ifndef K_MEANS_H
#define K_MEANS_H

#include <vector>
#include "coordinate.h"
#include <iostream>
#include <random>
#include <limits>

class KMeans {
public:
    static std::vector<std::vector<Coordinate>> group(std::vector<Coordinate> data, int k, int nIterations, std::mt19937 rng);
private:
    static long getSqDis(Coordinate begin, Coordinate end);
};


#endif //K_MEANS_H
