#include "Map.h" 

bool Creeper::move(){
    turnsLeft = currentSpeed;
    if (! m_map->hasBox[oldPos.x][oldPos.y-1][oldPos.z]){
        turnsLeft = 5;
        newPos.y -= 1;
        return true;
    }

    int a = rand() % 100;
    if ((a < 20) && (isValidMove(oldPos,0,1,0))){
        newPos.y += 1;
        turnsLeft = 3;
        dir = getRandomDirection();
        return true;
    }

    if (isValidMove(oldPos+dir)){
        newPos = newPos + dir;
        a = rand() % 100;
    } else{
        if (hasBox(oldPos+dir)){
            attack(oldPos,1,0,0,100);
            attack(oldPos,-1,0,0,100);
            attack(oldPos,0,1,0,100);
            attack(oldPos,0,-1,0,100);
            attack(oldPos,0,0,1,100);
            attack(oldPos,0,0,-1,100);
            suicide = true;
            newHP = -100000;
            if (atDiscoBall(oldPos,2)){
                attackDiscoBall(50);
            }
            turnsLeft = 5;
        } else{
            if (isValidMove(oldPos,0,1,0)){
                newPos.y += 1;
                turnsLeft = 3;
            }
            dir = getRandomDirection();
            return true;
        }
    }
    return true;
}

IntVec3 Creeper::getRandomDirection(){
    currentSpeed = rand() % 7 + 2;
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

int Creeper::getType(){
    return Creature::type; 
}

int Creeper::getOccupancy(){
    return Creeper::occupancy;
}

int Creeper::getBounty(){
    return Creeper::bounty;
}

void Creeper::onSpawn(){
    //m_map->gridOccupancy[oldPos.x][oldPos.y][oldPos.z] += occupancy;
    m_map->gridOccupancy[newPos.x][newPos.y][newPos.z] += getOccupancy();
}

void Creeper::onDeath(){
    m_map->gridOccupancy[oldPos.x][oldPos.y][oldPos.z] -= occupancy;
    if (! (oldPos == newPos)){
        m_map->gridOccupancy[newPos.x][newPos.y][newPos.z] -= occupancy;
    }
}
