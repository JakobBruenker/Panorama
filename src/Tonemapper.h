#ifndef TONEMAPPER_H
#define TONEMAPPER_H

#include <CImg.h>

class Tonemapper {
  unsigned int imWidth;
  unsigned int imHeight;
  double* pixels;

public:
  cimg_library::CImg<double> getTonemapped();
  Tonemapper(unsigned int width, unsigned int height, double* pix);
  ~Tonemapper();
};

#endif /* TONEMAPPER.H */
