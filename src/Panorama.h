#ifndef PANORAMA_H
#define PANORAMA_H

#include <CImg.h>
#include <OpenEXR/ImfRgbaFile.h>

#include "HDR.h"

class Panorama {
  private:
    bool isHDR;
    int width;
    int height;
    double cylMax;
    double cylMin;
    HDR hdr;
    cimg_library::CImg<double> firstPerson;
    cimg_library::CImg<double> panorama;
    cimg_library::CImg<double> normals;
    cimg_library::CImg<double> orig;
    void getRGBA(Imf_2_2::Rgba* pixels);
    void calcFirstPerson();
    void calcPanorama();
    void calcNormals();

  public:
    Panorama(int w, int h, int cmax, int cmin);
    void updateFirstPerson(double pitch, double yaw, cimg_library::CImgDisplay* disp);
    cimg_library::CImgDisplay showPanorama();
    cimg_library::CImgDisplay showFirstPerson();
    cimg_library::CImgDisplay showNormals();
    void writeEXRFile();
};

#endif /* PANORAMA_H */
