/** \file Map.cpp */
#include "Map.h"

//NOTE : moved IntVec3 to Map.h as Cube.cpp, Creature.cpp all need this class 
IntVec3::IntVec3(){
    x = 0;
    y = 0;
    z = 0;
}

bool IntVec3::operator == (const IntVec3& other) const{
    return ((x == other.x) && (y == other.y) && (z == other.z));
}

bool IntVec3::operator != (const IntVec3& other) const{
    return ((x != other.x) || (y != other.y) || (z != other.z));
}

IntVec3 IntVec3::operator - (const IntVec3& other) const{
    return IntVec3(x-other.x,y-other.y,z-other.z);
}

IntVec3 IntVec3::operator + (const IntVec3& other) const{
    return IntVec3(x+other.x,y+other.y,z+other.z);
}

IntVec3::IntVec3(int x, int y, int z){
    this->x = x;
    this->y = y;
    this->z = z;
} 

Vector3 IntVec3::toVector(){
    return Vector3(x,y,z);
}

//OTHER CLASS METHODS ===========================================================
Change::Change(Vector3 op, Vector3 d, int oh, int nh, int t, shared_ptr<VisibleEntity> e){
    oldPos = op;
    displacement = d;
    oldHP = oh;
    newHP = nh;
    type = t;
    ve = e;
}

//MAP CLASS METHODS ================================================================
Map::Map( shared_ptr<DemoScene> scene){

    m_scene = scene; 
    //Create AABox Manager 

    //shared_ptr< std::vector< shared_ptr<MapElement> > > vP = 


    //aaBoxManager( std::make_shared< std::vector< shared_ptr<MapElement> > >( elements ) ); 
    std::vector< shared_ptr<MapElement> > * pointer = &elements; 

    aaBoxManager = AABoxManager::create(pointer); 

    if(Map::debugMap) printf("aaa\n");
    for (int i = 0; i < 15; ++i){
        for (int j = 0; j < 15; ++j){
            for (int k = 0; k < 15; ++k){
                gridOccupancy[i][j][k] = 0;
                hasBox[i][j][k] = false;
                attackDamage[i][j][k] = 0;
                defendDamage[i][j][k] = 0;
            }
        }
    }

    //Initialize bottom 
    for (int i = 0; i < 15; ++i){
        for (int k = 0; k < 15; ++k){
            gridOccupancy[i][0][k] = 100;
            hasBox[i][0][k] = true;
        }
    }
    //DiscoBall
    gridOccupancy[discoBallLoc.x][discoBallLoc.y][discoBallLoc.z] = 10000000;
    hasBox[discoBallLoc.x][discoBallLoc.y][discoBallLoc.z] = true;
    discoBallHP = 3000;
    gold = 10000000;
    
    //gun = G3D::Sound::create(System::findDataFile("gun.ogg",false));
}

std::vector<shared_ptr<Change> > Map::getChange(){

    std::vector<shared_ptr<Change> > changes;
    //shared_ptr<Change> change(new Change( Vector3(1,0,1), Vector3(1,1,1), 100, 100, Creature::type, creature) ); 

    for(int i = 0; i<(int)elements.size(); i++){
        elements[i]->updateHP();       
    }

    for(int i = 0; i<(int)elements.size(); i++){
        elements[i]->run();       
    }

    for(int i = 0; i<(int)elements.size(); i++){
        elements[i]->takeDamage();       
    }
    
    for(int i = 0; i<(int)elements.size(); i++){
        if (elements[i]->change){
            shared_ptr<Change> change(new Change(elements[i]->oldPos.toVector(),elements[i]->displacement,(elements[i]->oldHP*100/elements[i]->maxHP), (elements[i]->newHP*100/elements[i]->maxHP), elements[i]->getType(), elements[i]->entity));

            changes.push_back(change);
        }
    }
    //Remove elements which have 0 health 
    for(int i = (int)elements.size() - 1; i>=0; --i){
        if(Map::debugMap) printf("%d \n", elements[i]->newHP);
        if (elements[i]->newHP <= 0){
            if (elements[i]->getType() == Creature::type){
                if (! elements[i]->suicide){
                    gold += elements[i]->getBounty();
                }
            }
            elements[i]->onDeath();
            //Separate CUBE Check 
            //if(elements[i]->getType() == Cube::type){
            //    hasBox[elements[i]->oldPos.x][elements[i]->oldPos.y][elements[i]->oldPos.z] = false;
            //    gridOccupancy[elements[i]->oldPos.x][elements[i]->oldPos.y][elements[i]->oldPos.z] -= 100;
            //}

            //Need to actually erase element from storage in all cases 
            elements.erase(elements.begin() + i); 
        }
    }

    for (int i = 0; i < 15; ++i){
        for (int j = 0; j < 15; ++j){
            for (int k = 0; k < 15; ++k){
                attackDamage[i][j][k] = 0;
                defendDamage[i][j][k] = 0;
            }
        }
    }

    gold += 0;
    //debugPrintf("%d\n", gold);
    return changes;
}

bool Map::add(shared_ptr<VisibleEntity> e, int type, int x, int y, int z){
    return add(e,type,0,x,y,z,1);
}

bool Map::canAdd( int type, int creatureType ){
    if( type == Cube::type ){
        if( creatureType == Cube::creatureType ){
            return (gold >= Cube::cost); 
        }
        else if( creatureType == BasicTower::creatureType ){
            return (gold >= BasicTower::cost);
        }
    }
}

//TO DO ADD IN CALL TO MAKEBOX FOR THESE THINGS 
bool Map::add(shared_ptr<VisibleEntity> e, int type, int creatureType, int x, int y, int z, float HPFactor){
    shared_ptr<MapElement> tmp;
    //Cube case
    if( type == Cube::type && creatureType == Cube::creatureType){
        gold -= Cube::cost; 
        tmp = shared_ptr<MapElement>(dynamic_cast<MapElement*>(new Cube(e,x,y,z,this)));
    } 
    else if( type == Cube::type && creatureType == BasicTower::creatureType ){
        gold -= BasicTower::cost; 
        tmp = shared_ptr<MapElement>(dynamic_cast<MapElement*>(new BasicTower(e,x,y,z,this)));
        //gridOccupancy[x][y][z] = 100; 
    }
    else{ 
        if (creatureType == Crawler::creatureType){
            tmp = shared_ptr<MapElement>(dynamic_cast<MapElement*>(new Crawler(e,x,y,z,this)));
        } else if (creatureType == Creeper::creatureType){
            tmp = shared_ptr<MapElement>(dynamic_cast<MapElement*>(new Creeper(e,x,y,z,this)));
        } else if (creatureType == SmartCrawler::creatureType){
            tmp = shared_ptr<MapElement>(dynamic_cast<MapElement*>(new SmartCrawler(e,x,y,z,this)));
        } else if (creatureType == SuperCrawler::creatureType){
            tmp = shared_ptr<MapElement>(dynamic_cast<MapElement*>(new SuperCrawler(e,x,y,z,this)));
        } else{
            tmp = shared_ptr<MapElement>(dynamic_cast<MapElement*>(new Creature(e,x,y,z,this)));
        }
        tmp->newHP *= HPFactor;
        tmp->maxHP *= HPFactor;
    }

    elements.push_back(tmp);
    tmp->onSpawn();

    //printf("%i\n",(int)elements.size());

    //Other case (to be more specific) 
    //else if( type == Creature::type ){
    //    shared_ptr<MapElement> tmp = shared_ptr<MapElement>(dynamic_cast<MapElement*>(new Creature(e,x,y,z,this)));
    //    elements.push_back(tmp);
    //}


    return true;

}

IntVec3 Map::getSelectedBuildPos(Ray ray){
    return aaBoxManager->findNewCubePos(ray); 
}

shared_ptr<MapElement> Map::getSelectedEntity(Ray ray, int type){
    shared_ptr<MapElement> rayHitElement = aaBoxManager->getRayHitElement(ray,type);
    
    if( isNull(rayHitElement) ){
        return shared_ptr<MapElement>(nullptr); 
    }
    else{
        return rayHitElement;//->entity; 
    }
}


bool Map::canSpawnCreature(int x, int y, int z, int occupancy){
    return (occupancy + gridOccupancy[x][y][z]) <= 100;
}

bool Map::canAddBox(int x, int y, int z){
    if(x==7 && z ==7) return false; 
    if ((x < 1) || (x > 13) || (y < 1) || (y > 13) || (z < 1) || (z > 13)) return false;
    if (gridOccupancy[x][y][z] == 0){
        if (y == 1){
            return true;
        } else{
            if (hasBox[x][y-1][z]){
                return true;
            } else{
                int support = 0;
                if (x > 0){
                    if (hasBox[x-1][y][z] && hasBox[x-1][y-1][z]) ++support;
                }
                if (x < 14){
                    if (hasBox[x+1][y][z] && hasBox[x+1][y-1][z]) ++support;
                }
                if (z > 0){
                    if (hasBox[x][y][z-1] && hasBox[x][y-1][z-1]) ++support;
                }
                if (z < 14){
                    if (hasBox[x][y][z+1] && hasBox[x][y-1][z+1]) ++support;
                }
                if (support > 0) return true;
            }
        }
    }
    return false;
}

bool Map::remove(shared_ptr<VisibleEntity> e){
    for(int i = 0; i<(int)elements.size(); i++){
        if( e == elements[i]->entity){
            //hasBox[elements[i]->oldPos.x][elements[i]->oldPos.y][elements[i]->oldPos.z] = false;
            //gridOccupancy[elements[i]->oldPos.x][elements[i]->oldPos.y][elements[i]->oldPos.z] -= 100;
            elements[i]->onDeath();
            elements.erase(elements.begin() + i); 
            break;
        }
    }
    return true;
}

/*
shared_ptr<MapElement> Map::getClosestLOSEntity( Vector3 pos, float maxDist, int type ){
    
    float minSqrDist = maxDist * maxDist; 
    shared_ptr<MapElement> closestEntity = shared_ptr<MapElement>(nullptr); 

    float curSqrDist; 

    for(int i = 0; i<(int)elements.size(); i++){

        if( type == elements[i]->getType() ){
            
            curSqrDist = (elements[i]->entity->frame().translation - pos).squaredMagnitude(); 
            //If it is closer than closest one so far
            if( curSqrDist < minSqrDist){

                Ray ray(pos, elements[i]->entity->frame().translation - pos); 

                //Check if there is LOS between them 
                if( elements[i] == aaBoxManager->getRayHitElement( ray, -1 ) ){
                    minSqrDist = curSqrDist; 
                    closestEntity = elements[i]; 
                }
            }
        }
    }

    return closestEntity; 
} 
*/

shared_ptr<MapElement> Map::getClosestLOSEntity( Vector3 pos, float maxDist, int type ){

    // ignorePos to prevent hitting the calling object 
    
    float minSqrDist = maxDist * maxDist; 
    shared_ptr<MapElement> closestEntity = shared_ptr<MapElement>(nullptr); 

    float curSqrDist; 

    for(int i = 0; i<(int)elements.size(); i++){

        if(  type == elements[i]->getType() ){
            
            curSqrDist = (elements[i]->entity->frame().translation - pos).squaredMagnitude(); 
            //If it is closer than closest one so far
            if( curSqrDist < minSqrDist){

                //Ray ray(pos, elements[i]->entity->frame().translation - pos); 
                Ray ray(pos, (elements[i]->getAABox().center() - pos).direction() ); 

                //Check if there is LOS between them 
                if( elements[i] == aaBoxManager->getRayHitElement( ray, -1 )){// || isNull(hitElement) ){
                    minSqrDist = curSqrDist; 
                    closestEntity = elements[i]; 
                }
            }
        }
    }

    return closestEntity; 
} 

bool Map::addBullet( int x, int y, int z, Vector3 velocity){
    m_bulletCount += 1;
    /*
    if (m_bulletCount == 20){
        gun->play(0.3);
        m_bulletCount = 0;
    }
    */

    shared_ptr<VisibleEntity> ve = m_scene->spawnVisibleEntity("bullet","bulletModel",CFrame::fromXYZYPRDegrees(x,y,z,0,0,0)); 
    //m_scene->spawnVisibleEntity("bullet","bulletModel",CFrame::fromXYZYPRDegrees(5,5,5,0,0,0)); 

    elements.push_back( shared_ptr<MapElement>(dynamic_cast<MapElement*>(new Bullet( ve, x,y,z, velocity, this) ) )); 

    return true; 
}
