#include <iostream>
#include <algorithm>
#include <CImg.h>

#include "Tonemapper.h"

using namespace cimg_library;
using namespace std;

typedef unsigned char uchar;
typedef unsigned int uint;

Tonemapper::Tonemapper(uint width, uint height, double* pix)
  : imWidth(width), imHeight(height), pixels(NULL) {
  pixels = new double[width*height*3];
  memcpy(pixels, pix, sizeof(*pixels) * width * height * 3);
}

Tonemapper::~Tonemapper() {
  delete[] pixels;
}

// returns luminance, converts pixel to XYZ colorspace
double rgb2xyz(double* p) {
  double x = 5.6507 * (0.49 * p[0] + 0.31 * p[1] + 0.2 * p[2]);
  double y = 5.6507 * (0.17697 * p[0] + 0.8124 * p[1] + 0.01063 * p[2]);
  double z = 5.6507 * (0.01 * p[1] + 0.99 * p[2]);
  p[0] = x;
  p[1] = y;
  p[2] = z;
  return y;
}

// converts pixel to RGB colorspace
void xyz2rgb(double* p) {
  double r = 0.41847 * p[0] - 0.15866 * p[1] - 0.082835 * p[2];
  double g = -0.091169 * p[0] + 0.25243 * p[1] + 0.015708 * p[2];
  double b = 9.209e-4 * p[0] - 0.0025498 * p[1] + 0.1786 * p[2];
  p[0] = r;
  p[1] = g;
  p[2] = b;
}

// assumes input is normalized to [0,1]
double correctGamma(double input) {
  if (input <= 0.018) {
    return 4.5 * input;
  } else {
    return 1.099 * pow(input, 0.41) - 0.99;
  }
}

CImg<double> Tonemapper::getTonemapped() {
  // convert RGB image to CIE XYZ and get max luminance
  double lwmin = 1e35;
  double lwmax = 0;
  for (uint x = 0; x < imWidth; x++) {
    for (uint y = 0; y < imHeight; y++) {
      double lw = rgb2xyz(&pixels[y*imWidth*3+x*3]);
      if (lw > lwmax) {
        lwmax = lw;
      }
      if (lw < lwmin) {
        lwmin = lw;
      }
    }
  }

  // calculate tonemapped luminance for each subpixel
  // normalize subpixels to [0,1]
  // convert back to RGB
  // apply gamma correction
  // draw the pixels on an image
  CImg<double> image = CImg<double>(imWidth, imHeight, 1, 3, 0);
  for (uint x = 0; x < imWidth; x++) {
    for (uint y = 0; y < imHeight; y++) {
      for (uint color = 0; color < 3; color++) {
        double lw = pixels[y*imWidth*3+x*3+color];
        // multiply denominator with lwmax to normalize to [0,1]
        pixels[y*imWidth*3+x*3+color] = log(lw + 1) / (lwmax *
          log(lwmax + 1) * log(2+8*pow(lw/lwmax,log(1.15)/log(0.5))));
      }
      xyz2rgb(&pixels[y*imWidth*3+x*3]);
      double pixel[3] = {0.0};
      for (uint color = 0; color < 3; color++) {
        pixel[color] = correctGamma(pixels[y*imWidth*3+x*3+color]);
      }
      image.draw_point(x,y,0,pixel);
    }
  }

  return image;
}
