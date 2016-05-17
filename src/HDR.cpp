#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>

#include "HDR.h"

typedef unsigned char uchar;
typedef unsigned int uint;

using namespace std;
using namespace cimg_library;

CImg<double>* HDR::getXs() {
  return xs;
}

// loads images specified in a hdrgen file, after sorting them according to the
// exposure time
void HDR::loadImages() {
  string hdrgenPath = getHdrgen();
  ifstream hdrgen(hdrgenPath.c_str());
  string directory = hdrgenPath.substr(0, hdrgenPath.rfind("/"));
  string line;
  uint numLines = 0;
  string filenames[numImages];
  while (getline(hdrgen, line)) {
    uint spacePos = static_cast<uint>(line.find(string(" ")));
    times[numLines] = atof(line.substr(spacePos + 1, line.length()).c_str());
    filenames[numLines] = directory + "/" + line.substr(0, spacePos);
    numLines++;
  }

  struct local {
    // sort assumes that the arrays contain more than one element, and then
    // sorts both according to the first one from high to low
    static void sort(double* ts, string* fns, uint start, uint end) {
      // pick middle element as pivot, to avoid bad performance on already
      // sorted arrays, which should happen quite often
      uint k = (start + end) / 2;
      uint first = start;
      uint last = end;
      do {
        while (ts[first] >= ts[k] && first < k) {
          first++;
        }
        while (ts[last] <= ts[k] && k < last) {
          last--;
        }
        double dswapper = ts[first];
        string sswapper = fns[first];
        ts[first] = ts[last];
        fns[first] = fns[last];
        ts[last] = dswapper;
        fns[last] = sswapper;
        if (k == last) {
          k = first;
        } else if (k == first) {
          k = last;
        }
      } while (first < last);
      if (start < k) {
        sort(ts, fns, start, k);
      }
      if (k + 1 < end) {
        sort(ts, fns, k + 1, end);
      }
    }
  };
  // sort images; this will make it possible to shave off a bit of time later
  local::sort(times, filenames, 0, numImages - 1);

  for (uint i = 0; i < numImages; i++) {
    loadImage(i, filenames[i]);
  }

  hdrgen.close();
}

// could be done as a function instead of saved in an array, but this is
// probably slightly more performant in the long run
void HDR::initWeights() {
  // the weights for extreme pixel values are set to zero, because otherwise,
  // overexposed and underexposed areas are taken into account wrongly
  weights[0] = weights[253] = weights[254] = 0;
  // the rest is a gaussian distribution
  for (uint i = 1; i < 254; i++) {
    weights[i] =
      exp((-4 * (i - 127.5) * (i - 127.5)) / (127.5 * 127.5));
  }
}

// Returns the path of the hdrgen file specified in the config file
string HDR::getHdrgen() {
  string line;
  ifstream config("config.cfg");
  if (!config) {
    cout << "FATAL: Config file ./config.cfg could not be loaded." << endl;
    exit(EXIT_FAILURE);
  }
  getline(config, line);
  config.close();
  return line;
}

bool HDR::isHDR() {
  string hdrgenPath = getHdrgen();
  ostringstream fileEnding;
  fileEnding << hdrgenPath.substr(hdrgenPath.rfind("."), hdrgenPath.length() - hdrgenPath.rfind("."));
  string fileEndingStr = fileEnding.str();
  return (fileEndingStr.compare(".hdrgen") == 0);
}

// loads a single image
void HDR::loadImage(uint imageNum, string filename) {
  try {
    images[imageNum] = CImg<uchar>(filename.c_str());
  } catch (...) {
    cout << "FATAL: Couldn't load image " << filename << "." << endl;
    exit(EXIT_FAILURE);
  }
}

// estimates the luminance value of a single subpixel
double HDR::estimateX(uint x, uint y, uint color) {
  double numerator = 0;
  double denominator = 0;
  for (uint i = 0; i < numImages; i++) {
    uchar pixval = images[i](x, y, 0, color);
    // since the weight would be zero if this is not the case, and nothing
    // would be added, we may as well just skip to the next pixel
    if (pixval > 0) {
      // again, the weight would be zero if this is case, and presumably, all
      // pixels in brighter images2 would also be zero, so we can just break
      // the whole loop
      if (pixval >= 254) {
        break;
      } else {
        double wij = weights[pixval];
        numerator += (wij * bigI[color][pixval]) / times[i];
        denominator += wij / (times[i] * times[i]);
      }
    }
  }
  return denominator == 0 ? 0 : numerator / denominator;
}

// estimates the lumanince over the whole scene
void HDR::estimateXs() {
  for (uint x = 0; x < static_cast<uint>(images[0].width()); x++) {
    for (uint y = 0; y < static_cast<uint>(images[0].height()); y++) {
      double estR = estimateX(x, y, 0);
      double estG = estimateX(x, y, 1);
      double estB = estimateX(x, y, 2);
      double xval[] = {estR, estG, estB};
      xs->draw_point(x,y,xval);
    }
  }
}

// If we don't do anything here, those pixels that were overexposed in all
// images will be black, since the weight will always have been 0. However, if
// they were overexposed even in the darkest image, we know that their
// luminance is at least as high as that of the most luminant pixel we have
// observed.
void HDR::maxOverexposed() {
  double maxLum[3] = {0.0};
  for (uint x = 0; x < static_cast<uint>(images[0].width()); x++) {
    for (uint y = 0; y < static_cast<uint>(images[0].height()); y++) {
      for (uint color = 0; color < 3; color++) {
        if ((*xs)(x,y,0,color) > maxLum[color]) {
          maxLum[color] = (*xs)(x,y,0,color);
        }
      }
    }
  }
  // since we have sorted the images, it suffices to check whether the pixels
  // are overexposed in the first image. If so, they ought to be overexposed in
  // all other images as well
  for (uint x = 0; x < static_cast<uint>(images[0].width()); x++) {
    for (uint y = 0; y < static_cast<uint>(images[0].height()); y++) {
      bool overwrite = false;
      double pixel[3];
      for (uint color = 0; color < 3; color++) {
        if (images[0](x,y,0,color) >= 254) {
          pixel[color] = maxLum[color];
          overwrite = true;
        } else {
          pixel[color] = (*xs)(x,y,0,color);
        }
      }
      if (overwrite) {
        xs->draw_point(x,y,pixel);
      }
    }
  }
}

// calculates the pixel value the camera would have given for a certain amount
// of light of a given color
uchar HDR::f(double light, uint color) {
  if (light <= bigI[color][0]) {
    return 0;
  }
  for (uint i = 1; i < 255; i++) {
    if (light <= bigI[color][i]) {
      return static_cast<uchar>(i);
    }
  }
  return 255;
}

// shows a simulated image with the specified exposure time in seconds
CImgDisplay HDR::showExposure(double time) {
  CImg<uchar> result(xs->width(), xs->height(), 1, 3, 0);
  for (uint x = 0; x < static_cast<uint>(xs->width()); x++) {
    for (uint y = 0; y < static_cast<uint>(xs->height()); y++) {
      uchar pixval[] = {f(time * (*xs)(x,y,0,0), 0),
                        f(time * (*xs)(x,y,0,1), 1),
                        f(time * (*xs)(x,y,0,2), 2)};
      result.draw_point(x,y,pixval);
    }
  }

  ostringstream ss;
  ss << "Image with exposure time of " << time << " seconds";
  return CImgDisplay(result, ss.str().c_str());
}

// puts the values of each of the subpixels into the supplied array
void HDR::getLuminances(double* buffer) {
  for (uint x = 0; x < static_cast<uint>(xs->width()); x++) {
    for (uint y = 0; y < static_cast<uint>(xs->height()); y++) {
      for (uint color = 0; color < 3; color++) {
        buffer[y*xs->width()*3 + x*3 + color] = (*xs)(x,y,0,color);
      }
    }
  }
}

// returns the width of the image
uint HDR::getWidth() {
  return xs->width();
}

// returns the height of the image
uint HDR::getHeight() {
  return xs->height();
}

// creates a window with the response function
CImgDisplay HDR::drawGraph() {
  // we know that I_128 is 1, so we can assume that the minimum value is at
  // most 1 and the maximum value is at least 1
  double minI = 1;
  double maxI = 1;
  for (uint i = 0; i < 254; i++) {
    for (uint j = 0; j < 3; j++) {
      if (bigI[j][i] > maxI) {
        maxI = bigI[j][i];
      } else if (bigI[j][i] < minI) {
        minI = bigI[j][i];
      }
    }
  }

  CImg<uchar> graph(510, 510, 1, 3, 0);

  struct local {
    static void drawData(CImg<uchar> *g, uint x, uint y, uint color) {
      uchar* col;
      uchar red[] = {255, 0, 0};
      uchar green[] = {0, 255, 0};
      uchar blue[] = {50, 50, 255};
      switch (color) {
        case 0: col = red; break;
        case 1: col = green; break;
        case 2: col = blue; break;
        default: col = red;
      }
      g->draw_point(x    , y    , col);
      g->draw_point(x + 1, y    , col);
      g->draw_point(x    , y + 1, col);
      g->draw_point(x + 1, y + 1, col);
    }
  };

  for (uint i = 0; i < 509; i += 2) {
    double den = (log(maxI) - log(minI)) / 510.;
    local::drawData(&graph,
        static_cast<uint>((log(bigI[0][i/2]) - log(minI)) / den), 510 - i, 0);
    local::drawData(&graph,
        static_cast<uint>((log(bigI[1][i/2]) - log(minI)) / den), 510 - i, 1);
    local::drawData(&graph,
        static_cast<uint>((log(bigI[2][i/2]) - log(minI)) / den), 510 - i, 2);
  }

  return CImgDisplay(graph, "Response curves");
}

void HDR::loadIs() {
  string hdrgenPath = getHdrgen();
  ostringstream filename;
  filename << hdrgenPath.substr(0, hdrgenPath.rfind(".")) << ".txt";
  ifstream hdrgen(filename.str().c_str());
  uint i = 0;
  string line;
  while (getline(hdrgen, line)) {
    uint firstBreak = static_cast<uint>(line.find(string(" ")));
    uint lastBreak = static_cast<uint>(line.rfind(string(" ")));
    bigI[0][i] =
      atof(line.substr(0, firstBreak).c_str());
    bigI[1][i] =
      atof(line.substr(firstBreak + 1, lastBreak - firstBreak - 1).c_str());
    bigI[2][i] =
      atof(line.substr(lastBreak + 1, line.length() - lastBreak - 1).c_str());
    i++;
  }
}

// constructor: loads the images and initializes values
HDR::HDR() {
  ifstream hdrgen(getHdrgen().c_str());
  if (!hdrgen) {
    cout << "FATAL: Could not load specified .hdrgen file." << endl;
    exit(EXIT_FAILURE);
  }
  numImages = 0;
  string line;
  while (getline(hdrgen, line)) {
    numImages++;
  }
  hdrgen.close();
  times = new double[numImages];
  images = new CImg<uchar>[numImages];
  loadIs();
  loadImages();
  initWeights();
  xs = new CImg<double>(images[0].width(), images[0].height(), 1, 3, 0);
}

// destructor; frees some allocated memory
HDR::~HDR() {
  delete[] images;
  delete xs;
  delete[] times;
}
