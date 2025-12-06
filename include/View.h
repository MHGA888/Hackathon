#ifndef VIEW_H
#define VIEW_H

#include <vector>

struct Point {
    int x;
    int y;
    int z;
    float red;
    float green;
    float blue;
    int classification = 0;
};

class View {
    public:
    std::vector<Point> completed;
};

#endif