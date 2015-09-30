#include "Map.h" 

bool Crawler::move(){
    turnsLeft = 20;

    if (atDiscoBall(oldPos,1)){
        attackDiscoBall(1);
        turnsLeft = 5;
        return true;
    }

    if (! m_map->hasBox[oldPos.x][oldPos.y-1][oldPos.z]){
        turnsLeft = 5;
        newPos.y -= 1;
        return true;
    }
    IntVec3 dir = getRandomDirection();
    if (isValidMove(oldPos+dir)){
        newPos = newPos + dir;
    } else{
        if (isValidMove(oldPos+dir,0,1,0) && hasBox(oldPos+dir)){
            newPos = newPos + dir;
            newPos.y += 1;
        } else{
            attack(oldPos+dir,10);
            turnsLeft = 5;
        }
    }
    return true;
}

IntVec3 Crawler::getRandomDirection(){
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

int Crawler::getType(){
    return Creature::type; 
}

int Crawler::getOccupancy(){
    return Crawler::occupancy;
}

int Crawler::getBounty(){
    return Crawler::bounty;
}

void Crawler::onSpawn(){
    //m_map->gridOccupancy[oldPos.x][oldPos.y][oldPos.z] += occupancy;
}

void Crawler::onDeath(){
    m_map->gridOccupancy[oldPos.x][oldPos.y][oldPos.z] -= occupancy;
    if (! (oldPos == newPos)){
        m_map->gridOccupancy[newPos.x][newPos.y][newPos.z] -= occupancy;
    }
}

