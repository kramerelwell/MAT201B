// Remix pulled from Rodney DuPlessis particles_p1.cpp
// from commit ""




#include "al/app/al_App.hpp"
#include "al/math/al_Random.hpp"
#include "al/ui/al_ControlGUI.hpp" // gui.draw(g)

using namespace al;

#include <fstream>
#include <vector>
using namespace std;

Vec3f rv(float scale) {
  return Vec3f(rnd::uniformS(), rnd::uniformS(), rnd::uniformS()) * scale;
}

string slurp(string fileName); 

struct AlloApp : App {
  Parameter pointSize{"/pointSize", "", 1.0, "", 0.0, 2.0};
  Parameter timeStep{"/timeStep", "", 0.01, "", 0.01, 1.9};
  Parameter drag{"/drag", "", 0.01, "", 0, 1};

  ControlGUI gui;

  ShaderProgram pointShader;
  Mesh mesh;     
  Texture trail; 

  vector<Vec3f> velocity;
  vector<Vec3f> acceleration;
  vector<float> mass;
  float bgColor;
  const double GM = 398600.4418; 

  void onCreate() override {

    gui << pointSize << timeStep << drag;
    gui.init();
    navControl().useMouse(false);

    trail.filter(Texture::LINEAR);
    pointShader.compile(slurp("../point-vertex.glsl"),
                        slurp("../point-fragment.glsl"),
                        slurp("../point-geometry.glsl"));

    auto rc = []() { return HSV(rnd::uniform(), 1.0f, 1.0f); };

    mesh.primitive(Mesh::POINTS);
    for (int _ = 0; _ < 888; _++) {
      mesh.vertex(rv(5));
    
      float sat = rnd::uniform(0.0,1.0);
      mesh.color(HSV(0.41, 1.0, 0.851));

      float m = 1; //
      mass.push_back(m);

      mesh.texCoord(pow(m, 1.0f / 3), 0); // s, t

      velocity.push_back(rv(0.1));
      acceleration.push_back(rv(1));
    }
    nav().pos(0, 0, 10);
  }

  bool freeze = false;
  void onAnimate(double dt) override {
    int count = 0;
    if (freeze)
      return;

    dt = timeStep;

    bgColor += dt;
    if (bgColor > 1){  
      bgColor -= 1;
      count++;
    }

    for (int i = 0; i < velocity.size(); i++) {
      acceleration[i] -= velocity[i] * drag;
      
      Color c(rnd::uniform(0.0,1.0), rnd::uniform(0.0,1.0), rnd::uniform(0.0,1.0));
      mesh.colors()[i].rgb() = c;
      
      for (int j = i + 1; j < velocity.size(); j++) {
        Vec3f r = mesh.vertices()[i] - mesh.vertices()[j];
        Vec3d rhat = r.normalize();
        double distsqr = r.magSqr() * 1000000000;
        acceleration[i] -= ((GM) / distsqr) * rhat;
        acceleration[j] += ((GM) / distsqr) * rhat;
      }
    }
 
    vector<Vec3f> &position(mesh.vertices());
    for (int i = 0; i < velocity.size(); i++) {
      velocity[i] += acceleration[i] / mass[i] * dt;
      position[i] += velocity[i] * dt;
    }

    for (auto &a : acceleration)
      a.zero();
  }

  bool onKeyDown(const Keyboard &k) override {
    if (k.key() == ' ') {
      freeze = !freeze;
    }

    if (k.key() == '1') {
      for (int i = 0; i < velocity.size(); i++) {
        acceleration[i] = rv(1) / mass[i];
      }
    }
    return true;
  }

  void onDraw(Graphics &g) override {
    g.clear(bgColor);
    trail.resize(fbWidth(), fbHeight());
    g.tint(0.96);
    g.quadViewport(trail);
    g.tint(1); 
    g.shader(pointShader);
    g.shader().uniform("pointSize", pointSize / 100);
    g.blending(true);
    g.blendModeTrans();
    g.depthTesting(true);
    g.draw(mesh);
    g.depthTesting(false); 
    trail.copyFrameBuffer(); 

    imguiBeginFrame();
    ParameterGUI::beginPanel("Control");
    ParameterGUI::drawParameter(&pointSize);
    ParameterGUI::drawParameter(&timeStep);
    ParameterGUI::drawParameter(&drag);
    ImGui::Text("Framerate %.3f", ImGui::GetIO().Framerate);
    ParameterGUI::endPanel();
    imguiEndFrame();
    imguiDraw();
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