#ifndef DemoScene_h
#define DemoScene_h

#include <G3D/G3DAll.h>


class DemoScene : public Scene {
protected:

public:

    static shared_ptr<DemoScene> create();
    
    shared_ptr<VisibleEntity> spawnVisibleEntity(String, String, CFrame); 
    void removeVisibleEntity( shared_ptr<VisibleEntity> ve ); 
    
};

#endif

