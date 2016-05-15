#ifndef PANORAMA_H
#define PANORAMA_H

#include "CDisplay.h"

class Panorama {
  private:
    cimg_library::CImg<double> image;

  public:
    Panorama(const char* filename);
    CDisplay showImage();
};

#endif /* PANORAMA_H */
