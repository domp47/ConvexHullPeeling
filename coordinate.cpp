#include "coordinate.h"

Coordinate::Coordinate(int x, int y){
    this->x = x;
    this->y = y;
}

int Coordinate::getX(){
    return this->x;
}

int Coordinate::getY(){
    return this->y;
}

void Coordinate::setX(int val){
    this->x = val;
}

void Coordinate::setY(int val){
    this->y = val;
}

/**
 * Returns whether a point and this point line on the same point
 */
bool Coordinate::equal(Coordinate c){
    return (this->x == c.getX()) && (this->y == c.getY());
}

std::string Coordinate::print(){
    std::string s;
    s += "(";
    s += std::to_string(this->x);
    s += ", ";
    s += std::to_string(this->y);
    s += ")";
    return s;
}