#ifndef VisualControl_h 
#define VisualControl_h

#include <G3D/G3DAll.h>
#include "DemoScene.h" 

#include "Map.h" 
#include "Animation.h" 
class VisualControl{

 protected:

    shared_ptr<Map> m_map;

std::vector< shared_ptr<Sound> > deathSounds; 
std::vector< shared_ptr<Sound> > hitSounds; 

    std::vector< shared_ptr<Animation> > allAnimations; 


 public:

int dimX = 15;
int dimY = 15; 
int dimZ = 15; 

 float updateRate; 
    
    int gridOccupancy[15][15][15]; 
    
    shared_ptr<DemoScene> m_scene; 
     
    static shared_ptr<VisualControl> create(shared_ptr<DemoScene> scene, float updateRate, shared_ptr<Map> map);    
    VisualControl(shared_ptr<DemoScene> scene, float updateRate, shared_ptr<Map> map); 

    void update( std::vector< shared_ptr<Change> > allChanges ); 

void step(); 
};

#endif 
