#ifndef CDISPLAY_H
#define CDISPLAY_H

struct CDisplay {
  private:
    cimg_library::CImgDisplay display;

  public:
    CDisplay(cimg_library::CImgDisplay disp);
    bool is_closed();
    void wait();
    static void wait(CDisplay disp);
    static void wait(CDisplay disp1, CDisplay disp2);
    static void wait(CDisplay disp1, CDisplay disp2, CDisplay disp3);
    static void
      wait(CDisplay disp1, CDisplay disp2, CDisplay disp3, CDisplay disp4);
    static void wait_all();
};


#endif /* CDISPLAY_H */
