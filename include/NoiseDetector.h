#ifndef NOISE_DETECTOR
#define NOISE_DETECTOR

#include "./View.h"
#include "Modul.h"
#include <pcl/point_types.h>
#include <pcl/segmentation/sac_segmentation.h>
#include <pcl/filters/extract_indices.h>
#include <pcl/filters/radius_outlier_removal.h>

class NoiseDetector : public Modul {
    public:
    void detect(View& view) {

    }
};

#endif