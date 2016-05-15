#include <CImg.h>

#include "Panorama.h"

using namespace cimg_library;

int main() {
  Panorama pano = Panorama("test/escher.png");
  CDisplay disp = pano.showImage();
  while (!disp.is_closed()) {
    CDisplay::wait_all();
  }
  return 0;
}
