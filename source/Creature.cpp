#include "Map.h" 

//AABox Cube::aaBox = AABox( Vector3(-.25,-.25,-.25), Vector3(.25f,.25f,.25f) ); 

bool Creature::move(){
    turnsLeft = 50;

    int desiredX = 15; 
    int desiredZ = 15; 

    //oldPos and newPos are int vector3 

    int curX = oldPos.x;
    int curY = oldPos.y; 
    int curZ = oldPos.z;

    int moveX = curX - desiredX; 
    int moveZ = curZ - desiredZ; 

    moveX = moveX/moveX; 
    moveZ = moveZ/moveZ; 

    int canMoveX = 0; 
    int canMoveZ = 0; 

    //if wants to move on x axis, check if box in way 
    if( !m_map->hasBox[curX + moveX][curY][curZ] ){
        canMoveX = moveX; 
    }
    //same for z axis 
    if( !m_map->hasBox[curX][curY][curZ + moveZ] ){
        canMoveZ = moveZ; 
    }

    //check if can move diagonally 
    if( canMoveX == 1 && canMoveZ == 1){
        if( m_map->hasBox[curX + moveX][curY][curZ + moveZ] ){
            canMoveX = 0; 
        }
    }

    //if creature can move 
    if( canMoveX != 0 || canMoveZ != 0){
        newPos.x += canMoveX; 
        newPos.z += canMoveZ; 

        if( !m_map->hasBox[curX+canMoveX][curY-1][curZ+canMoveZ] && curY != 1){
            newPos.y -= 1; 
        }
    }

    //if creature can't move 
    else{
        //KAI PLEASE ADD SLOW MOVEMENT FOR UPDWARDS MOVE 
        turnsLeft = 100;
        newPos.y += 1; 
        //newHP = 0; 
    }
    //if (curX > 0){
    //    m_map->attackDamage[curX-1][curY][curZ]=100;
    //}
    //if (curX < 15){
    //    m_map->attackDamage[curX+1][curY][curZ]=100;
    //}
    //if (curZ > 0){
    //    m_map->attackDamage[curX][curY][curZ-1]=100;
    //}
    //if (curZ < 15){
    //    m_map->attackDamage[curX][curY][curZ+1]=100;
    //}

    attack(oldPos,1,0,0,100);
    attack(oldPos,-1,0,0,100);
    attack(oldPos,0,0,1,100);
    attack(oldPos,0,0,-1,100);
    return true; 
}

int Creature::getType(){
    return Creature::type; 
}

int Creature::getOccupancy(){
    return Creature::occupancy;
}

int Creature::getBounty(){
    return Creature::bounty;
}

void Creature::onSpawn(){
    m_map->gridOccupancy[oldPos.x][oldPos.y][oldPos.z] += occupancy;
}

void Creature::onDeath(){
    m_map->gridOccupancy[oldPos.x][oldPos.y][oldPos.z] -= occupancy;
    if (! (oldPos == newPos)){
        m_map->gridOccupancy[newPos.x][newPos.y][newPos.z] -= occupancy;
    }
}
