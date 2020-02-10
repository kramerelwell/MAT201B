#include "al/app/al_App.hpp"
#include "al/math/al_Random.hpp"
#include "al/ui/al_ControlGUI.hpp"  
using namespace al;

#include <fstream>
#include <vector>
using namespace std;

string slurp(string fileName);  


struct particle : Vec3f {
    Vec3f position;
    Vec3f velocity;
    Vec3f acceleration;
    float r;
    float prevX, prevY;
    float maxSpeed, maxForce;

    


    particle(float _x, float _y, float _z) {
        float maxSpeed(9);
        float maxForce(0.2);
        float size(rnd::uniform(16, 8));
        float sat(rnd::uniform(1.0, 0.5));
              
        Vec3f position(x, y, z);
        Vec3f acceleration(0,0,0);
        Vec3f velocity(rnd::uniform(1,1));
        Vec3f color(187*sat, 0, 0);
    }



    Vec3f seek(Vec3f target) {
        Vec3f desired = target -= this;
        desired.normalize();
        desired *= maxSpeed;
        Vec3f steer(desired -= velocity);
        steer.limit(maxForce);  
        return steer;
    }
        




    Vec3f separate (vector<particle> particles) {
        float desiredseparation = 25.0;
        Vec3f steer(0, 0, 0);
        int count = 0;
            

        for (auto& p : particles){
            float d = dist(this.position, p.position);
            if ((d > 0) && (d < desiredseparation)) {
                Vec3f diff(this.position - p.position);
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
            steer -= velocity;
            steer.limit(maxForce);
        }
        return steer;
    }

    

    Vec3f align(vector<particle> particles) {
        float neighbordist = 50;
        Vec3f sum(0, 0, 0);
        int count = 0;

        for (auto& p : particles) {
            float d = dist(this.position, p.position);
            if ((d > 0) && (d < neighbordist)) {
                sum += p.velocity;
                count++;
            }
            if (count > 0) {
                sum /= float(count);
                sum.normalize();
                sum *= maxSpeed;
                Vec3f steer(sum - this.velocity);
                if (maxForce < steer) { steer = maxForce; }
                else (steer < maxForce) { steer; }
                return steer;
            } 
            else { return new Vec3f(0, 0, 0); }
        }
    }
    



    Vec3f cohesion (vector<particle> particles) {
        float neighbordist = 50;
        Vec3f sum(0,0,0);   
        int count = 0;

        for (auto& other : particles) {
            float d = dist(Vec3f(this), other.position);
            if ((d > 0) && (d < neighbordist)) {
                sum += other.position; 
                count++;
            }
        }
        
        if (count > 0) {
            sum /= count;
            Vec3f cohes(seek(sum));
            return cohes;
        } else {
            return new Vec3f(0, 0, 0);
        }
    }


    void flock(vector<particle> particles) {
        Vec3f separation = separate(particles);  
        Vec3f alignment = align(particles);     
        Vec3f cohesed = cohesion(particles);   

        separation *= 2.3;                                              // Separation Scaling
        alignment *= 0.8;                                               // Alignment Scaling
        cohesed *= 2;                                                   // Cohesion Scaling

        acceleration += separation;
        acceleration += alignment;
        acceleration += cohesed;
    }


    void wrap(vector<particle> particles) {
        for (auto& p : particles) {
            if ((p.position.x < -50) || (p.position.x > 50)) p.velocity.x *= -1;
            if ((p.position.y < -50) || (p.position.y > 50)) p.velocity.y *= -1;
            if ((p.position.z < -50) || (p.position.z > 50)) p.velocity.z *= -1;
        }
    }


    void update(vector<particle> particles) {
        flock(particles);
        velocity += acceleration;
        velocity.limit(maxSpeed);
        position += velocity;
        for (auto& p : particles) {
            p.acceleration.zero();
        }
    }  

    void run(vector<particle> particles) {

    }
};


struct particleSystem {
    vector<particle> particles;

    void addParticle(particle p) {
        particles.push_back(p);
    }

    void run() {
        for (auto& p : particles) 
            p.run(particles);
    }
};



struct AlloApp : App {
  Parameter pointSize{"/pointSize", "", 4.0, "", 0.0, 5.0};
  ControlGUI gui;

  ShaderProgram pointShader;
  Mesh mesh;  
  particleSystem particleSystem;

  vector<Vec3f> velocity;
  vector<Vec3f> acceleration;




    void reset() {
        mesh.reset();
        velocity.clear();
        acceleration.clear();

        auto rc = []() { return RGB(rnd::uniform()*255, rnd::uniform()*255, rnd::uniform()*255); };

        for (auto& p : particles) {
            mesh.vertex(p.position);                    // set position                                            
            mesh.texCoord(5);                           // set size
            mesh.color(rc());                           // set color               
            velocity.push_back(p.velocity);             // set Veloicty
            acceleration.push_back(p.acceleration);     // set acceleration
        }
    }

    void update() {
        for (int i = 0; i < mesh.vertices().size(); i++)
            for (auto& p : particles) {
                mesh.vertices()[i] = p.position[i];
                acceleration[i] = p.acceleration[i];
                velocity[i] = p.velocity[i];
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
        particle p(0,0,0);
        particleSystem.addParticle(p);
    }

    nav().pos(0, 0, 10);
  }





  bool pause = false;
  void onAnimate(double dt) override {
    if (pause) return;

    particleSystem.run();
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