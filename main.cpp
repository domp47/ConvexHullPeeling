#include <iostream>
#include "convexHull.h"

GlImage* img = nullptr;
ConvexHull* cv = nullptr;

void resize(int width, int height){
}

/**
 * initially displays the loaded image from memory
 */
void initDisplay(){
    if(img == nullptr)
        return;

    glDrawPixels(img->getWidth(), img->getHeight(), GL_RGB, GL_UNSIGNED_BYTE, (GLubyte*)img->getImg());
    glFlush();
}

/**
 * Menu functions represented in keyboard input
 *
 * @param key Keyboard key pressed
 * @param x not used
 * @param y not used
 */
void keyboard(unsigned char key, int x, int y){
    if(cv == nullptr)
        return;

    switch (key){
        case 'q':
        case 'Q':
            exit(0);
        case 'r':
        case 'R':
            cv->generatePoints();
            break;
        case 'c':
        case 'C':
            cv->convHull(cv->getAllPoints());
            break;
        case 'p':
        case 'P':
            cv->convPeel(cv->getAllPoints());
            break;
        case 'k':
        case 'K':
            cv->clusterPeels();
            break;
        default:break;
    }
}

void printMenu(){
    std::cout << "Q - Quit" << std::endl;
    std::cout << "R - Create Random Points" << std::endl;
    std::cout << "C - Apply Convex Hull to Image" << std::endl;
    std::cout << "P - Apply Convex Peel to Image" << std::endl;
    std::cout << "K - Apply K-Means Clustering to Convex Peel" << std::endl;
}


int main(int argc, char** argv) {

    int nRanPoints = 100;
    int kClusters = 3;
    int w = 1920;
    int h = 1020;

    if(argc != 3){
        std::cout << "Number of points and k clusters not specified. Defaulting to " << nRanPoints << " and " << kClusters << ".\n";
    }else{
        nRanPoints = std::stoi(argv[1]);
        kClusters = std::stoi(argv[2]);
    }

    if(nRanPoints < 3){
        std::cout << "Smallest possible Convex Hull has 3 points. " << nRanPoints << " is not valid.\n";
        return 1;
    }
    if(kClusters > nRanPoints && kClusters > 0){
        std::cout << "k clusters must be less than or equal to the number of random points" << std::endl;
        return 1;
    }

    img = new GlImage(w, h);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
    glutInitWindowSize(w, h);
    glutReshapeFunc(resize);
    glutCreateWindow("Convex Hull");
    glShadeModel(GL_SMOOTH);
    glutDisplayFunc(initDisplay);
    glutKeyboardFunc(keyboard);
    glMatrixMode(GL_PROJECTION);

    printMenu();

    cv = new ConvexHull(img, w, h, nRanPoints, kClusters);

    glutMainLoop();

    delete img;
    delete cv;

    return 0;
}
