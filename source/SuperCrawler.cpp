#include "Map.h" 

bool SuperCrawler::move(){
    if (atDiscoBall(oldPos,1)){
        attackDiscoBall(1);
        turnsLeft = 5;
        return true;
    }
    turnsLeft = 20;
    IntVec3 dir(1,0,0);
    int tmp;
    int minDist = search(oldPos+IntVec3(1,0,0),9);
    tmp = search(oldPos+IntVec3(-1,0,0),9);
    if (minDist > tmp){
        minDist = tmp;
        dir = IntVec3(-1,0,0);
    }
    tmp = search(oldPos+IntVec3(0,0,1),9);
    if (minDist > tmp){
        minDist = tmp;
        dir = IntVec3(0,0,1);
    }
    tmp = search(oldPos+IntVec3(0,0,-1),9);
    if (minDist > tmp){
        minDist = tmp;
        dir = IntVec3(0,0,-1);
    }

    IntVec3 rdir(0,0,0);

    if (tmp>0){
        int distX = m_map->discoBallLoc.x - oldPos.x;
        int distZ = m_map->discoBallLoc.z - oldPos.z;
        if (abs(distX) > abs(distZ)){
            if (distX > 0){
                rdir.x += 1;
            } else{
                rdir.x -= 1;
            }
        } else{
            if (distZ > 0){
                rdir.z += 1;
            } else{
                rdir.z -= 1;
            }
        }
        if (isValidMove(oldPos+rdir)){
            dir = rdir;
        } else{
            attack(oldPos+rdir,10);
            turnsLeft = 30;
            return true;
        }
    }
    newPos = newPos + dir;
}

int SuperCrawler::search(IntVec3 currentPos, int costLeft){
    if (currentPos == m_map->discoBallLoc){
        return (-costLeft*100);
    }
    if (!isValidMove(currentPos)){
        return 9999999;
    }
    if (costLeft == 0){
        IntVec3 dist = m_map->discoBallLoc - currentPos;
        return (abs(dist.x)+abs(dist.y)+abs(dist.z));
    } else{
        if (! hasBox(currentPos-IntVec3(0,1,0))){
            return search(currentPos-IntVec3(0,1,0),costLeft-1);
        }
        int minDist = 9999999;
        minDist = min(minDist,search(currentPos+IntVec3(1,0,0),costLeft-1));
        minDist = min(minDist,search(currentPos+IntVec3(-1,0,0),costLeft-1));
        minDist = min(minDist,search(currentPos+IntVec3(0,0,1),costLeft-1));
        minDist = min(minDist,search(currentPos+IntVec3(0,0,-1),costLeft-1));
        return minDist;
    }
}

int SuperCrawler::getType(){
    return Creature::type; 
}

int SuperCrawler::getOccupancy(){
    return SuperCrawler::occupancy;
}

int SuperCrawler::getBounty(){
    return SuperCrawler::bounty;
}

void SuperCrawler::onSpawn(){
    //m_map->gridOccupancy[oldPos.x][oldPos.y][oldPos.z] += occupancy;
    m_map->gridOccupancy[newPos.x][newPos.y][newPos.z] += getOccupancy();
}

void SuperCrawler::onDeath(){
    m_map->gridOccupancy[oldPos.x][oldPos.y][oldPos.z] -= occupancy;
    if (! (oldPos == newPos)){
        m_map->gridOccupancy[newPos.x][newPos.y][newPos.z] -= occupancy;
    }
}

