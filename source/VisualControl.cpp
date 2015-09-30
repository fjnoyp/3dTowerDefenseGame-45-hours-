#include "VisualControl.h"
#include "Animation.h" 


VisualControl::VisualControl(shared_ptr<DemoScene> scene, float updateRate, shared_ptr<Map> map){

    m_scene = scene;
    m_map = map;
    this->updateRate = updateRate; 


    shared_ptr<Sound> death0 = G3D::Sound::create(System::findDataFile("death0.wav",false));
    shared_ptr<Sound> death1 = G3D::Sound::create(System::findDataFile("death1.aiff",false));
    shared_ptr<Sound> death2 = G3D::Sound::create(System::findDataFile("death2.aiff",false));

    deathSounds.push_back(death0); 
    deathSounds.push_back(death1); 
    deathSounds.push_back(death2); 

    shared_ptr<Sound> hit0 = G3D::Sound::create(System::findDataFile("hit0.wav",false));
    shared_ptr<Sound> hit1 = G3D::Sound::create(System::findDataFile("hit1.wav",false));
    shared_ptr<Sound> hit2 = G3D::Sound::create(System::findDataFile("hit2.wav",false));

    hitSounds.push_back(hit0); 
    hitSounds.push_back(hit1); 
    hitSounds.push_back(hit2); 


}

shared_ptr<VisualControl> VisualControl::create(shared_ptr<DemoScene> scene, float updateRate, shared_ptr<Map> map){
    return shared_ptr<VisualControl>(new VisualControl(scene,updateRate,map)); 
}

void VisualControl::step(){
    //Animation Objects 
    for(int i = 0; i<(int)allAnimations.size(); i++){
        if( allAnimations[i]->step() < 0 ){
            m_scene->removeVisibleEntity( allAnimations[i]->ve ); 
            allAnimations.erase( allAnimations.begin() + i); 
        }
    }
}

void VisualControl::update( std::vector< shared_ptr<Change> > allChanges ){
    for(int i = 0; i < (int)allChanges.size(); i++){
        if( allChanges[i]->newHP <= 0 ){


            //NOTE: we want this code here so we can animate on 0 hp events 
            if( allChanges[i]->type == Creature::type || allChanges[i]->type == Cube::type){

                deathSounds[ rand() % deathSounds.size() ]->play(1.0); 
                //death->play(1.0);
                        
                Vector3 point = allChanges[i]->oldPos; 

                std::vector<shared_ptr<Animation> > newAnim = Animation::explosion( "explosionModel", point, 3000, 5, 2, m_scene); 

                for(int i = 0; i< (int)newAnim.size(); ++i){
                    allAnimations.push_back( newAnim[i] ); 
                }

        }

            m_scene->removeVisibleEntity(allChanges[i]->ve); 

            continue; 
            //allChanges.erase(allChanges.begin()+i);
        }
        //changes to cube will require update to its AABox for ray casting (for placing more cubes) 
        if(allChanges[i]->type == Cube::type){

            //Move the cube 
            allChanges[i]->ve->setFrame( allChanges[i]->ve->frame() + allChanges[i]->displacement/updateRate );
        }
        else{


            if( allChanges[i]->newHP < allChanges[i]->oldHP && allChanges[i]->type == Creature::type){

                Vector3 oldPos = allChanges[i]->oldPos; 

        float randX((rand()%20 - 10.0) / 10.0f); 
        float randY = (rand()%10) / 10.0f; 
        float randZ = (rand()%20 - 10.0) / 10.0f; 

        Vector3 randPos(Vector3(randX,randY,randZ)); 

        allAnimations.push_back( shared_ptr<Animation>(new Animation( m_scene->spawnVisibleEntity("hurt","bulletModel",CFrame::fromXYZYPRDegrees(oldPos.x,oldPos.y,oldPos.z,0,0,0)), randPos, 10))); 

        allChanges[i]->displacement = allChanges[i]->displacement+randPos/5.0f; 

                hitSounds[ rand() % hitSounds.size() ]->play(1.0); 

            }

            allChanges[i]->ve->setFrame( allChanges[i]->ve->frame() + allChanges[i]->displacement/updateRate );

            /* old code to make creatures crawl and face in the right direction 
            if(allChanges[i]->type == Creature::type){
                //Make face in right direction 
                Vector3 point = allChanges[i]->ve->frame().translation + allChanges[i]->displacement ; 
                point = point + Vector3(0, ((rand()%2)-.5f)/50.0f, 0); 
                CoordinateFrame frame(allChanges[i]->ve->frame()); 
                frame.lookAt( point );
                allChanges[i]->ve->setFrame( frame ); 

            }
            */

        }
    }
}

