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
  filename << hdrgenPath.substr(0, hdrgenPath.rfind(".")) << "PANO.exr";
  RgbaOutputFile
    file(filename.str().c_str(), xs->width(), xs->height(), WRITE_RGBA);
  Rgba* pixels = new Rgba[xs->width()*xs->height()];
  getRGBA(pixels);
  file.setFrameBuffer(pixels, 1, xs->width());
  file.writePixels(xs->height());
  delete[] pixels;
}

// calculates the Panorama
void Panorama::calcPanorama() {
  // for every point in the panorama
  for (int xoffs = -width/2; xoffs < width/2; xoffs++) {
    for (int yoffs = -height/2; yoffs < height/2; yoffs++) {
      // get the angle around the cylinder
      double angle = M_PI * xoffs / 180. / (width / 360.);
      // and the vertical offset along cylinder
      double cylY = (cylMax - cylMin) * (static_cast<double>(yoffs) / height + 0.5) + cylMin;
      double x;
      double y;
      // edge case, would divide by 0 otherwise
      if (angle == 0 && yoffs == 0) {
        x = orig.width() / 2.;
        y = orig.height() / 2.;
      } else {
        // rotate around z axis and move vertically
        double px = cos(angle);
        double py = sin(angle);
        double pz = cylY;
        double length = sqrt(pz*pz + py*py);
        // cross product of (1, 0, 0) and new point
        double uy = -pz / length;
        double uz = py / length;
        // angle between (1, 0, 0) and new point (divided by 2)
        double beta = acos(px / sqrt(px*px + py*py + pz*pz)) / 2;
        // rotate about u by beta
        x = uz * sin(beta) * orig.width() / 2 + orig.width() / 2;
        y = uy * sin(beta) * orig.height() / 2 + orig.height() / 2;
      }
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
      // get pixel from input image with bilinear interpolation
      for (uint color = 0; color < 3; color++) {
        pixel[color] += (1 - aX) * (    aY) * orig(origX  , origY  , 0, color);
        pixel[color] += (    aX) * (    aY) * orig(origXp1, origY  , 0, color);
        pixel[color] += (1 - aX) * (1 - aY) * orig(origX  , origYp1, 0, color);
        pixel[color] += (    aX) * (1 - aY) * orig(origXp1, origYp1, 0, color);
      }
      // draw the actual point into the panorama
      panorama.draw_point(xoffs + (width / 2), yoffs + (height / 2), pixel);
    }
  }
}

// convenience function
double i2d(int i) {
  return static_cast<double>(i);
}

// We assume the radius of the photographed sphere is 1, therefore an HSIZE of
// 2 corresponds to 90 degrees fov (atan(1) = 45 degrees, but on both sides)
const double HSIZE = 2;

double pitch = 0;
double yaw = 0;

void Panorama::calcFirstPerson() {
  double VSIZE = HSIZE * height / width;
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
  for (int xoffs = -width/2; xoffs < width/2; xoffs++) {
    for (int yoffs = -height/2; yoffs < height/2; yoffs++) {
      // initRayX would be 1 every time, so we don't need to specify it
      double initRayY = (i2d(xoffs) / width + 0.5) * HSIZE - (HSIZE / 2.);
      double initRayZ = (i2d(yoffs) / height + 0.5) * VSIZE - (VSIZE / 2.);
      // multiply by rotation matrix to get ray with yaw applied
      double yawRayX = yc - ys * initRayY;
      double yawRayY = ys + yc * initRayY;
      double yawRayZ = initRayZ;
      // multiply yawRay by rotation matrix for pitch, so ray has both yaw and
      // pitch applied
      double rayX = yawRayX * (pc + pitchRotX * pitchRotX * (1 - pc)) +
        yawRayY * (pitchRotX * pitchRotY * (1 - pc)) +
        yawRayZ * (pitchRotY * ps);
      double rayY = yawRayX * (pitchRotY * pitchRotX * (1 - pc)) +
        yawRayY * (pc + pitchRotY * pitchRotY * (1 - pc)) +
        yawRayZ * (-pitchRotX * ps);
      double rayZ = yawRayX * (-pitchRotY * ps) +
        yawRayY * (pitchRotX * ps) +
        yawRayZ * pc;
      double length = sqrt(rayZ*rayZ + rayY*rayY);
      // this is now the same as in calcPanorama()
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
      firstPerson.draw_point(xoffs + (width / 2), yoffs + (height / 2), pixel);
    }
  }
}

Panorama::Panorama(int w, int h, int cmax, int cmin) {
  width = w;
  height = h;
  cylMax = cmax;
  cylMin = cmin;
  isHDR = hdr.isHDR();
  normals = CImg<double>("normals.png");
  panorama = CImg<double>(width, height, 1, 3, 0);
  firstPerson = CImg<double>(width, height, 1, 3, 0);
  // if it's an HDR image, estimate the lightness values using the existing
  // response curve and then write out an EXR image of the panorama
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
    // otherwise, just take the image as is and write out the panorama as png
    string filename = hdr.getHdrgen();
    orig = CImg<double>(filename.c_str());
    orig.normalize(0,255);
    ostringstream outfilename;
    outfilename << filename.substr(0, filename.rfind(".")) << "PANO.png";
    calcPanorama();
    panorama.save(outfilename.str().c_str());
  }
  calcFirstPerson();
  calcNormals();
}

// looks up the normals in the provided normalmap image and replaces them with
// the corresponding pixels from the scene
void Panorama::calcNormals() {
  for (uint x = 0; x < static_cast<uint>(normals.width()); x++) {
    for (uint y = 0; y < static_cast<uint>(normals.height()); y++) {
      bool black = true;
      for (uint color = 0; color < 3; color++) {
        if (normals(x,y,0,color) != 0) {
          black = false;
          break;
        }
      }
      // if the pixel is completely black, we assume it's part of the
      // background and just leave it black
      if (!black) {
        // red specifies how far to the right the surface points
        uint origX = static_cast<uint>(normals(x,y,0,0)/255.*orig.width());
        uint origY = static_cast<uint>
          (orig.height() - normals(x,y,0,1)/255.*orig.height());
        // green specifies how far up the surface points
        double aX = origX - trunc(origX);
        double aY = origY - trunc(origY);
        uint origXp1 = origX + 1;
        uint origYp1 = origY + 1;
        if (origXp1 >= static_cast<uint>(orig.width())) {
          origXp1 = origX;
        }
        if (origYp1 >= static_cast<uint>(orig.height())) {
          origYp1 = origY;
        }
        // get original pixel with bilinear interpolation
        double pixel[3] = {0.};
        for (uint color = 0; color < 3; color++) {
          pixel[color] += (1 - aX) * (    aY) * orig(origX  , origY  , 0, color);
          pixel[color] += (    aX) * (    aY) * orig(origXp1, origY  , 0, color);
          pixel[color] += (1 - aX) * (1 - aY) * orig(origX  , origYp1, 0, color);
          pixel[color] += (    aX) * (1 - aY) * orig(origXp1, origYp1, 0, color);
        }
        normals.draw_point(x,y,pixel);
      }
    }
  }
}

CImgDisplay Panorama::showNormals() {
  return CImgDisplay(normals, "Applied to normal map");
}

CImgDisplay Panorama::showFirstPerson() {
  // create Display without normalization (otherwise, brightness would change
  // while looking around)
  return CImgDisplay(firstPerson, "First Person View", 0);
}

// updates the first person view with the specified pitch and yaw
void Panorama::updateFirstPerson(double delPitch, double delYaw, CImgDisplay* disp) {
  yaw += delYaw;
  pitch += delPitch;
  if (pitch > 90.) pitch = 90.;
  if (pitch < -90.) pitch = -90.;
  calcFirstPerson();
  firstPerson.display(*disp);
}
