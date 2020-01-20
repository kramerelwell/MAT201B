#include "al/app/al_App.hpp"
#include "al/graphics/al_Image.hpp"  // al::Image
#include "al/math/al_Random.hpp"
#include "al/ui/al_ControlGUI.hpp"  // gui.draw(g)

using namespace al;

#include <fstream>
#include <vector>
using namespace std;

string slurp(string fileName) {
  fstream file(fileName);
  string returnValue = "";
  while (file.good()) {
    string line;
    getline(file, line);
    returnValue += line + "\n";
  }
  return returnValue;
}

struct AlloApp : App {
  Parameter pointSize{"/pointSize", "", 0.46, "", 0.0, 1.0};
  ControlGUI gui;
  ShaderProgram pointShader;
  Mesh mesh;
  vector<Vec3f> coordinates[4];

  void onCreate() override {
    gui << pointSize;
    gui.init();
    navControl().useMouse(false);


    pointShader.compile(slurp("../point-vertex.glsl"),
                        slurp("../point-fragment.glsl"),
                        slurp("../point-geometry.glsl"));

    mesh.primitive(Mesh::POINTS);

    const char* filename = "../color_spaces.png";
    auto imageData = Image(filename);
    if (imageData.array().size() == 0) {
      std::cout << "failed to load image" << std::endl;
      exit(1);
    }

    Image::RGBAPix pixel;
    const int W = imageData.width();
    const int H = imageData.height();

    for (int c = 0; c < W; c++)
      for (int r = 0; r < H; r++) {
        imageData.read(pixel, c, r);

        Color pixCol = RGB(pixel.r, pixel.g, pixel.b);
        HSV cTwo(pixCol);
        int radius = 600;
        
        Vec3f position((c - W / 2) * 0.005, (r - H / 2) * 0.005, 0);

        Vec3f cubePosition( (((pixel.r / 255.0) * 1000.0) - 500) * 0.005, 
                            (((pixel.g / 255.0) * 1000.0) - 500) * 0.005, 
                            (((pixel.b / 255.0) * 1000.0) - 500) * 0.005);

        Vec3f cylinderPosition( cTwo.s * (radius * cos(cTwo.h * M_2PI)) * 0.005,
                                cTwo.s * (radius * sin(cTwo.h * M_2PI)) * 0.005,
                                ((cTwo.v * 2) * 0.005) - 3);

        /*
            Take a tour around this one... it almost lookslike a conical "splattering" on the outside of a cube
            like a spatial inverse to the cube... Looked cool enough... my machine runs to slow to experiment
            a whole lot with this... 
        */
       
        Vec3f customPosition(   (cTwo.s * ((4.5 * pixel.r) * cos(cTwo.h * M_2PI)) * 0.005) - 2,
                                (cTwo.s * ((4.5 * pixel.g) * sin(cTwo.h * M_2PI)) * 0.005) - 2,
                                cTwo.s * ((4.5 * pixel.b) * sin(cTwo.h * M_2PI)) * 0.005);
                                
        coordinates[0].push_back(position);
        coordinates[1].push_back(cubePosition);
        coordinates[2].push_back(cylinderPosition);
        coordinates[3].push_back(customPosition);

        mesh.vertex(position);
        mesh.color(pixel.r / 255.0, pixel.g / 255.0, pixel.b / 255.0);
      }

    nav().pos(0, 0, 15);
  }

  int endPos = 0;
  int startPos = 0;

  double t = 0;
  void onAnimate(double dt) override {
      t += dt;
      if (t > 0 && t < 1) {
        for (int i = 0; i < mesh.vertices().size(); ++i)
          mesh.vertices()[i] = (coordinates[startPos][i] * (1 - t)) + (coordinates[endPos][i] * t);
    }
  }

/*
    Could use a confirmation here... Struggled with the interpolation and I dont know if its a product
    of my graphics system being out of date, or if its because there aren't enough frames in the draw 
    loop for the points to get to their destination and there is a fraction of a step left to travel... 
    but my points don't always make it back home after interpolating.
*/


  bool onKeyDown(const Keyboard& k) override {
    if (k.key() == '1') {
      t = 0;
      startPos = endPos;
      endPos = 0;
    }
    if (k.key() == '2') {
      t = 0;
      startPos = endPos;
      endPos = 1;
    }
    if (k.key() == '3') {
      t = 0;
      startPos = endPos;
      endPos = 2;
    }
    if (k.key() == '4') {
      t = 0;
      startPos = endPos;
      endPos = 3;
    }
    return true;
  }

  void onDraw(Graphics& g) override {
    g.clear(0.88);
    g.shader(pointShader);
    g.shader().uniform("pointSize", pointSize / 100);
    g.depthTesting(true);
    g.draw(mesh);
    gui.draw(g);
  }
};

int main() { AlloApp().start(); }