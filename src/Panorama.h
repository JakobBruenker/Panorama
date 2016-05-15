#ifndef PANORAMA_H
#define PANORAMA_H

#include <CImg.h>

class Panorama {
  private:
    cimg_library::CImg<double> image;
    const char* filename;

  public:
    Panorama(const char* filename);
    cimg_library::CImgDisplay showImage();
    cimg_library::CImgDisplay showFirstPerson(double yaw);
};

#endif /* PANORAMA_H */
