#include "Map.h" 


bool MapElement::operator < (const MapElement& other) const{
    return ((newPos.x+newPos.z*10+newPos.y*100) < (other.newPos.x+other.newPos.z*10+other.newPos.y*100));
}

MapElement::MapElement(shared_ptr<VisibleEntity> e, int x, int y, int z, Map* m){
    entity = e;
    oldPos = IntVec3(x,y,z);
    newPos = IntVec3(x,y,z);
    oldHP = 100;
    newHP = 100;
    maxHP = 100;
    m_map = m;
    turnsLeft = 1;
    oldOffset = Vector3(0,0,0);
    newOffset = oldOffset;
    //Compute the entry point
    if (!inMap()){
        x = oldPos.x;
        z = oldPos.z;
        //if (x < 0){
        //    newPos.z *= (float)7/(7-x);
        //}
        //if (x > 14){
        //    newPos.z *= (float)7/(x-7);
        //}
        //if (z < 0){
        //    newPos.x *= (float)7/(7-z);
        //}
        //if (z > 14){
        //    newPos.x *= (float)7/(z-7);
        //}
        if (newPos.x < 0){
            newPos.x = 0;
        }
        if (newPos.x > 14){
            newPos.x = 14;
        }
        if (newPos.z < 0){
            newPos.z = 0;
        }
        if (newPos.z > 14){
            newPos.z = 14;
        }
        turnsLeft = 100;
        displacement = ((newPos-oldPos).toVector())/float(turnsLeft);
    }
}

void MapElement::updateHP(){
    oldHP = newHP;
}

bool MapElement::boundaryCheck(int x, int y, int z){
    return ((x>=0) && (x<15) && (y>=1) && (y<15) && (z>=0)&&(z<15));
}

bool MapElement::inMap(){
    return boundaryCheck(oldPos.x,oldPos.y,oldPos.z);
}

bool MapElement::isValidMove(int x, int y, int z){
    if (boundaryCheck(x,y,z)){
        return ((getOccupancy() + m_map->gridOccupancy[x][y][z]) <= 100);
    } else{
        return false;
    }
}

bool MapElement::isValidMove(IntVec3 old, int x, int y, int z){
    return isValidMove(old.x+x,old.y+y,old.z+z);
}

bool MapElement::isValidMove(IntVec3 v){
    return isValidMove(v.x,v.y,v.z);
}

bool MapElement::attack(int x, int y, int z, int damage){
    if (boundaryCheck(x,y,z)){
        if (getType() == Cube::type){
            m_map->defendDamage[x][y][z] += damage;
        } else{
            m_map->attackDamage[x][y][z] += damage;
        }
        return true;
    } else{
        return false;
    }
}

bool MapElement::hasBox(int x, int y, int z){
    if (boundaryCheck(x,y,z) || boundaryCheck(x,y+1,z)){
        return m_map->hasBox[x][y][z];
    } else{
        return false;
    }
}

bool MapElement::hasBox(IntVec3 v){
    return hasBox(v.x,v.y,v.z);
}

bool MapElement::attack(IntVec3 old, int x, int y, int z, int damage){
    return attack(old.x+x,old.y+y, old.z+z,damage);
}

bool MapElement::attack(IntVec3 v, int damage){
    return attack(v.x,v.y,v.z,damage);
}

bool MapElement::attackDiscoBall(int damage){
    m_map->discoBallHP -= damage;
    return true;
}

bool MapElement::atDiscoBall(IntVec3 v, int range){
    IntVec3 dist = v - m_map->discoBallLoc;
    return ((abs(dist.x)+abs(dist.y)+abs(dist.z)) <= range);
}

bool MapElement::atDiscoBall(int x, int y, int z, int range){
    return atDiscoBall(IntVec3(x,y,z),range);
}

//Dont's override 
//Calculate the displacement for the object based on turnsL
void MapElement::run(){
    if (! inMap()){
	newHP = maxHP;
    }
    change = false;
    turnsLeft -= 1;
    if (turnsLeft <= 0){
        if (! (oldPos == newPos) && inMap()){
            m_map->gridOccupancy[oldPos.x][oldPos.y][oldPos.z] -= getOccupancy();
        }
        oldPos = newPos; 
        move();
        if (! (oldPos == newPos)){
            m_map->gridOccupancy[newPos.x][newPos.y][newPos.z] += getOccupancy();
        }
        
        displacement = ((newPos-oldPos).toVector())/float(turnsLeft);

        if (getType() == Creature::type){

            oldOffset = newOffset;
            newOffset = Vector3((randFloat() - 0.5)/3.0, (randFloat() - 0.5)/10.0, (randFloat() - 0.5)/3.0);
            displacement += (newOffset-oldOffset)/float(turnsLeft);
        }
    }
    
    change = !(oldPos == newPos);

    //move AABox here 
    aaBox = aaBox + displacement; 
}

void MapElement::takeDamage(){
    if (inMap()){
        if (getType() == Cube::type){
            newHP -= m_map->attackDamage[oldPos.x][oldPos.y][oldPos.z] / ((float)m_map->gridOccupancy[oldPos.x][oldPos.y][oldPos.z]/getOccupancy());
            if (! (oldPos == newPos)){
                newHP -= m_map->attackDamage[newPos.x][newPos.y][newPos.z] / ((float)m_map->gridOccupancy[newPos.x][newPos.y][newPos.z]/getOccupancy());
            }
        } else{
            newHP -= m_map->defendDamage[oldPos.x][oldPos.y][oldPos.z] / ((float)m_map->gridOccupancy[oldPos.x][oldPos.y][oldPos.z]/getOccupancy());
            if (! (oldPos == newPos)){
                newHP -= m_map->defendDamage[newPos.x][newPos.y][newPos.z] / ((float)m_map->gridOccupancy[newPos.x][newPos.y][newPos.z]/getOccupancy());
            }
            if ( (m_map->hasBox[oldPos.x][oldPos.y][oldPos.z]) || (m_map->hasBox[newPos.x][newPos.y][newPos.z])){
                newHP = -10000000;
            }
        }
    }

    change = (change || (!(oldHP == newHP)));
}

bool MapElement::move(){
    if(Map::debugMap) printf("screwd up");
    return false;
}

AABox MapElement::getAABox(){
    return aaBox; 
}

int MapElement::getType(){
    //debugPrintf("MapElement getType should not be called, should be overriden in subclass");
    return -1; 
}

int MapElement::getOccupancy(){
    //debugPrintf("MapElement getOccupancy should not be called, should be overriden in subclass");
    return -1;
}

int MapElement::getBounty(){
    //debugPrintf("MapElement getBounty should not be called, should be overriden in subclass");
    return -1;
}

void MapElement::onSpawn(){
    //debugPrintf("MapElement onSpawn should not be called, should be overriden in subclass");
}

void MapElement::onDeath(){
    //debugPrintf("MapElement onDeath should not be called, should be overriden in subclass");
}

float MapElement::randFloat(){
    return (static_cast <float> (rand()) / static_cast <float> (RAND_MAX));
}
