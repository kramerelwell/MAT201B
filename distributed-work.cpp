// Remix pulled from Rodney DuPlessis particles_p1.cpp
// from commit ""

#include "al/app/al_App.hpp"
#include "al/math/al_Random.hpp"
#include "al/ui/al_ControlGUI.hpp" // gui.draw(g)
#include "al_ext/statedistribution/al_CuttleboneStateSimulationDomain.hpp"

using namespace al;

#include <fstream>
#include <vector>
using namespace std;


Vec3f rv(float scale) {
  return Vec3f(rnd::uniformS(), rnd::uniformS(), rnd::uniformS()) * scale;
}

string slurp(string fileName); 


// Even though we aren't using orientation here...
struct Agent : Pose {
};

struct DrawableAgent {
  Vec3f position;

  void from(const Agent& agent) {
    position.set(agent.pos());
  }
};

#define N (5)
struct SharedState {
  Pose cameraPose;
  float background;
  float pointSize, timeStep, drag;
  DrawableAgent agent[N];
};



struct AlloApp : public DistributedAppWithState<SharedState> {
    Parameter pointSize{"/pointSize", "", 1.0, "", 0.0, 2.0};
    Parameter timeStep{"/timeStep", "", 0.01, "", 0.01, 1.9};
    Parameter drag{"/drag", "", 0.01, "", 0, 1};

    ControlGUI gui;

    ShaderProgram pointShader;

    Mesh mesh;     
    Texture trail; 

    vector<Agent> agent;

    vector<Vec3f> velocity;
    vector<Vec3f> acceleration;
    vector<float> mass;
    float bgColor;
    const double GM = 398600.4418; 

    std::shared_ptr<CuttleboneStateSimulationDomain<SharedState>>
        cuttleboneDomain;



    void onCreate() override {
        cuttleboneDomain =
            CuttleboneStateSimulationDomain<SharedState>::enableCuttlebone(this);
        if (!cuttleboneDomain) {
        std::cerr << "ERROR: Could not start Cuttlebone. Quitting." << std::endl;
        quit();
        }

        gui << pointSize << timeStep << drag;
        gui.init();
        navControl().useMouse(false);

        trail.filter(Texture::LINEAR);
        pointShader.compile(slurp("../point-vertex.glsl"),
                            slurp("../point-fragment.glsl"),
                            slurp("../point-geometry.glsl"));


        mesh.primitive(Mesh::POINTS);

    for (int _ = 0; _ < N; _++) {
        float m = 1; 
        agent[_].pos(rv(5));

        mesh.vertex(agent[_].pos());
        mesh.color(HSV(0.41, 1.0, 0.851));
        mass.push_back(m);
        mesh.texCoord(pow(m, 1.0f / 3), 0); 

        velocity.push_back(rv(0.1));
        acceleration.push_back(rv(1));
    }
    nav().pos(0, 0, 10);
  }

    
    void onAnimate(double dt) override {
        if (cuttleboneDomain->isSender()) {
            int count = 0;
            dt = timeStep;

            bgColor += dt;
            if (bgColor > 1){  
            bgColor -= 1;
            count++;
            }

            for (int i = 0; i < N; i++) {
                acceleration[i] -= velocity[i] * drag;
                
                for (int j = i + 1; j < N; j++) {
                    Vec3f r = mesh.vertices()[i] - mesh.vertices()[j];
                    Vec3d rhat = r.normalize();
                    double distsqr = r.magSqr() * 1000000000;
                    acceleration[i] -= ((GM) / distsqr) * rhat;
                    acceleration[j] += ((GM) / distsqr) * rhat;
                }
            }

            for (int i = 0; i < velocity.size(); i++) {
                velocity[i] += acceleration[i] / mass[i] * dt;
                agent[i].pos() += velocity[i] * dt;
                mesh.vertices()[i] = agent[i].pos();
            }

            for (auto &a : acceleration)
            a.zero();


            // COPY AGENT POSITIONS TO SHARED STATE
            for (unsigned i = 0; i < N; i++) {
                state().agent[i].from(agent[i]);
            }
            state().cameraPose.set(nav());
            state().background = bgColor;
            state().pointSize = pointSize.get();
            state().timeStep = timeStep.get();
            state().drag = drag.get();

        } else {
            nav().set(state().cameraPose);
        }



        // EVERYONE ASSIGNS POSITION AND COLOR TO VECTICES HERE
        vector<Vec3f>& position(mesh.vertices());
        vector<Color>& c(mesh.colors());
        for (int i = 0; i < N; i++) {
            position[i] = state().agent[i].position;
            c[i].set(rnd::uniform(0.0,1.0), rnd::uniform(0.0,1.0), rnd::uniform(0.0,1.0));
        }
    }



    void onDraw(Graphics &g) override {
        float f = state().background;
        g.clear(f, f, f);
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

        if (cuttleboneDomain->isSender()) {
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