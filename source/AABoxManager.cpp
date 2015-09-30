#include "Map.h"

//TO DO: optimization would be to return references to vectors rather than return a vector by value 
AABoxManager::AABoxManager( std::vector<shared_ptr<MapElement> > *elements){
    allElements = elements; 

    for(int x = 0; x < Map::dimX; x++){
        for(int z = 0; z < Map::dimZ; z++){
            groundCubeAA.push_back( makeBox( x, 0, z )); 
        }
    }
}


shared_ptr<AABoxManager> AABoxManager::create( std::vector<shared_ptr<MapElement> > *elements){
    return shared_ptr<AABoxManager>(new AABoxManager( elements )); 
}

std::vector<shared_ptr<MapElement> > AABoxManager::getRayHitElements(Ray ray, int type){
    std::vector< shared_ptr<MapElement> > hitElements; 

    //debugPrintf("SIZE: %i", (*allElements).size() ); 

    //Find boxes that intersect our ray 
    for(int i = 0; i< (*allElements).size(); ++i){

        //debugPrintf("THIS: %i \n", (*allElements).at(i)->getType()); 

        if( type==-1 || (*allElements)[i]->getType() == type ){

            /*
            AABox aaBox = (*allElements)[i]->getAABox(); 
            Vector3 pos = (*allElements)[i]->entity->frame().translation;

            //memory leak? no actually modifying the value 
            (*allElements)[i]->aaBox = AABox( Vector3( pos.x-.5f, pos.y-.5f, pos.z-.5f), 
                                              Vector3( pos.x+.5f, pos.y+.5f, pos.z+.5f) ); 
            */


            if( Intersect::rayAABox( ray, (*allElements)[i]->getAABox() ) ){
                //debugPrintf("considered %i \n", (*allElements)[i]->getType()); 
                hitElements.push_back( (*allElements)[i] );
            }
        }
    }

    //Exception case for ground grid intersection
    /*
    if( type==Cube::type ){
        for(int i = 0; i< (int)groundCubeAA.size(); ++i){
            if( Intersect::rayAABox( ray, groundCubeAA[i] )){
                hitElements.push_back( groundCubeAA[i] ); 
            }
        }
    }
    */

    return hitElements; 
}

shared_ptr<MapElement> AABoxManager::getClosestElement(Ray ray, std::vector<shared_ptr<MapElement> > elements ){

    shared_ptr<MapElement> closestElement; 
    float minDist( 9999999 ); 

    float ourDist; 
    for(int i = 0; i<(int)elements.size(); ++i){
        ourDist = (elements[i]->aaBox.center() - ray.origin()).squaredMagnitude();

        if(ourDist < minDist){
            minDist = ourDist; 
            closestElement = elements[i]; 
        }
    }

    return closestElement; 
}

AABox AABoxManager::makeBox(int posX, int posY, int posZ){
    return AABox( Vector3( posX-.5f, posY-.5f, posZ-.5f ),
                  Vector3( posX+.5f, posY+.5f, posZ+.5f ) );
}

shared_ptr<MapElement> AABoxManager::getRayHitElement(Ray ray, int type){
    std::vector< shared_ptr<MapElement> > hitElements = getRayHitElements(ray,type); 

    if(hitElements.size() != 0){
        return getClosestElement(ray, hitElements); 
    }
    else{
        return shared_ptr<MapElement>(nullptr); 
    }
}


IntVec3 AABoxManager::findNewCubePos(const Ray& ray){
    std::vector< shared_ptr<MapElement> > hitElements; 

    hitElements = getRayHitElements(ray,Cube::type); 


    AABox closestBox;
    bool found = false; 

    if( hitElements.size() != 0){
        closestBox = getClosestElement( ray, hitElements )->aaBox;
        found = true; 
    }
    else{
        //Grid hit case 
        std::vector< AABox > hitBoxes = getRayHitBoxes( ray, groundCubeAA ); 

        //debugPrintf( "%i", hitBoxes.size()); 

        if( hitBoxes.size() != 0 ){
            closestBox = getClosestBox(ray,hitBoxes); 
            found = true; 
        }
    }

    if( found ){

        Vector3 pos = closestBox.center(); 

        int posX = floor(pos.x+.5); 
        int posY = floor(pos.y+.5);
        int posZ = floor(pos.z+.5); 

        //Find new box position, add boxes where our new box might possibly be 
        std::vector< AABox > possibleBoxes; 
        possibleBoxes.push_back( makeBox(posX,posY,posZ+1));
        possibleBoxes.push_back( makeBox(posX,posY,posZ-1));
        possibleBoxes.push_back( makeBox(posX,posY+1,posZ));
        possibleBoxes.push_back( makeBox(posX,posY-1,posZ));
        possibleBoxes.push_back( makeBox(posX+1,posY,posZ));
        possibleBoxes.push_back( makeBox(posX-1,posY,posZ));

        //Then find which of these new boxes our ray intersects
        possibleBoxes = getRayHitBoxes( ray, possibleBoxes ); 
            
        if( (int)possibleBoxes.size() != 0){

            AABox newBox = getClosestBox( ray, possibleBoxes); 
            pos = newBox.center(); 
            posX = floor(pos.x+.5); 
            posY = floor(pos.y+.5);
            posZ = floor(pos.z+.5); 
            
            if( posX >= 0 && posX < Map::dimX &&
                posY >= 0 && posY < Map::dimY &&
                posZ >= 0 && posZ < Map::dimZ ){
                //if(gridOccupancy[posX][posY][posZ] != 1){
                return IntVec3(posX, posY, posZ); 
            }
        }
    }
    //return IntVec3(-1,-1,-1); 
    return IntVec3(1,1,1); 
    
}

std::vector< AABox > AABoxManager::getRayHitBoxes(Ray ray, std::vector<AABox> possibleBoxes ){
    std::vector< AABox > hitBoxes; 

    //Find boxes that intersect our ray 
    for(int i = 0; i < (int)possibleBoxes.size(); ++i){

        if( Intersect::rayAABox( ray, possibleBoxes[i] ) ){
            hitBoxes.push_back( possibleBoxes[i] );
        }
    }

    return hitBoxes; 

}

AABox AABoxManager::getClosestBox(Ray ray, std::vector<AABox> hitBoxes ){

    AABox closestBox; 
    float minDist( 9999999 ); 

    float ourDist; 
    for(int i = 0; i<(int)hitBoxes.size(); ++i){
        ourDist = ( hitBoxes[i].center() - ray.origin()).squaredMagnitude();

        if(ourDist < minDist){
            minDist = ourDist; 
            closestBox = hitBoxes[i]; 
        }
    }

    return closestBox; 


    }
