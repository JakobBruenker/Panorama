#include <iostream>
#include <CImg.h>

#include "Panorama.h"

using namespace cimg_library;

int main() {
  Panorama pano = Panorama("/mnt/usbdrive/ex2crop/IMG_9405.png");
  CImgDisplay disp = pano.showImage();
  while (!disp.is_closed()) {
    if (disp.is_keyARROWUP()) {
      pano.showFirstPerson(-15., 0., &disp);
    } else if (disp.is_keyARROWDOWN()) {
      pano.showFirstPerson(15., 0., &disp);
    } else if (disp.is_keyARROWLEFT()) {
      pano.showFirstPerson(0., -15., &disp);
    } else if (disp.is_keyARROWRIGHT()) {
      pano.showFirstPerson(0., 15., &disp);
    } else if (disp.is_keyQ()) {
      break;
    }
    disp.wait();
  }
  return 0;
}
