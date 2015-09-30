#include "Map.h"

bool Bullet::move(){
    turnsLeft = 1; 
    newHP -= 7; 
     
    // newPos = oldPos + IntVector3( (int) velocity.x, (int) velocity.y, (int) velocity.z); 
    return true; 
}

void Bullet::run(){
    move(); 
    aaBox = aaBox + displacement; 
}

void Bullet::takeDamage(){
    Vector3 curPos = entity->frame().translation; 
    curPos += Vector3(.5f,.5f,.5f); 

    if (m_map->hasBox[int(curPos.x)][int(curPos.y)][int(curPos.z)]){
        newHP = -10000000;
    }

    change = true; 
}

void Bullet::onDeath(){
    //Because App.cpp does not destroy this entity, we must manually remove it 
    //m_map->m_scene->removeVisibleEntity( entity ); 
}

bool BasicTower::move(){

    turnsLeft = 5;
    oldPos = newPos;
    oldHP = newHP;
    int x = oldPos.x;
    int y = oldPos.y;
    int z = oldPos.z;

    //Fall down 
    bool isFalling = Cube::checkIfFall(x,y,z); 
    if ( isFalling ){
        //take fall damage
        newHP -= 50; 

        m_map->hasBox[x][y][z] = false;
        m_map->hasBox[x][y-1][z] = true;
        newPos.y -= 1;
    }

    //Fire if not falling down 
    if(!isFalling){

        turnsLeft = 20; 

        int fireSpeed = 3;

        shared_ptr<MapElement> closestEntity; 
        float closestDist(100000); 
        float dist; 

        float xOffset(0);
        float zOffset(0);

        Vector3 curPos = this->entity->frame().translation; 

        shared_ptr<MapElement> entity = m_map->getClosestLOSEntity( Vector3( curPos.x+.6f, curPos.y , curPos.z), 10000, Creature::type ); 
        if( notNull(entity)){
            dist = abs((curPos - entity->entity->frame().translation).squaredMagnitude()); 

            if( closestDist > dist){
                closestEntity = entity;
                closestDist = dist;
                xOffset = 1.6f; 
            }

        }

        entity = m_map->getClosestLOSEntity( Vector3( curPos.x-.6f, curPos.y , curPos.z), 10000, Creature::type ); 
        if( notNull(entity)){
            dist = abs((curPos - entity->entity->frame().translation).squaredMagnitude()); 

            if( closestDist > dist){
                closestEntity = entity;
                closestDist = dist;
                xOffset = -.6f; 
            }

        }

        entity = m_map->getClosestLOSEntity( Vector3( curPos.x, curPos.y , curPos.z+.6f), 10000, Creature::type ); 
        if( notNull(entity)){
            dist = abs((curPos - entity->entity->frame().translation).squaredMagnitude()); 

            if( closestDist > dist){
                closestEntity = entity;
                closestDist = dist;
                zOffset = .6f; 
                xOffset = 0; 
            }

        }

        entity = m_map->getClosestLOSEntity( Vector3( curPos.x, curPos.y, curPos.z-.6f), 10000, Creature::type ); 
        if( notNull(entity)){
            dist = abs((curPos - entity->entity->frame().translation).squaredMagnitude()); 

            if( closestDist > dist){
                closestEntity = entity;
                closestDist = dist;
                zOffset = -.6f; 
                xOffset = 0; 
            }

        }

        if( notNull(closestEntity) && dist < 120){
            shot->play(1.0); 
            closestEntity->newHP = closestEntity->oldHP - 50;
            Vector3 curPos = this->entity->frame().translation; 

            m_map->addBullet( curPos.x+xOffset, curPos.y, curPos.z+zOffset, ( closestEntity->aaBox.center()- aaBox.center()).direction() * fireSpeed ); 
        }

        /*
        if( notNull( entity ) ){
            entity->newHP = entity->oldHP - 10; 
            m_map->addBullet( oldPos.x + 1, oldPos.y, oldPos.z, ( entity->aaBox.center()- aaBox.center()).direction() * fireSpeed ); 
        }

        entity = m_map->getClosestLOSEntity( Vector3( oldPos.x - .6f, oldPos.y, oldPos.z), 100, Creature::type); 

        if( notNull( entity ) ){
            entity->newHP = entity->oldHP - 10; 
            m_map->addBullet( oldPos.x - 1, oldPos.y, oldPos.z, ( entity->aaBox.center()- aaBox.center()).direction() * fireSpeed ); 
        }

        entity = m_map->getClosestLOSEntity( Vector3( oldPos.x, oldPos.y, oldPos.z + .6f), 100, Creature::type); 

        if( notNull( entity ) ){
            entity->newHP = entity->oldHP - 10; 
            m_map->addBullet( oldPos.x, oldPos.y, oldPos.z + 1, ( entity->aaBox.center()- aaBox.center()).direction() * fireSpeed ); 
        }

        entity = m_map->getClosestLOSEntity( Vector3( oldPos.x, oldPos.y, oldPos.z - .6f), 100, Creature::type); 

        if( notNull( entity ) ){
            entity->newHP = entity->oldHP - 10; 
            m_map->addBullet( oldPos.x, oldPos.y, oldPos.z - 1, ( entity->aaBox.center()- aaBox.center()).direction() * fireSpeed ); 
        }
        */
    }

    

    return isFalling; 
}

int BasicTower::getType(){
    return Cube::type; 
}

int BasicTower::getOccupancy(){
    return Cube::occupancy; 
}

void BasicTower::onSpawn(){
    Cube::onSpawn(); 
}

void BasicTower::onDeath(){
    Cube::onDeath(); 
}

