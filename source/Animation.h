#ifndef Animation_h
#define Animation_h

#include <G3D/G3DAll.h>
#include "DemoScene.h"

//Manages creation, deletion, and movement of a visible entity 
class Animation{
 protected:

Vector3 direction; 
int lifeTime; 


 public: 

    shared_ptr<VisibleEntity> ve; 
    int step(); 

    
Animation(shared_ptr<VisibleEntity> ve, Vector3 direction, int lifeTime);
static shared_ptr<Animation> create(shared_ptr<VisibleEntity> ve, Vector3 direction, int lifeTime);


 static std::vector<shared_ptr<Animation> > explosion(String modelName, Vector3 point, int duration, int severity, int speed, shared_ptr<DemoScene> m_scene); 
    
}; 

#endif 
