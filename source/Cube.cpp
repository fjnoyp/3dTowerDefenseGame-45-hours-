#include "Map.h"

//AABox Cube::aaBox = AABox( Vector3(-.5f,-.5f,-.5f), Vector3(.5f,.5f,.5f) ); 

bool Cube::move(){
    turnsLeft = 5;
    oldPos = newPos;
    oldHP = newHP;
    int x = oldPos.x;
    int y = oldPos.y;
    int z = oldPos.z;

    //Fall down 
    bool isFalling = checkIfFall(x,y,z); 
    if ( isFalling ){
        m_map->hasBox[x][y][z] = false;
        m_map->hasBox[x][y-1][z] = true;
        newPos.y -= 1;
    }
    return isFalling; 


    /*
    if (x > 0){
        m_map->defendDamage[x-1][y][z]+=1;
    }
    if (x < 15){
        m_map->defendDamage[x+1][y][z]+=1;
    }
    if (z > 0){
        m_map->defendDamage[x][y][z-1]+=1;
    }
    if (z < 15){
        m_map->defendDamage[x][y][z+1]+=1;
    }

    bool result = true;
    if (y == 1){
        result = false;
    } 
    //Else check that this cube has proper supports (KAI-FUTURE OPT IS TO ONLY DO THIS ON REMOVE CUBE EVENTS) 
    else{
        if (m_map->hasBox[x][y-1][z]){
            result = false;
        } else{
            int support = 0;
            if (x > 1){
                if (m_map->hasBox[x-1][y][z] && m_map->hasBox[x-1][y-1][z]) ++support;
            }
            if (x < 15){
                if (m_map->hasBox[x+1][y][z] && m_map->hasBox[x+1][y-1][z]) ++support;
            }
            if (z > 1){
                if (m_map->hasBox[x][y][z-1] && m_map->hasBox[x][y-1][z-1]) ++support;
            }
            if (z < 15){
                if (m_map->hasBox[x][y][z+1] && m_map->hasBox[x][y-1][z+1]) ++support;
            }
            if (support > 0) result = false;
        }
    }
    if (result){
        m_map->hasBox[x][y][z] = false;
        m_map->hasBox[x][y-1][z] = true;
        newPos.y -= 1;
    }


    //result = true;
    //newHP -= 20;

    return result;
    */

}

bool Cube::checkIfFall(int x, int y, int z){
    bool result = true;
    if (y == 1){
        result = false;
    } 
    //Else check that this cube has proper supports (KAI-FUTURE OPT IS TO ONLY DO THIS ON REMOVE CUBE EVENTS) 
    else{
        if (m_map->hasBox[x][y-1][z]){
            result = false;
        } else{
            result = checkSupport(x,y,z); 
        }
    }
    return result; 
}

bool Cube::checkSupport(int x, int y, int z){
    int support = 0;
    if (x > 1){
        if (m_map->hasBox[x-1][y][z] && m_map->hasBox[x-1][y-1][z]) ++support;
    }
    if (x < 15){
        if (m_map->hasBox[x+1][y][z] && m_map->hasBox[x+1][y-1][z]) ++support;
    }
    if (z > 1){
        if (m_map->hasBox[x][y][z-1] && m_map->hasBox[x][y-1][z-1]) ++support;
    }
    if (z < 15){
        if (m_map->hasBox[x][y][z+1] && m_map->hasBox[x][y-1][z+1]) ++support;
    }
    return !(support > 0); 

}

int Cube::getType(){
    return Cube::type; 
}

int Cube::getOccupancy(){
    return Cube::occupancy;
}

void Cube::onSpawn(){
    m_map->gridOccupancy[newPos.x][newPos.y][newPos.z] += getOccupancy();
    m_map->hasBox[oldPos.x][oldPos.y][oldPos.z] = true;
}

void Cube::onDeath(){
    //debugPrintf("!!!!");
    m_map->gridOccupancy[oldPos.x][oldPos.y][oldPos.z] -= getOccupancy();
    m_map->hasBox[newPos.x][newPos.y][newPos.z] = false;
    if (! (oldPos == newPos)){
        m_map->gridOccupancy[newPos.x][newPos.y][newPos.z] -= getOccupancy();
    }
}
