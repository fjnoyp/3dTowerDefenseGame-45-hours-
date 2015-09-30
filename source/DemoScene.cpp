#include "DemoScene.h"
//#include "PlayerEntity.h"
#include "G3D/Random.h"
shared_ptr<DemoScene> DemoScene::create() {
    return shared_ptr<DemoScene>(new DemoScene());
}


shared_ptr<VisibleEntity> DemoScene::spawnVisibleEntity( String name, String modelName, CFrame cFrame ) {

    shared_ptr<VisibleEntity> ve = VisibleEntity::create( name, this, m_modelTable[modelName], cFrame ); 

    insert( ve ); 
    return ve; 
}

void DemoScene::removeVisibleEntity( shared_ptr<VisibleEntity> ve ){
    remove( ve ); 
}

//shared_ptr<Light> DemoScene::addLight( name, this, 
