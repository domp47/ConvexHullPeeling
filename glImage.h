#ifndef GL_IMAGE_H
#define GL_IMAGE_H

#include <GL/glut.h>
#include <vector>
#include <iostream>
#include <mutex>
#include "glPixel.h"

class GlImage {
public:
    explicit GlImage(int w=0, int h=0);
    ~GlImage();
    void setPixel(int X, int y, int r, int g, int b);
    GlPixel* getImg();
    int getHeight();
    int getWidth();
    GlPixel getPixel(int y, int x);
private:
    std::mutex mutex;
    int imgHeight;
    int imgWidth;
    std::vector<GlPixel> imgVector;
};


#endif //GL_IMAGE_H
