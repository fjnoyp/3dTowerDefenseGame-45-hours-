#include "Map.h" 

bool SmartCrawler::move(){
    turnsLeft = 20;

    if (atDiscoBall(oldPos,1)){
        attackDiscoBall(1);
        turnsLeft = 5;
        return true;
    }

    if (! m_map->hasBox[oldPos.x][oldPos.y-1][oldPos.z]){
        if(!movingUp){
        turnsLeft = 5;
        newPos.y -= 1;
        return true;
        }
        movingUp = false; 
    }

    IntVec3 dir(0,0,0);

    int distX = m_map->discoBallLoc.x - oldPos.x;
    int distZ = m_map->discoBallLoc.z - oldPos.z;
    
    if (abs(distX) > abs(distZ)){
        if (distX > 0){
            dir.x += 1;
        } else{
            dir.x -= 1;
        }
    } else{
        if (distZ > 0){
            dir.z += 1;
        } else{
            dir.z -= 1;
        }
    }
    
    if (isValidMove(oldPos+dir)){
        newPos = newPos + dir;
    } else{
        if (isValidMove(oldPos+dir,0,1,0) && hasBox(oldPos+dir)){
            newPos = newPos + dir;
            newPos.y += 1;
            turnsLeft = 100; 
        } else{
            newPos.y += 1; 
            movingUp = true; 
            turnsLeft = 100; 
            //attack(oldPos+dir,10);
            //turnsLeft = 5;
        }
    }
    return true;
}

IntVec3 SmartCrawler::getRandomDirection(){
    int a = (rand() % 4);
    if (a == 0){
        return IntVec3(1,0,0);
    } else if (a == 1){
        return IntVec3(-1,0,0);
    } else if (a == 2){
        return IntVec3(0,0,1);
    } else{
        return IntVec3(0,0,-1);
    }
}

int SmartCrawler::getType(){
    return Creature::type; 
}

int SmartCrawler::getOccupancy(){
    return SmartCrawler::occupancy;
}

int SmartCrawler::getBounty(){
    return SmartCrawler::bounty;
}

void SmartCrawler::onSpawn(){
    //m_map->gridOccupancy[oldPos.x][oldPos.y][oldPos.z] += occupancy;
    m_map->gridOccupancy[newPos.x][newPos.y][newPos.z] += getOccupancy();
}

void SmartCrawler::onDeath(){
    m_map->gridOccupancy[oldPos.x][oldPos.y][oldPos.z] -= occupancy;
    if (! (oldPos == newPos)){
        m_map->gridOccupancy[newPos.x][newPos.y][newPos.z] -= occupancy;
    }
}

