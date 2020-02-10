#include "al/app/al_App.hpp"
#include "al/math/al_Random.hpp"
#include "al/ui/al_ControlGUI.hpp"  
using namespace al;

#include <fstream>
#include <vector>
using namespace std;

string slurp(string fileName);  


vector<Vec3f> boids;
vector<Vec3f> velocity, acceleration;
float r;
float prevX, prevY;
float maxSpeed, maxForce;

float maxSpeed(9);
float maxForce(0.2);
float size(rnd::uniform(16, 8));



Vec3f seek(Vec3f target, Vec3f start, Vec3f velocity) {
    Vec3f desired = target -= start;
    desired.normalize();
    desired *= maxSpeed;
    Vec3f steer(desired -= velocity);
    if (steer > maxForce) steer = maxForce;
    else steer *= 1; 
    return steer;
}
        

Vec3f separate (vector<Vec3f> particles) {
    float desiredseparation = 1.0;
    Vec3f steer(0, 0, 0);
    int count = 0;
            
    for (int i = 0; i < particles.size(); i++) {
        for (int j = 0; j < particles.size(); j++) {
            
            float d = dist(particles[i], particles[j]);

            if ((d > 0) && (d < desiredseparation)) {
                Vec3f diff(particles[i] - particles[j]);
                diff.normalize();
                diff /= d;        
                steer += diff;
                count++;
            }
        }
                
        if (count > 0) {
            steer /= float(count);
        }

        if (steer.mag() > 0) {
            steer.normalize();
            steer *= maxSpeed;
            steer -= velocity[i];
            if (steer > maxForce) steer = maxForce;
            else steer *= 1;
        }

        return steer;
    }
}

    
Vec3f align(vector<Vec3f> particles) {
    float neighbordist = 1.0;
    Vec3f sum(0, 0, 0);
    int count = 0;

    for (int i = 0; i < particles.size(); i++) {
        for (int j = 0; j < particles.size(); j++) {
            float d = dist(particles[i], particles[j]);
            if ((d > 0) && (d < neighbordist)) {


                sum += velocity[j];


            count++;
            }
        }

    if (count > 0) {
        sum /= float(count);
        sum.normalize();
        sum *= maxSpeed;
        Vec3f steer(sum - velocity[i]);
        if (steer > maxForce) steer = maxForce;  
        else steer *= 1; 
        return steer;
    } 
    else return new Vec3f(0, 0, 0); 
    }
}
    

Vec3f cohesion (vector<Vec3f> particles) {
    float neighbordist = 5;
    Vec3f sum(0,0,0);   
    int count = 0;

    for (int i = 0; i < particles.size(); i++) {
        for (int j = 0; j < particles.size(); j++) {
            float d = dist(particles[i], particles[j]);
            if ((d > 0) && (d < neighbordist)) {
                sum += particles[j]; 
                count++;
            }
        }

        if (count > 0) {
            sum /= count;
            Vec3f cohes(seek(sum, particles[i], velocity[i]));
            return cohes;
        } else return new Vec3f(0, 0, 0);
    }
}


void flock(vector<Vec3f> particles) {
    Vec3f separation = separate(particles);  
    Vec3f alignment = align(particles);     
    Vec3f cohesed = cohesion(particles);   

    separation *= 2.3;                                              // Separation Scaling
    alignment *= 0.8;                                               // Alignment Scaling
    cohesed *= 2;                                                   // Cohesion Scaling

    acceleration += (separation + alignment + cohesed);
}


void wrap(vector<Vec3f> particles) {
    for (int i = 0; i < particles.size(); i++) {
        if ((particles[i].x < -10) || (particles[i].x > 10)) velocity[i].x *= -1;
        if ((particles[i].y < -10) || (particles[i].y > 10)) velocity[i].y *= -1;
        if ((particles[i].z < -10) || (particles[i].z > 10)) velocity[i].z *= -1;
        }
    }
}

// void update(vector<particle> particles) {
//     flock(particles);
//     velocity += acceleration;
//     if (velocity > maxSpeed) velocity = maxSpeed;
//     else velocity *= 1;
//     position += velocity;
// }  

// void run(vector<particle> particles) {

// }


struct AlloApp : App {
  Parameter pointSize{"/pointSize", "", 4.0, "", 0.0, 5.0};
  ControlGUI gui;

  ShaderProgram pointShader;
  Mesh mesh;  

  vector<Vec3f> velocity;
  vector<Vec3f> acceleration;
  vector<Vec3f> particles;




    void reset() {
        mesh.reset();
        velocity.clear();
        acceleration.clear();

        auto rc = []() { return RGB(rnd::uniform()*255, rnd::uniform()*255, rnd::uniform()*255); };

        for (int i = 0; i < particles.size(); i++) {
            mesh.vertex(p.position);                    // set position                                            
            mesh.texCoord(5);                           // set size
            mesh.color(rc());                           // set color               
            velocity.push_back(p.velocity);             // set Veloicty
            acceleration.push_back(p.acceleration);     // set acceleration
        }
    }

      
  void onCreate() override {
    gui << pointSize;
    gui.init();
    navControl().useMouse(false);

    pointShader.compile(slurp("../point-vertex.glsl"),
                        slurp("../point-fragment.glsl"),
                        slurp("../point-geometry.glsl"));

    mesh.primitive(Mesh::POINTS);


    for (int i = 0; i < 300; i++){
 
    }

    nav().pos(0, 0, 10);
  }





  bool pause = false;
  void onAnimate(double dt) override {
    if (pause) return;

    
    vector<Vec3f>& position(mesh.vertices());
    for (int i = 0; i < mesh.vertices().size(); i++) {
        velocity[i] += acceleration[i];
        position[i] += velocity[i];
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