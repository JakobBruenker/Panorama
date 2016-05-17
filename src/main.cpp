#include <iostream>
#include <CImg.h>

#include "Panorama.h"

using namespace cimg_library;

int main() {
  // Create a new Panorama with specified width, height, and cylinder
  // dimensions (top and bottom), and display the results
  Panorama pano = Panorama(1366, 768, 4., -4.);
  CImgDisplay fpDisp = pano.showFirstPerson();
  CImgDisplay normDisp = pano.showNormals();
  // while at least one display is still open
  while (!normDisp.is_closed() || !fpDisp.is_closed()) {
    // check if an arrowkey is pressed, and if so, move the first person view
    // accordingly
    if (!fpDisp.is_closed()) {
      if (fpDisp.is_keyARROWUP()) {
        pano.updateFirstPerson(-15., 0., &fpDisp);
      } else if (fpDisp.is_keyARROWDOWN()) {
        pano.updateFirstPerson(15., 0., &fpDisp);
      } else if (fpDisp.is_keyARROWLEFT()) {
        pano.updateFirstPerson(0., -15., &fpDisp);
      } else if (fpDisp.is_keyARROWRIGHT()) {
        pano.updateFirstPerson(0., 15., &fpDisp);
      // quit the program if q is pressed in first person view
      } else if (fpDisp.is_keyQ()) {
        break;
      }
    }
    // wait for an event
    CImgDisplay::wait_all();
  }
  return 0;
}
