#ifndef PANORAMA_H
#define PANORAMA_H

#include <CImg.h>
#include <OpenEXR/ImfRgbaFile.h>

#include "HDR.h"

class Panorama {
  private:
    bool isHDR;
    HDR hdr;
    cimg_library::CImg<double> firstPerson;
    cimg_library::CImg<double> panorama;
    cimg_library::CImg<double> orig;
    void getRGBA(Imf_2_2::Rgba* pixels);
    void calcFirstPerson();
    void calcPanorama();

  public:
    Panorama(const char* filename);
    void updateFirstPerson(double pitch, double yaw, cimg_library::CImgDisplay* disp);
    cimg_library::CImgDisplay showPanorama();
    cimg_library::CImgDisplay showFirstPerson();
    void writeEXRFile();
};

#endif /* PANORAMA_H */
