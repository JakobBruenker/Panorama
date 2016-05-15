// for using the cmath constants
#define _USE_MATH_DEFINES

#include <cmath>
#include <CImg.h>
#include <iostream>

#include "Panorama.h"

typedef unsigned int uint;

using namespace cimg_library;
using namespace std;

const int WIDTH = 1366;
const int HEIGHT = 768;
const double CYL_MAX = 4;
const double CYL_MIN = -4.;

//Panorama::Panorama(const char* filename) {
//  CImg<double> orig(filename);
//  image = CImg<double>(WIDTH, HEIGHT, 1, 3, 0);
//  for (int xoffs = -WIDTH/2; xoffs < WIDTH/2; xoffs++) {
//    for (int yoffs = -HEIGHT/2; yoffs < HEIGHT/2; yoffs++) {
//      double angle = M_PI * xoffs / 180. / (WIDTH / 360.);
//      double cylY = (CYL_MAX - CYL_MIN) * (static_cast<double>(yoffs) / HEIGHT + 0.5) + CYL_MIN;
//      double x;
//      double y;
//      if (angle == 0 && yoffs == 0) {
//        x = orig.width() / 2.;
//        y = orig.height() / 2.;
//      } else {
//        double px = cos(angle);
//        double py = sin(angle);
//        double pz = cylY;
//        double length = sqrt(pz*pz + py*py);
//        double uy = -pz / length;
//        double uz = py / length;
//        double beta = acos(px / sqrt(px*px + py*py + pz*pz)) / 2;
//        x = uz * sin(beta) * orig.width() / 2 + orig.width() / 2;
//        y = uy * sin(beta) * orig.height() / 2 + orig.height() / 2;
//      }
//      double aX = x - trunc(x);
//      double aY = y - trunc(y);
//      double pixel[3] = {0.};
//      uint origX = static_cast<uint>(x);
//      uint origY = orig.height() - 1 - static_cast<uint>(y);
//      uint origXp1 = origX + 1;
//      uint origYp1 = origY + 1;
//      if (origXp1 >= static_cast<uint>(orig.width())) {
//        origXp1 = origX;
//      }
//      if (origYp1 >= static_cast<uint>(orig.height())) {
//        origYp1 = origY;
//      }
//      for (uint color = 0; color < 3; color++) {
//        pixel[color] += (1 - aX) * (    aY) * orig(origX  , origY  , 0, color);
//        pixel[color] += (    aX) * (    aY) * orig(origXp1, origY  , 0, color);
//        pixel[color] += (1 - aX) * (1 - aY) * orig(origX  , origYp1, 0, color);
//        pixel[color] += (    aX) * (1 - aY) * orig(origXp1, origYp1, 0, color);
//      }
//      image.draw_point(xoffs + (WIDTH / 2), yoffs + (HEIGHT / 2), pixel);
//    }
//  }
//}

Panorama::Panorama(const char* filename) {
  CImg<double> orig(filename);
  image = CImg<double>(WIDTH, HEIGHT, 1, 3, 0);
  for (int xoffs = -WIDTH/2; xoffs < WIDTH/2; xoffs++) {
    for (int yoffs = -HEIGHT/2; yoffs < HEIGHT/2; yoffs++) {
      double angle = M_PI * xoffs / 180. / (WIDTH / 360.);
      double cylY = (CYL_MAX - CYL_MIN) * (static_cast<double>(yoffs) / HEIGHT + 0.5) + CYL_MIN;
      double x;
      double y;
      if (angle == 0 && yoffs == 0) {
        x = orig.width() / 2.;
        y = orig.height() / 2.;
      } else {
        double px = cos(angle);
        double py = sin(angle);
        double pz = cylY;
        double length = sqrt(pz*pz + py*py);
        double uy = -pz / length;
        double uz = py / length;
        double beta = acos(px / sqrt(px*px + py*py + pz*pz)) / 2;
        x = uz * sin(beta) * orig.width() / 2 + orig.width() / 2;
        y = uy * sin(beta) * orig.height() / 2 + orig.height() / 2;
      }
      double aX = x - trunc(x);
      double aY = y - trunc(y);
      double pixel[3] = {0.};
      uint origX = static_cast<uint>(x);
      uint origY = orig.height() - 1 - static_cast<uint>(y);
      uint origXp1 = origX + 1;
      uint origYp1 = origY + 1;
      if (origXp1 >= static_cast<uint>(orig.width())) {
        origXp1 = origX;
      }
      if (origYp1 >= static_cast<uint>(orig.height())) {
        origYp1 = origY;
      }
      for (uint color = 0; color < 3; color++) {
        pixel[color] += (1 - aX) * (    aY) * orig(origX  , origY  , 0, color);
        pixel[color] += (    aX) * (    aY) * orig(origXp1, origY  , 0, color);
        pixel[color] += (1 - aX) * (1 - aY) * orig(origX  , origYp1, 0, color);
        pixel[color] += (    aX) * (1 - aY) * orig(origXp1, origYp1, 0, color);
      }
      image.draw_point(xoffs + (WIDTH / 2), yoffs + (HEIGHT / 2), pixel);
    }
  }
}

CDisplay Panorama::showImage() {
  return CDisplay(CImgDisplay(image, "testball"));
}
