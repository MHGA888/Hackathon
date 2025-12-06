#ifndef MODUL_H
#define MODUL_H

#include "./View.h"
#include <pcl/point_types.h>
#include <pcl/segmentation/sac_segmentation.h>
#include <pcl/filters/extract_indices.h>

class Modul {
    public:
    virtual void detect(View& view);
};

#endif