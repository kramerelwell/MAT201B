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

Vec3f sunDist(float scale, float distance) {
  return Vec3f(distance, 0, 0) * scale;
}

string slurp(string fileName);  



struct AlloApp : App {
  Parameter pointSize{"/pointSize", "", 4.0, "", 0.0, 5.0};
  Parameter timeStep{"/timeStep", "", 0.1, "", 0.01, 3.0};
  Parameter symmetry{"/symmetry", "", 0.5, "", 0.0, 1.0}; 
  ControlGUI gui;

  ShaderProgram pointShader;
  Mesh mesh;  

  vector<Vec3f> velocity;
  vector<Vec3f> acceleration;
  vector<Vec3f> gravitation;
  vector<float> mass;

  float initV = 1;
  const float gravCon = 1.190588106 * pow(10, -8);

  vector<float> planetM     {330000, 317.83, 95.162, 14.536, 17.147, 1, 0.815, 0.107, 0.0553, 0.0022};  // Earth Mass       
  vector<float> planetD     {0, 5.20, 9.58, 19.2, 30.1, 1, 0.722, 1.52, 0.387, 39.5};                   // AU Dist



  void reset() {
    mesh.reset();
    velocity.clear();
    acceleration.clear();

    auto rc = []() { return HSV(rnd::uniform(), 1.0f, 1.0f); };

    for (int i = 0; i < 500; i++) {
      float d = (i < 10 ? planetD[i] : rnd::uniform(30.0)+1);                           // set position
      mesh.vertex(sunDist(1.0, d));                                            


      float m = 0;                                                                      // set mass
      if (i < 10) { m = planetM[i]; }                                                   
      else if (10 <= i <= 100) { m = rnd::uniform(0.00001) + 0.00001;} 
      else if (100 < i <= 250) { m = rnd::uniform(0.0000077) + 0.0000077;} 
      else if (250 < i <= 500) { m = rnd::uniform(0.00000001) + 0.00000001;}
      mass.push_back(m);

      if (i == 0) {                                                                     // set size and color
        mesh.texCoord(pow(planetM[i], 1.0 / 7), 0);     
        mesh.color(RGB(1,1,1));
      }
      else if (0 < i < 10) {
        mesh.color(rc());
        float size = pow(planetM[i], 1.0 / 3.0);
        if (size < 2.5 ? size = 2.5 : size);
        mesh.texCoord(size, 0); 
      }
      else if (i >= 10) {
        mesh.color(RGB(0.5,0.5,0.5));
        float size = pow(m, 1.0 / 3.0);
        if (size < 1 ? size = 1 : size);
        mesh.texCoord(size, 0); 
      }
    
      // (2 * M_PI * planetD[i]) / days[i]
      // sqrt((gravCon * planetM[i]) / planetD[i])

      Vec3f initV(0, sqrt((gravCon * planetM[0]) / d), 0);                            // set Veloicty
      velocity.push_back(initV);
 
      acceleration.push_back(0);                                                      // set acceleration
    }
  }
                           
  void onCreate() override {

    gui << pointSize << timeStep << symmetry;
    gui.init();
    navControl().useMouse(false);

    pointShader.compile(slurp("../point-vertex.glsl"),
                        slurp("../point-fragment.glsl"),
                        slurp("../point-geometry.glsl"));

    mesh.primitive(Mesh::POINTS);
    reset();
    nav().pos(0, 0, 10);
  }


  bool pause = false;
  void onAnimate(double dt) override {
    if (pause) return;

    dt = timeStep;

    // Gravitation
    for (int i = 0; i < mass.size(); i++){
      for (int k = i + 1; k < planetM.size(); k++){
        Vec3f r = mesh.vertices()[i] - mesh.vertices()[k]; 
        //  Vec3f rNorm = r.normalize();
        double rDist = pow(r.mag(), 3.0);  
        if (rDist == 0) continue;

        Vec3f gravitation = gravCon * mass[i] * mass[k] * r / rDist;

        acceleration[i] -= gravitation * ((1 - symmetry) * 2);
        acceleration[k] += gravitation * (symmetry * 2);
        }
    }

    // // Drag
    // for (int i = 0; i < velocity.size(); i++) {
    //  acceleration[i] -= velocity[i] * dragFactor;
    // }

    // for (int i = 0; i < acceleration.size(); i++) {
    //   float m = acceleration[i].mag();
    //   if (m > maxAccel) {
    //     acceleration[i].normalize(maxAccel);
    //   }
    // }

    vector<Vec3f>& position(mesh.vertices());
    for (int i = 0; i < velocity.size(); i++) {
      if (i == 0) {velocity[i] = Vec3f(0,0,0);}
      else {
        velocity[i] += acceleration[i] / mass[i] * dt;
        position[i] += velocity[i] * dt;
      }
    }

    for (auto& a : acceleration) a.zero();
  }


  bool onKeyDown(const Keyboard& k) override {
    if (k.key() == ' ') {
      pause = !pause;
    }
     if (k.key() == 'r') {
      reset();
    }

    if (k.key() == '1') {
      for (int i = 0; i < velocity.size(); i++) {
        acceleration[i] = rv(1) / mass[i];
      }
    }
    return true;
  }


  void onDraw(Graphics& g) override {
    g.clear(0.1);
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