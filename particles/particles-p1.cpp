#include "al/app/al_App.hpp"
#include "al/math/al_Random.hpp"
#include "al/ui/al_ControlGUI.hpp"  

using namespace al;

#include <fstream>
#include <vector>
using namespace std;

Vec3f rv(float scale) {
  return Vec3f(rnd::uniformS(), rnd::uniformS(), rnd::uniformS()) * scale;
}

string slurp(string fileName);  // forward declaration


struct AlloApp : App {
  Parameter pointSize{"/pointSize", "", 1.0, "", 0.0, 2.0};
  Parameter timeStep{"/timeStep", "", 0.1, "", 0.01, 0.6};
  ControlGUI gui;

  ShaderProgram pointShader;
  Mesh mesh;  


  vector<Vec3f> velocity;
  vector<Vec3f> acceleration;
  vector<Vec3f> gravitation;
  vector<float> mass;

  float gravCon = 0.001;


  void onCreate() override {
    gui << pointSize << timeStep;
    gui.init();
    navControl().useMouse(false);

    pointShader.compile(slurp("../point-vertex.glsl"),
                        slurp("../point-fragment.glsl"),
                        slurp("../point-geometry.glsl"));

    auto rc = []() { return HSV(rnd::uniform(), 1.0f, 1.0f); };



    mesh.primitive(Mesh::POINTS);    
    for (int _ = 0; _ < 1000; _++) {
      mesh.vertex(rv(5));
      mesh.color(rc());

      float m = 1;
      mass.push_back(m);

      mesh.texCoord(pow(m, 1.0f / 3), 0);  

      velocity.push_back(rv(0.1));
      acceleration.push_back(rv(1));
    }
    nav().pos(0, 0, 10);
  }

  bool pause = false;
  void onAnimate(double dt) override {
    if (pause) return;

    dt = timeStep;

    // Gravitation
    for (int i = 0; i < acceleration.size() - 1; i++){
      for (int k = i+1; k < acceleration.size(); k++){
        Vec3f r = mesh.vertices()[i] - mesh.vertices()[k];
        Vec3f rNorm = r.normalize();
        double rDist = r.magSqr();

        Vec3f gravitation = rNorm * (gravCon * ((mass[i] * mass[i+1]) / rDist));

        acceleration[i] -= gravitation / mass[i];
        acceleration[k] += gravitation / mass[k];
        
        /* Karl's way
        Vec3f& a(mesh.vertices()[i]), b(mesh.vertices()[k]);
        Vec3f r = b - a;
        Vec3f gravitation = gravCon * (r / pow(r.mag(), 3));
        acceleration[i] += gravitation * mass[k];
        acceleration[k] -= gravitation * mass[i];
        */
      }
    }

    // Drag
    for (int i = 0; i < velocity.size(); i++) {
      acceleration[i] -= velocity[i] * 0.1;
    }

    vector<Vec3f>& position(mesh.vertices());
    for (int i = 0; i < velocity.size(); i++) {
      velocity[i] += acceleration[i] / mass[i] * dt;
      position[i] += velocity[i] * dt;
    }

    for (auto& a : acceleration) a.zero();
  }


  bool onKeyDown(const Keyboard& k) override {
    if (k.key() == ' ') {
      pause = !pause;
    }
    if (k.key() == '1') {
      for (int i = 0; i < velocity.size(); i++) {
        // F = ma
        acceleration[i] = rv(1) / mass[i];
      }
    }
    return true;
  }


  void onDraw(Graphics& g) override {
    g.clear(0.3);
    g.shader(pointShader);
    g.shader().uniform("pointSize", pointSize / 100);
    g.blending(true);
    g.blendModeTrans();
    g.depthTesting(true);
    g.draw(mesh);
    gui.draw(g);
  }
};


int main() { AlloApp().start(); }


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