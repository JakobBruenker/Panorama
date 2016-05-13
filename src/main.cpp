#include <CImg.h>

#include "CDisplay.h"

using namespace cimg_library;

typedef unsigned char uchar;
typedef unsigned int uint;

int main() {
  CImg<uchar> image("test/0.jpg");
  CDisplay disp = CDisplay(CImgDisplay(image, "testim"));
  while (!disp.is_closed()) {
    CDisplay::wait_all();
  }
  return 0;
}
