#include "kMeans.h"

/**
 * Applies the k-means clustering algorithm to a list of points
 * @param data Data Points
 * @param k K  clusters
 * @param nIterations Number of iterations to do
 * @return List of clustered data
 */
std::vector<std::vector<Coordinate>> KMeans::group(std::vector<Coordinate> data, int k, int nIterations){

    std::vector<Coordinate> centroids;

    ulong seed = std::random_device()();
    std::mt19937 rng(seed);
    std::uniform_int_distribution<std::mt19937::result_type> dist(0, data.size());

    centroids.reserve(k);
    for(int i = 0; i < k; i++){
        centroids.push_back(data[dist(rng)]);
    }

    std::vector<int> assignments(data.size());

    for(int i = 0; i < nIterations; i++){

        for(int p = 0; p < data.size(); p++){
            long minDis = std::numeric_limits<long>::max();
            int bestCluster = -1;

            for(int j = 0; j < k; j++){
                long dis = getSqDis(data[p], centroids[j]);

                if(dis < minDis){
                    minDis = dis;
                    bestCluster = j;
                }
            }
            assignments[p] = bestCluster;
        }


        std::vector<Coordinate> newCentroids(k);
        std::vector<int> occurrences(k, 0);
        for(int p = 0; p < data.size(); p++){
            int cluster = assignments[p];
            newCentroids[cluster].setX(newCentroids[cluster].getX() + data[p].getX());
            newCentroids[cluster].setY(newCentroids[cluster].getY() + data[p].getY());
            occurrences[cluster] += 1;
        }


        for(int c = 0; c < k; c++){
            int occurrence = std::max(1, occurrences[c]);

            centroids[c].setX(newCentroids[c].getX() / occurrence);
            centroids[c].setY(newCentroids[c].getY() / occurrence);
        }

        std::cout << "\r"  << i+1 << "/" << nIterations << " Iterations Complete." << std::flush;
    }
    std::cout << std::endl;

    std::vector<std::vector<Coordinate>> clusteredData(k);

    for(Coordinate cord: data){
        long minDis = std::numeric_limits<long>::max();
        int bestCluster = -1;

        for(int j = 0; j < k; j++){
            long dis = getSqDis(cord, centroids[j]);

            if(dis < minDis){
                minDis = dis;
                bestCluster = j;
            }
        }
        clusteredData[bestCluster].push_back(cord);
    }

    return clusteredData;
}

long KMeans::getSqDis(Coordinate begin, Coordinate end){
    return (begin.getX() - end.getX())*(begin.getX() - end.getX()) + (begin.getY() - end.getY())*(begin.getY() - end.getY());
}
