// for using the cmath constants
#define _USE_MATH_DEFINES

#include <cmath>
#include <CImg.h>
#include <iostream>

#include "Panorama.h"
#include "Tonemapper.h"

typedef unsigned int uint;

using namespace cimg_library;
using namespace Imf_2_2;
using namespace std;

const int WIDTH = 1366;
const int HEIGHT = 768;
// TODO different pano width and fps width
const double CYL_MAX = 4;
const double CYL_MIN = -4.;

// puts the pixel in the supplied array, as objects of type Rgba
void Panorama::getRGBA(Rgba* pixels) {
  CImg<double>* xs = &panorama;
  for (uint x = 0; x < static_cast<uint>(xs->width()); x++) {
    for (uint y = 0; y < static_cast<uint>(xs->height()); y++) {
      pixels[y*xs->width()+x] =
        Rgba(static_cast<float>((*xs)(x,y,0,0)),
            static_cast<float>((*xs)(x,y,0,1)),
            static_cast<float>((*xs)(x,y,0,2)));
    }
  }
}

// writes the HDR image in the OpenEXR format
void Panorama::writeEXRFile() {
  CImg<double>* xs = &panorama;
  string hdrgenPath = hdr.getHdrgen();
  ostringstream filename;
  filename << hdrgenPath.substr(0, hdrgenPath.rfind(".")) << ".exr";
  RgbaOutputFile
    file(filename.str().c_str(), xs->width(), xs->height(), WRITE_RGBA);
  Rgba* pixels = new Rgba[xs->width()*xs->height()];
  getRGBA(pixels);
  file.setFrameBuffer(pixels, 1, xs->width());
  file.writePixels(xs->height());
  delete[] pixels;
}


void Panorama::calcPanorama() {
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
      panorama.draw_point(xoffs + (WIDTH / 2), yoffs + (HEIGHT / 2), pixel);
    }
  }
}

double i2d(int i) {
  return static_cast<double>(i);
}

const double HSIZE = 2;
double pitch = 0;
double yaw = 0;

void Panorama::calcFirstPerson() {
  double VSIZE = HSIZE * HEIGHT / WIDTH;
  double x;
  double y;
  // sine and cosine of yaw angle
  double yc = cos(M_PI * yaw / 180);
  double ys = sin(M_PI * yaw / 180);
  // sine and cosine of pitch angle
  double pc = cos(M_PI * pitch / 180);
  double ps = sin(M_PI * pitch / 180);
  // get axis to rotate about for pitch (same for all rays)
  double plength = sqrt(yc*yc + ys*ys);
  double pitchRotX = -ys / plength;
  double pitchRotY = yc / plength;
  // create a ray for each pixel and get the corresponding original pixel
  for (int xoffs = -WIDTH/2; xoffs < WIDTH/2; xoffs++) {
    for (int yoffs = -HEIGHT/2; yoffs < HEIGHT/2; yoffs++) {
      // initRayX would be 1 every time, so we don't need to specify it
      double initRayY = (i2d(xoffs) / WIDTH + 0.5) * HSIZE - (HSIZE / 2.);
      double initRayZ = (i2d(yoffs) / HEIGHT + 0.5) * VSIZE - (VSIZE / 2.);
      double qrayX = yc - ys * initRayY;
      double qrayY = ys + yc * initRayY;
      double qrayZ = initRayZ;
      // multiply yawRay by rotation matrix for pitch
      double rayX = qrayX * (pc + pitchRotX * pitchRotX * (1 - pc)) +
        qrayY * (pitchRotX * pitchRotY * (1 - pc)) +
        qrayZ * (pitchRotY * ps);
      double rayY = qrayX * (pitchRotY * pitchRotX * (1 - pc)) +
        qrayY * (pc + pitchRotY * pitchRotY * (1 - pc)) +
        qrayZ * (-pitchRotX * ps);
      double rayZ = qrayX * (-pitchRotY * ps) +
        qrayY * (pitchRotX * ps) +
        qrayZ * pc;
      double length = sqrt(rayZ*rayZ + rayY*rayY);
      double uy = -rayZ / length;
      double uz = rayY / length;
      double beta = acos(rayX / sqrt(rayX*rayX + rayY*rayY + rayZ*rayZ)) / 2;
      x = uz * sin(beta) * orig.width() / 2 + orig.width() / 2;
      y = uy * sin(beta) * orig.height() / 2 + orig.height() / 2;

      // parameters for bilinear interpolation
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
      // get original pixel with bilinear interpolation
      for (uint color = 0; color < 3; color++) {
        pixel[color] += (1 - aX) * (    aY) * orig(origX  , origY  , 0, color);
        pixel[color] += (    aX) * (    aY) * orig(origXp1, origY  , 0, color);
        pixel[color] += (1 - aX) * (1 - aY) * orig(origX  , origYp1, 0, color);
        pixel[color] += (    aX) * (1 - aY) * orig(origXp1, origYp1, 0, color);
      }
      firstPerson.draw_point(xoffs + (WIDTH / 2), yoffs + (HEIGHT / 2), pixel);
    }
  }
}

Panorama::Panorama(const char* filename) {
  isHDR = hdr.isHDR();
  panorama = CImg<double>(WIDTH, HEIGHT, 1, 3, 0);
  firstPerson = CImg<double>(WIDTH, HEIGHT, 1, 3, 0);
  if (isHDR) {
    hdr.estimateXs();
    hdr.maxOverexposed();
    orig = *hdr.getXs();
    calcPanorama();
    writeEXRFile();
    double* buffer = new double[hdr.getWidth()*hdr.getHeight()*3];
    hdr.getLuminances(buffer);
    Tonemapper tm = Tonemapper(hdr.getWidth(), hdr.getHeight(), buffer);
    orig = tm.getTonemapped();
    delete [] buffer;
    orig.normalize(0,255);
  } else {
    orig = CImg<double>(filename);
    orig.normalize(0,255);
    calcPanorama();
  }
  calcFirstPerson();
}

CImgDisplay Panorama::showFirstPerson() {
  // create Display without normalization (otherwise, brightness would change
  // while looking around)
  return CImgDisplay(firstPerson, "First Person View", 0);
}

void Panorama::updateFirstPerson(double delPitch, double delYaw, CImgDisplay* disp) {
  yaw += delYaw;
  pitch += delPitch;
  if (pitch > 90.) pitch = 90.;
  if (pitch < -90.) pitch = -90.;
  calcFirstPerson();
  firstPerson.display(*disp);
}
