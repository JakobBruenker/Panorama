#ifndef PANORAMA_H
#define PANORAMA_H

#include <CImg.h>

class Panorama {
  private:
    cimg_library::CImg<double> image;
    cimg_library::CImg<double> orig;
    void calcFirstPerson();

  public:
    Panorama(const char* filename);
    cimg_library::CImgDisplay showImage();
    void showFirstPerson(double pitch, double yaw, cimg_library::CImgDisplay* disp);
};

#endif /* PANORAMA_H */
