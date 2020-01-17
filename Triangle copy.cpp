#include "al/app/al_App.hpp"
using namespace al;

struct MyApp : App {
  Mesh  frame; 

  void onCreate() override {
    frame.primitive(Mesh::LINES);
    // Line from origin +1 to x 
    frame.vertex(0,0,0);
    frame.color(1,0,0);
    frame.vertex(1,0,0);
    frame.color(1,0,0);

    frame.vertex(0,0,0);
    frame.color(0,1,0);
    frame.vertex(0,1,0);
    frame.color(0,1,0);

    frame.vertex(0,0,0);
    frame.color(0,0,1);
    frame.vertex(0,0,1);
    frame.color(0,0,1);

    // set start location
    nav().pos(Vec3d(4.453048, 3.885736, 5.371391)); 
    nav().quat(Quatd(0.908733, -0.266079, 0.316129, 0.058902));
  }

  void onAnimate(double dt) override {
    // print view positions
    // nav().print();
  }

  void onDraw(Graphics& g) override {
    g.clear(0.8);
    g.meshColor();
    g.draw(frame);
    //
  }
};

int main() {
  MyApp app;
  app.start();
}
