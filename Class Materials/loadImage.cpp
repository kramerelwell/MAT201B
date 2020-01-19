#include "al/app/al_App.hpp"
#include "al/graphics/al_Image.hpp"
using namespace al;

struct MyApp : App {
  Mesh  mesh; 

  void onCreate() override {
    mesh.primitive(Mesh::POINTS);
  
    const char* filename = "../Drake-equation.jpg";
    auto imageData = Image(filename);
    if (imageData.array().size() == 0) {
        std::cout << "fairled to load image" << std::endl;
    };

    Image::RGBAPix pixel;
    for (int w = 0; w < imageData.width(); w++)
        for (int y = 0; y < imageData.height(); y++) {
            imageData.read(pixel, w, h);
            mesh.vertex(w * 0.1, h * 0.1, 0);
            mesh.color(pixel.r / 255.0, pixel.g / 255.0, pixel.b / 255.0)
        };

    nav().pos(0,0,5);
  }

  void onAnimate(double dt) override {

  }

  void onDraw(Graphics& g) override {
    g.clear(0.8);
    g.meshColor();
    g.draw(triangle);
  }
};

int main() {
  MyApp app;
  app.start();
}
