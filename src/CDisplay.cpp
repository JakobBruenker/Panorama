#include <CImg.h>

#include "CDisplay.h"

using namespace cimg_library;

CDisplay::CDisplay(CImgDisplay disp)
  : display(disp)
{}

bool CDisplay::is_closed() {
  return display.is_closed();
}

void CDisplay::wait() {
  display.wait();
}

void CDisplay::wait(CDisplay disp) {
	CImgDisplay::wait(disp.display);
}

void CDisplay::wait(CDisplay disp1, CDisplay disp2) {
	CImgDisplay::wait(disp1.display, disp2.display);
}

void CDisplay::wait(CDisplay disp1, CDisplay disp2, CDisplay disp3) {
	CImgDisplay::wait(disp1.display, disp2.display, disp3.display);
}

void CDisplay::
  wait(CDisplay disp1, CDisplay disp2, CDisplay disp3, CDisplay disp4) {
	CImgDisplay::
    wait(disp1.display, disp2.display, disp3.display, disp4.display);
}

void CDisplay::wait_all() {
  CImgDisplay::wait_all();
}
