#include "al/app/al_App.hpp"
#include "al/math/al_Random.hpp"
#include "al/ui/al_ControlGUI.hpp"  

using namespace al;

#include <fstream>
#include <vector>
using namespace std;


Vec3f rv(float scale = 1.0f) {
  return Vec3f(rnd::uniformS(), rnd::uniformS(), rnd::uniformS()) * scale;
}

string slurp(string fileName);  

struct AlloApp : App {
    Parameter moveRate{"/moveRate", "", 0.2, "", 0.0, 2.0};
    Parameter turnRate{"/turnRate", "", 1.0, "", 0.0, 2.0};
    Parameter localRadius{"/localRadius", "", 0.4, "", 0.01, 0.9};
    Parameter sepMod{"/sepMod", "", 0.02, "", 0.01, 0.3};
    Parameter lignMod{"/lignMod", "", 0.02, "", 0.01, 0.3};
    Parameter heseMod{"/heseMod", "", 0.02, "", 0.01, 0.3};
    Parameter pointSize{"/pointSize", "", 4.0, "", 0.0, 10.0};
    ControlGUI gui;

    ShaderProgram shader;
    Mesh mesh;

    vector<Pose> agent;

    void onCreate() override {
        gui << moveRate << turnRate << localRadius << sepMod << lignMod << heseMod << pointSize;
        gui.init();
        navControl().useMouse(false);

        shader.compile( slurp("../tetrahedron-vertex.glsl"),
                        slurp("../tetrahedron-fragment.glsl"),
                        slurp("../tetrahedron-geometry.glsl"));

        mesh.primitive(Mesh::POINTS);

        for (int i = 0; i < 500; i++) {
            Pose p;
            p.pos(rv());
            p.faceToward(rv());
            agent.push_back(p);

            mesh.vertex(p.pos());
            mesh.normal(p.uf());
            Vec3f up(p.uu());
            mesh.color(up.x, up.y, up.z);
        }
        nav().pos(0, 0, 5);
    }

    void onAnimate(double dt) override {

        struct Data {
            Vec3f heading, center, diff;
            unsigned neighborCount{0};
            float distance{0};
        };

        vector<Data> data;
        data.resize(agent.size());

        int N = agent.size();
        for (unsigned i = 0; i < N; i++){ 
            for (unsigned j = 1 + i; j < N; j++) {

                float distance = (agent[j].pos() - agent[i].pos()).mag(); 

                Vec3f separation, alignment, cohesed, sumP, sumV;    

                if (distance < localRadius) {         
                    data[i].heading = agent[j].uf();
                    data[j].heading = agent[i].uf();

                    data[i].center += agent[j].pos();
                    data[j].center += agent[i].pos();

                    data[i].neighborCount++;
                    data[j].neighborCount++;

                    if (distance < 2 ? distance = 2 : NULL){
                    agent[i].faceToward(agent[j].pos(), (-1/distance)*sepMod);
                    agent[j].faceToward(agent[i].pos(), (-1/distance)*sepMod);
                    }
                }
            }
        }

        for (unsigned i = 0; i < N; i++) {
            if (data[i].neighborCount > 0) {
                
                data[i].heading /= data[i].neighborCount;
                agent[i].faceToward(data[i].heading, lignMod);

                data[i].center /= data[i].neighborCount;
                agent[i].faceToward(data[i].center, heseMod);
            
                agent[i].pos() += agent[i].uf() * moveRate * 0.002 * data[i].neighborCount;                           
            }
            agent[i].pos() += agent[i].uf() * moveRate * 0.001; 

            data[i].center = Vec3f(0,0,0);
            data[i].heading = Vec3f(0,0,0);
            data[i].neighborCount = 0;
        }
    
        vector<Vec3f>& m(mesh.vertices());                                                             
        vector<Vec3f>& n(mesh.normals());
        vector<Color>& c(mesh.colors());
        for (unsigned i = 0; i < N; i++) {
            m[i] = agent[i].pos();
            n[i] = agent[i].uf();
            const Vec3d& up(agent[i].uu());
            c[i].set(up.x, up.y, up.z);
        }
    }

    void onDraw(Graphics& g) override {
        g.clear(0.1, 0.1, 0.1);
        gl::blending(true);
        gl::blendTrans();
        g.shader(shader);
        g.shader().uniform("size", pointSize / 100);
        g.shader().uniform("ratio", 0.2);
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
