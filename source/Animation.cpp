#include "Animation.h"

int Animation::step(){
    direction /= 1.6f; 
    ve->setFrame( ve->previousFrame().translation + direction ); 
    lifeTime --; 
    return lifeTime; 
}

Animation::Animation(shared_ptr<VisibleEntity> ve, Vector3 direction, int lifeTime){
    this->ve = ve; 
    this->direction = direction;
    this->lifeTime = lifeTime; 
}

shared_ptr<Animation> Animation::create(shared_ptr<VisibleEntity> ve, Vector3 direction, int lifeTime){
    return shared_ptr<Animation>(new Animation(ve,direction,lifeTime)); 
}

std::vector<shared_ptr<Animation> > Animation::explosion(String modelName, Vector3 point, int duration, int severity, int speed, shared_ptr<DemoScene> m_scene){
    std::vector<shared_ptr<Animation> > allAnim; 

    int halfSpeed = speed / 2; 
    int quarDur = duration / 4; 

    for(int i = 0; i<severity; i++){
        
        shared_ptr<VisibleEntity> ve = m_scene->spawnVisibleEntity("explosion","explosionModel", CFrame::fromXYZYPRDegrees(point.x,point.y,point.z,0,0,0) ); 

        //allAnim.push_back( Animation::create( ve, Vector3( (rand() % speed) - halfSpeed, (rand()/halfSpeed), (rand() % speed) - halfSpeed) , duration + rand() % quarDur ) ); 

        float randX((rand()%20 - 10.0) / 10.0f); 
        float randY = (rand()%10) / 10.0f; 
        float randZ = (rand()%20 - 10.0) / 10.0f; 

        allAnim.push_back( Animation::create( ve, Vector3( randX, randY, randZ), 12 ) ); 
    }

    return allAnim; 
}


