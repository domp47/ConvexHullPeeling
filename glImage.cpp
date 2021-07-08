#include "glImage.h"

/**
 * Stores an image with width w and height h.
 * defaults the image to grey
 */
GlImage::GlImage(int w, int h){
    for(int y = 0; y < h; y++){
        for(int x = 0; x < w; x++){
            GlPixel p;

            p.r = 128;
            p.g = 128;
            p.b = 128;

            this->imgVector.push_back(p);
        }
    }

    this->imgWidth = w;
    this->imgHeight = h;
}

GlImage::~GlImage() = default;

/**
 * Sets a pixel at location x and y in the image with the given color channels
 */
void GlImage::setPixel(int x, int y, int r, int g, int b){

    if(y >= this->imgHeight || y < 0 || x >= this->imgWidth || x < 0){
        return;
    }

    int loc = y * this->imgWidth + x;

    std::lock_guard<std::mutex> lk(mutex);
    this->imgVector[loc].r = r;
    this->imgVector[loc].g = g;
    this->imgVector[loc].b = b;
}

GlPixel* GlImage::getImg(){
    return &this->imgVector[0];
}

int GlImage::getHeight() {
    return this->imgHeight;
}

int GlImage::getWidth() {
    return this->imgWidth;
}

GlPixel GlImage::getPixel(int y, int x) {
    if(y >= this->imgHeight || y < 0 || x >= this->imgWidth || x < 0){
        std::cerr << "Error: index out of bounds." << std::endl;
        return this->imgVector[0];
    }

    return this->imgVector[y * this->imgWidth + x];
}

