#include <unordered_map>
#include "convexHull.h"

ConvexHull::ConvexHull(GlImage *img, int imgWidth, int imgHeight, int nRanPoints, int kClusters) {
    this->img = img;
    this->nRanPoints = nRanPoints;
    this->kClusters = kClusters;

    initImg(imgWidth, imgHeight);
    generatePoints();
}

/**
 * Removes a coordinate from a vector
 * @param vec Pointer to vector to modify
 * @param c Coordinate to remove
 */
void ConvexHull::removeFromVector(std::vector<Coordinate> *vec, Coordinate c) {
    for(int i = 0; i < vec->size(); i++){
        if((*vec)[i].equal(c)){
            vec->erase(vec->begin() + i);
            return;
        }
    }
}

/**
 * Applies a convex peel to the convPoints specified
 * @param convPoints Points to peel
 * @param r R channel
 * @param g G channel
 * @param b B channel
 */
void ConvexHull::convPeel(const std::vector<Coordinate>& convPoints, int r, int g, int b) {
    if(convPoints.size() < 3){
        std::cout << "Not enough convPoints. Generate some convPoints" << std::endl;
        return;
    }

    std::vector<Coordinate> workPoints;

    workPoints.reserve(convPoints.size());
    for (Coordinate point : convPoints) {
        workPoints.emplace_back(point.getX(), point.getY());
    }

    while(workPoints.size() > 2){

        std::vector<Coordinate> verticesToRemove = convHull(workPoints, r, g, b);

        for(Coordinate c: verticesToRemove){
            removeFromVector(&workPoints, c);
        }
    }
}

/**
 * Applies k-means clustering to the list of points
 */
void ConvexHull::clusterPeels() {
    if(points.size() < 3){
        std::cout << "Not enough points. Generate some points" << std::endl;
        return;
    }

    //K-means clustering algorithm applied to points
    std::vector<std::vector<Coordinate>> clusters = KMeans::group(points, kClusters, MAX_ITERATIONS);

    for(int i = 0; i < kClusters; i++){

        ulong seed = std::random_device()();
        std::mt19937 rng(seed);
        std::uniform_int_distribution<int> dist(0, 255);

        int r = dist(rng), g = dist(rng), b = dist(rng);

        for(Coordinate c: clusters[i]){
            int xLoc = c.getX();
            int yLoc = c.getY();

            for(int deltaY = DELTA_START; deltaY <= DELTA_END; deltaY++){
                for(int deltaX = DELTA_START; deltaX <= DELTA_END; deltaX++){
                    if(yLoc + deltaY >= 0 && yLoc + deltaY < img->getHeight() && xLoc + deltaX >= 0 && xLoc + deltaX < img->getWidth()){
                        img->setPixel(xLoc+deltaX, yLoc+deltaY, r, g, b);
                    }
                }
            }
        }

        convPeel(clusters[i], r, g, b);
    }
}

/**
 * Applies the convex hull algorithm to specified convPoints
 * @param convPoints Points to hull
 * @param r R channel
 * @param g G channel
 * @param b B channel
 * @return List of convPoints used in the hull
 */
std::vector<Coordinate> ConvexHull::convHull(std::vector<Coordinate> convPoints, int r, int g, int b) {
    std::vector<Coordinate> verticesUsed;

    int minX = std::numeric_limits<int>::max(), minY = std::numeric_limits<int>::max();
    int minCoord = -1;

    for(int i = 0; i < convPoints.size(); i++){
        if(convPoints[i].getX() < minX){
            minX = convPoints[i].getX();
            minY = convPoints[i].getY();
            minCoord = i;
        }else if(convPoints[i].getX() == minX && convPoints[i].getY() < minY){
            minX = convPoints[i].getX();
            minY = convPoints[i].getY();
            minCoord = i;
        }
    }

    int i = minCoord;
    int j;

    do{
        j = (int)((i+1) % convPoints.size());
        std::vector<Coordinate> colinearPoints;

        for(int k = 0; k < convPoints.size(); k++){
            int dis = fastOrientation(convPoints[i], convPoints[k], convPoints[j]);

            if(dis < 0){ //The edge is counter clockwise
                j = k;
            }else if(dis == 0 && i != k && k != j){// colinear
                if(sqDis(convPoints[i], convPoints[j]) < sqDis(convPoints[i], convPoints[k])){
                    colinearPoints.push_back(convPoints[j]);
                    j = k;
                }else{
                    colinearPoints.push_back(convPoints[k]);
                }
            }
        }

        verticesUsed.emplace_back(convPoints[i].getX(), convPoints[i].getY());

        for(Coordinate c: colinearPoints){
            verticesUsed.emplace_back(c.getX(), c.getY());
        }

        wuLine(convPoints[i].getX(), convPoints[i].getY(), convPoints[j].getX(), convPoints[j].getY(), r , g, b);
        glutPostRedisplay();

        i = j;

    }while(i != minCoord);

    return sortCoords(verticesUsed);
}

/**
 * Orders the Coordinates in clockwise fashion
 * @param List of Coordinates
 * @return Sorted List of Coordinates
 */
std::vector<Coordinate> ConvexHull::sortCoords(std::vector<Coordinate> list) {
    for(int i = 0; i < list.size(); i++){
        for(int j = 0; j < list.size(); j++){
            if(less(list[i], list[j])){
                auto temp = list[i];
                list[i] = list[j];
                list[j] = temp;
            }
        }
    }
    return list;
}

/**
 * Orders coordinates in counter clockwise fashion 
 * @param Coordinate A
 * @param Coordinate B
 * @return Whether to swap the coordinates 
 */
bool ConvexHull::less(Coordinate a, Coordinate b) {
    Coordinate center(img->getWidth()/2, img->getHeight()/2);

    if (a.getX() - center.getX() >= 0 && b.getX() - center.getX() < 0)
        return true;
    if (a.getX() - center.getX() < 0 && b.getX() - center.getX() >= 0)
        return false;
    if (a.getX() - center.getX() == 0 && b.getX() - center.getX() == 0) {
        if (a.getY() - center.getY() >= 0 || b.getY() - center.getY() >= 0)
            return a.getY() > b.getY();
        return b.getY() > a.getY();
    }

    // compute the cross product of vectors (center -> a) x (center -> b)
    int det = (a.getX() - center.getX()) * (b.getY() - center.getY()) - (b.getX() - center.getX()) * (a.getY() - center.getY());
    if (det < 0)
        return true;
    if (det > 0)
        return false;

    // points a and b are on the same line from the center
    // check which point is closer to the center
    int d1 = (a.getX() - center.getX()) * (a.getX() - center.getX()) + (a.getY() - center.getY()) * (a.getY() - center.getY());
    int d2 = (b.getX() - center.getX()) * (b.getX() - center.getX()) + (b.getY() - center.getY()) * (b.getY() - center.getY());
    return d1 > d2;
}

/**
 * Returns the squared euclidean distance between two points
 * @param begin Point one
 * @param end  Point two
 * @return Squared Distance
 */
int ConvexHull::sqDis(Coordinate begin, Coordinate end) {
    return (begin.getY() - end.getY())*(begin.getY() - end.getY()) + (begin.getX() - end.getX())*(begin.getX() - end.getX());
}

/**
 * Returns the orientation between 3 points
 * @param begin Point one
 * @param mid  Point two
 * @param end  Point three
 * @return 0 if the three points are co-linear or a positive or negative number otherwise
 */
int ConvexHull::fastOrientation(Coordinate begin, Coordinate mid, Coordinate end) {
    return ((mid.getY() - begin.getY()) * (end.getX() - mid.getX())) - ((mid.getX() - begin.getX()) * (end.getY() - mid.getY()));
}

/**
 * Uses Xiaolin Wu's algorithm to draw a line on the image
 * @param x1 First Coordinate
 * @param y1
 * @param x2 Second Coordinate
 * @param y2
 * @param r R channel
 * @param g G channel
 * @param b B channel
 */
void ConvexHull::wuLine(int x1, int y1, int x2, int y2, int r, int g, int b) {
    bool steep = std::abs(y2 - y1) > std::abs(x2 - x1);

    if(steep){
        int temp = x1;
        x1 = y1;
        y1 = temp;

        temp = x2;
        x2 = y2;
        y2 = temp;
    }
    if(x1 > x2){
        int temp = x1;
        x1 = x2;
        x2 = temp;

        temp = y1;
        y1 = y2;
        y2 = temp;
    }

    int dx = x2 - x1;
    int dy = y2 - y1;
    double gradient = (double)dy / (double)dx;

    if (dx == 0.0){
        gradient = 1.0;
    }

    int xEnd = x1;
    double yEnd = y1 + gradient * (xEnd - x1);
    int xGap = (int)getRemainder(x1 + 0.5);
    int xpxl1 = xEnd;
    int ypxl1 = std::floor(yEnd);

    if (steep){
        img->setPixel(ypxl1, xpxl1, (int)(r * (getRemainder(yEnd)*xGap)), (int)(g * (getRemainder(yEnd)*xGap)), (int)(b * (getRemainder(yEnd)*xGap)));
        img->setPixel(ypxl1+1, xpxl1, (int)(r * (getFrac(yEnd)*xGap)), (int)(g * (getFrac(yEnd)*xGap)), (int)(b * (getFrac(yEnd)*xGap)));
    }else{
        img->setPixel(xpxl1, ypxl1, (int)(r * (getRemainder(yEnd)*xGap)), (int)(g * (getRemainder(yEnd)*xGap)), (int)(b * (getRemainder(yEnd)*xGap)));
        img->setPixel(xpxl1, ypxl1+1, (int)(r * (getFrac(yEnd)*xGap)), (int)(g * (getFrac(yEnd)*xGap)), (int)(b * (getFrac(yEnd)*xGap)));
    }
    double intery = yEnd + gradient;

    xEnd = x2;
    yEnd = y2 + gradient * (xEnd - x2);
    xGap = (int)getFrac(x2 + 0.5);
    int xpxl2 = xEnd;
    int ypxl2 = std::floor(yEnd);

    if (steep){
        img->setPixel(ypxl2, xpxl2, (int)(r * (getRemainder(yEnd)*xGap)), (int)(g * (getRemainder(yEnd)*xGap)), (int)(b * (getRemainder(yEnd)*xGap)));
        img->setPixel(ypxl2+1, xpxl2, (int)(r * (getFrac(yEnd)*xGap)), (int)(g * (getFrac(yEnd)*xGap)), (int)(b * (getFrac(yEnd)*xGap)));
    }else{
        img->setPixel(xpxl2, ypxl2, (int)(r * (getRemainder(yEnd)*xGap)), (int)(g * (getRemainder(yEnd)*xGap)), (int)(b * (getRemainder(yEnd)*xGap)));
        img->setPixel(xpxl2, ypxl2+1, (int)(r * (getFrac(yEnd)*xGap)), (int)(g * (getFrac(yEnd)*xGap)), (int)(b * (getFrac(yEnd)*xGap)));
    }

    if (steep){
        for(int x = xpxl1; x < xpxl2; x++){
            img->setPixel(std::floor(intery) , x, (int)(r * getRemainder(intery)), (int)(g * getRemainder(intery)), (int)(b * getRemainder(intery)));
            img->setPixel((int)std::floor(intery)+1, x, (int)(r * getFrac(intery)), (int)(g * getFrac(intery)), (int)(b * getFrac(intery)));
            intery += gradient;
        }
    }else{
        for(int x = xpxl1; x < xpxl2; x++){
            img->setPixel(x, std::floor(intery)  , (int)(r * getRemainder(intery)), (int)(g * getRemainder(intery)), (int)(b * getRemainder(intery)));
            img->setPixel(x, (int)std::floor(intery)+1, (int)(r * getFrac(intery)), (int)(g * getFrac(intery)), (int)(b * getFrac(intery)));
            intery += gradient;
        }
    }
    glutPostRedisplay();
}

/**
 * Gets the remainder of a double
 * @param x Number to get remainder of
 * @return Remainder of x
 */
double ConvexHull::getRemainder(double x) {
    return 1 - getFrac(x);
}

/**
 * Gets the fraction part of a double
 * @param x Number to get fraction of
 * @return Fraction of x
 */
double ConvexHull::getFrac(double x){
    return x - std::floor(x);
}

/**
 * Creates n number of unique random points on the screen.
 * n is specified on program startup
 */
void ConvexHull::generatePoints() {
    points.clear();
    initImg(img->getWidth(), img->getHeight());

    ulong seed = std::random_device()();
    std::mt19937 rng(seed);
    std::uniform_int_distribution<int> distH(1, img->getHeight()-1);
    std::uniform_int_distribution<int> distW(1, img->getWidth()-1);

    std::unordered_map<int, int> usedPoints;

    for(int i = 0; i < nRanPoints; i++){
        int xLoc = distW(rng);
        int yLoc = distH(rng);

        if(usedPoints.find((yLoc*img->getWidth())+xLoc) != usedPoints.end()){
            i--;
            continue;
        }

        img->setPixel(xLoc, yLoc, 255, 255, 255);

        points.emplace_back(xLoc, yLoc);
        usedPoints.insert({(yLoc*img->getWidth())+xLoc, 0});

        std::cout << "\r" << i+1 << "/" << nRanPoints << " Created." << std::flush;
    }
    std::cout << std::endl;

    glutPostRedisplay();
}

/**
 * Initializes the image black
 * @param w Width of image
 * @param h Height of image
 */
void ConvexHull::initImg(int w, int h) {
    delete img;
    img = new GlImage(w, h);

    for(int y = 0; y < h; y++){
        for(int x = 0; x < w; x++){
            img->setPixel(x, y, 0, 0, 0);
        }
    }
}

std::vector<Coordinate> ConvexHull::getAllPoints() {
    return this->points;
}
