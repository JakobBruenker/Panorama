#ifndef HDR_H
#define HDR_H

#include <CImg.h>

class HDR {
  cimg_library::CImg<unsigned char>* images;
  cimg_library::CImg<double>* xs;
  double* times;
  unsigned int numImages;
  double bigI[3][256];
  double weights[256];

  void loadImages();
  void loadResponse();
  void initWeights();
  void loadImage(unsigned int imageNum, std::string);
  double estimateX(unsigned int x, unsigned int y, unsigned int dim);
  unsigned char f(double light, unsigned int color);

public:
  HDR();
  ~HDR();
  std::string getHdrgen();
  void loadIs();
  bool isHDR();
  cimg_library::CImg<double>* getXs();
  void estimateXs();
  void drawResults();
  void maxOverexposed();
  // buffer is a one-dimensional array with dimensions width*height*3
  void getLuminances(double* buffer);
  unsigned int getWidth();
  unsigned int getHeight();
  cimg_library::CImgDisplay drawGraph();
  cimg_library::CImgDisplay showExposure(double time);
};

#endif /* HDR_H */
