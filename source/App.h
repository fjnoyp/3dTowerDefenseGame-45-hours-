#ifndef App_h
#define App_h

#include "VisualControl.h" 
//#include "PlayerEntity.h"

#include "DemoScene.h"

#include <G3D/G3DAll.h>
#include <math.h>

#include "Map.h" 


class DemoScene;

class App : public GApp {
protected:

    Vector3 rotatePoint(float cx,float cy,float angle,Vector3 p);

    std::vector< shared_ptr<Change> > allChanges; 

    shared_ptr<Shader>            m_toonShader;
    shared_ptr<Framebuffer>  m_tmpbuffer;
    RealTime                m_lastLightingChangeTime;

    float cameraYaw = 0; 
    float curHeight = 10; 
    float curDist = 14; 
    float curAngle = 0; 

    int curTime = 0; 
    int updateRate = 1; 

    shared_ptr<VisualControl> vControl; 

    shared_ptr<DemoScene>   m_scene;    
    
    shared_ptr<Map> m_map;

    float randomFloat(float start, float end); 
    
    /** Called from onInit */
    void makeGUI();

    void spawnCeature();
    int spawnTimer = - 9999999; 
    int levelProgression = 0;

    int m_gold;
    int m_hp;
    int safeCount = 0;
    int oldHP;
    int flashCount = 0;

    shared_ptr<Sound> levelUp;
    shared_ptr<Sound> placeCube;


    
    bool pause = true;
    int menuPage = 0;

    void onHelp();

public:
    
    App(const GApp::Settings& settings = GApp::Settings());
    virtual void onInit() override;
    virtual void onUserInput(UserInput* ui) override;
    virtual void onGraphics3D(RenderDevice* rd, Array<shared_ptr<Surface> >& allSurfaces) override;

    virtual void onSimulation(RealTime rdt, SimTime sdt, SimTime idt) override;

    
};

#endif
