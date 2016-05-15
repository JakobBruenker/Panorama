#include <iostream>
#include <CImg.h>

#include "Panorama.h"

using namespace cimg_library;

int main() {
  Panorama pano = Panorama("test/escher.png");
  CImgDisplay disp = pano.showImage();
  while (!disp.is_closed()) {
    if (disp.is_keyARROWUP()) {
      disp = pano.showFirstPerson(0.);
    } else if (disp.is_keyARROWDOWN()) {
      disp = pano.showFirstPerson(0.);
    } else if (disp.is_keyARROWLEFT()) {
      disp = pano.showFirstPerson(-15.);
    } else if (disp.is_keyARROWRIGHT()) {
      disp = pano.showFirstPerson(15.);
    }
    disp.wait();
  }
  return 0;
}
