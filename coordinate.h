#ifndef COORDINATE_H
#define COORDINATE_H


#include <string>

class Coordinate {
public:
    Coordinate(int x=0, int y=0);
    int getX();
    int getY();
    void setX(int val);
    void setY(int val);
    bool equal(Coordinate c);
    std::string print();
private:
    int x, y;
};


#endif //COORDINATE_H
