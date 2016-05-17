#include <iostream>
#include <CImg.h>

#include "Panorama.h"

using namespace cimg_library;

int main() {
  Panorama pano = Panorama("test/escher.png");
  CImgDisplay fpDisp = pano.showFirstPerson();
  while (!fpDisp.is_closed()) {
    if (fpDisp.is_keyARROWUP()) {
      pano.updateFirstPerson(-15., 0., &fpDisp);
    } else if (fpDisp.is_keyARROWDOWN()) {
      pano.updateFirstPerson(15., 0., &fpDisp);
    } else if (fpDisp.is_keyARROWLEFT()) {
      pano.updateFirstPerson(0., -15., &fpDisp);
    } else if (fpDisp.is_keyARROWRIGHT()) {
      pano.updateFirstPerson(0., 15., &fpDisp);
    } else if (fpDisp.is_keyQ()) {
      break;
    }
    CImgDisplay::wait_all();
  }
  return 0;
}
