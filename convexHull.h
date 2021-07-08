#ifndef CONVEX_HULL_H
#define CONVEX_HULL_H


#include "glImage.h"
#include "coordinate.h"
#include "kMeans.h"
#include <thread>
#include <unordered_map>
#include <future>

#define MAX_ITERATIONS 500
#define DELTA_START 0
#define DELTA_END 0

class ConvexHull {
public:
    ConvexHull(GlImage* img, int imgWidth, int imgHeight, int nRanPoints, int kClusters, ulong seed);

public:
    void convPeel(const std::vector<Coordinate>& convPoints, int r = 255, int g = 255, int b = 255);
    std::vector<Coordinate> convHull(std::vector<Coordinate> convPoints, int r = 255, int g = 255, int b = 255);
    void clusterPeels();
    void generatePoints();
    std::vector<Coordinate> getAllPoints();

private:
    static void removeFromVector(std::vector<Coordinate> *vec, Coordinate c);
    std::vector<Coordinate> sortCoords(std::vector<Coordinate> list);
    bool less(Coordinate a, Coordinate b);
    static int sqDis(Coordinate begin, Coordinate end);
    static int fastOrientation(Coordinate begin, Coordinate mid, Coordinate end);
    void wuLine(int x1, int y1, int x2, int y2, int r, int g, int b);
    static double getRemainder(double x);
    static double getFrac(double x);
    void initImg(int w, int h);
    template<typename T> std::vector<std::vector<T>> group(std::vector<T> items, uint nGroups);
    void processClustersAsync(const std::vector<std::vector<Coordinate>>& clusters);

private:
    std::mt19937 rng;
    GlImage* img;
    int nRanPoints;
    int kClusters;
    std::vector<Coordinate> points;
};


#endif //CONVEX_HULL_H
