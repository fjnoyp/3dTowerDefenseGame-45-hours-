/**
  \file Map.h
 */
#ifndef Map_h
#define Map_h

#include <G3D/G3DAll.h>

#include "DemoScene.h"

/** Map **/

class Map;

/** Vector3 of integers, equals and +/- are overloaded */
class IntVec3{
    public:
        bool operator == (const IntVec3& other) const;
        bool operator != (const IntVec3& other) const; 
        IntVec3 operator - (const IntVec3& other) const;
        IntVec3 operator + (const IntVec3& other) const;
        int x;
        int y;
        int z;
        /**Convert to a normal Vector3**/
        Vector3 toVector();
        IntVec3();
        IntVec3(int x, int y, int z);
};

/** \brief Class that describes a change in the visibile entity. 
    Sent from Map to VisualControl. \n
    Contains information about the change to an visible entity in the past frame \n
    VisualControl is responsible for decoding the information */
class Change{
    public:
        /** Old Position at the start of the move, does not reflect interpolation */
        Vector3 oldPos;
        /** Displacement vector, reflects the change every frame **/
        Vector3 displacement;
        /** Old HP before this frame */
        int oldHP;
        /** New HP at this frame */
        int newHP;
        /** Type of the MapElement*/
        int type;
        /** The VisibleEntity to be changed */
        shared_ptr<VisibleEntity> ve;
        /** Constructor 
            \param op oldPos
            \param d displacement
            \param oh oldHP
            \param nh newHP
            \param t type
            \param e ve
        */
        Change(Vector3 op, Vector3 d, int oh, int nh, int t, shared_ptr<VisibleEntity> e);
};

/** \brief Parent class for elements in the map. Only child classes should be used
    A MapElement is the basic unit in Map. It contains information about is location, HP and etc, as well as methods that provides basic attack and move methods. In addition to that, it also contains information about its graphical representation.

    Extended by Creature and Cube
*/
class MapElement{
    public:
        /** Overloaded less than for sorting */
        bool operator < (const MapElement& other) const;
        /** Destination of this after finishing the current move. See move() */
        IntVec3 newPos;
        /** Previous position of this before the current move. */
        IntVec3 oldPos;
        /** Offset used for random placement within a Map grid. Reflects the offset for oldPos */
        Vector3 oldOffset;
        /** Offset used for random placement within a Map grid. Reflects the offset for newPos */
        Vector3 newOffset;
        /** True if anything about it has changed in the past frame, vice versa */
        bool change;
        /** HP at this frame */
        int newHP;
        /** HP before this frame */
        int oldHP;
        /** Max HP of creature*/
        int maxHP;
        /** Number of frames left before the current move is finished. See move() */
        int turnsLeft;
        /** Displacement per frame. Calculated as the difference between newPos and oldPos(plus offset) divided by the number of turns need to finish a move */
        Vector3 displacement;
        /** Pointer to the map*/
        Map* m_map;
        /** Pointer to the VisibleEntity in the actual Scene */
        shared_ptr<VisibleEntity> entity;
        
        /** Used for raycasting*/
        AABox aaBox; 
    
        /** Constructor 
            \param e entity
            \param x,y,z used for initializating oldPos and newPos
            \param m m_map
        */
        MapElement(shared_ptr<VisibleEntity> e, int x, int y, int z, Map* m);
        /** Generates pseudo-random float between 0 and 1 */
        float randFloat();
        /** \brief Called every frame to update the MapElement
            A MapElement takes a certain number of turns(frames) to finish a move. Run is called to check if the current move has been finished. Everytime run() is called, turnsLeft is decremented by 1. If turnsLeft equals 0, move() is called to perform the next move
        */
        virtual void run();
        /** Called every frame to update the HP of the MapElement
        */
        void updateHP();
        /** Takes damage from the grid for both the oldPos and newPos. Takes attack damage if a Cube and defend damage if a Creature.

            See Map::attackDamage and Map::defendDamage for more info.
        */
        virtual void takeDamage();
        /** \brief Attack the position on a grid
            Does attack damage if a Creature and defend Damage if a Cube. The damage is distributed to corresponding MapElement in the grid with weight base on corresponding occupancy.
            \param x,y,z grid to attack
            \param damage Damage dealt

            See Map::attackDamage and Map::defendDamage for more info.
        */
        bool attack(int x, int y, int z, int damage);
        /** Helper for calling attack(int x,int y,int z,int damage) 
            \param v IntVec3 that contains x,y,z
            \param damage Damage dealt
        */
        bool attack(IntVec3 v, int damage);
        /** Helper for calling attack(int x,int y,int z,int damage) 
            \param old IntVec3 that contains the coordinate
            \param x,y,z offset to IntVec3 old
            \param damage Damage dealt
        */
        bool attack(IntVec3 old, int x, int y, int z, int damage);
        /** Attack the Disco Ball!!!! 
            \param damage Damage to Disco Ball
        */
        bool attackDiscoBall(int damage);
        /** Check if this position is in the Map grid in order to avoid bad access
            \param x,y,z point to be checked
        */
        bool boundaryCheck(int x, int y, int z);
        /** Check if there is a box at the specified position
            \param x,y,z position to be checked
        */
        bool hasBox(int x, int y, int z);
        /** Helper for calling hasBox(int x, int y, int z);
            \param v IntVec3 of the position
        */
        bool hasBox(IntVec3 v);
        /** \brief Check if can move to the position
            Can move if the occupancy of this MapElement sums the gridOccupancy of the destination grid is less than or equal to 100
            \param x,y,z position to be checked
        */
        bool isValidMove(int x, int y, int z);
        /** Helper for calling isValidMove(int x, int y, int z);
            \param v IntVec3 of the position
        */
        bool isValidMove(IntVec3 v);
        /** Helper for calling isValidMove(int x, int y, int z);
            \param old IntVec3 that contains the coordinate
            \param x,y,z offset to IntVec3 old
        */
        bool isValidMove(IntVec3 old, int x, int y, int z);
        /** Helper for calling atDiscoBall(IntVec3 v, int range)
            \param x,y,z position of MapElement
            \param range range of attack, in mahattan distance
        */
        bool atDiscoBall(int x, int y, int z, int range);
        /** Check if the MapElement is close to the Disco Ball
            \param v position of MapElement
            \param range range of attack, in mahattan distance

            See Map::discoBallLoc for more info
        */
        /** Check if the MapElement is inside the map. Used for visualizing the creatures approaching the playable map **/
        bool inMap();
        bool atDiscoBall(IntVec3 v, int range);
        /** \brief Take a move
            This is the method that actually determines the behavior of the creature. Called by run() everytime when turnsLeft = 0;
            
            Must update turnsLeft. Must be overridden in sub classesA
        */
        virtual bool move();


        /** Get the AABox corresponding to the creature. Has to be virtual because size vary */
        virtual AABox getAABox(); 
        /** \return the type of it */
        virtual int getType(); 
        /** \return the occupancy of it */
        virtual int getOccupancy();
        /** \return the bounty of it when killed, applicable to only Creature */
        virtual int getBounty();
        /** If true, no bounty is given upon death */
        int suicide = false;
        /** Update m_map when it spawns */
        virtual void onSpawn();
        /** Update m_map when it dies */
        virtual void onDeath();
};


class Bullet: public MapElement{
 public:
    static const int type = 100; 
    Vector3 velocity; 

 Bullet(shared_ptr<VisibleEntity> e, int x, int y, int z, Vector3 velocity, Map* m) : MapElement(e,x,y,z,m){
        aaBox = AABox( Vector3(x-.2f,y-.2f,z-.2f), Vector3(x+.2f,y+.2f,z+.2f) ); 
        this->velocity = velocity; 
        displacement = velocity; 

        oldHP = 100; 
        newHP = 100; 
        maxHP = 100;
    };
    virtual void takeDamage(); 
    virtual bool move(); 
    virtual void run(); 
    virtual void onDeath(); 

};



class Cube : public MapElement{
    public:
    static const int type = 1; 
    static const int creatureType = 0; 

 Cube(shared_ptr<VisibleEntity> e, int x, int y, int z, Map* m) : MapElement(e,x,y,z,m){
        aaBox = AABox( Vector3(x-.5f,y-.5f,z-.5f), Vector3(x+.5f,y+.5f,z+.5f) ); 
    };
        int color;
        virtual bool move();
        static const int occupancy = 100;
        static const int cost = 1000;
        //NOTE: getting the object's type is different from checking its type 
        virtual int getType(); 
        virtual int getOccupancy();
        virtual void onSpawn();
        virtual void onDeath();

    protected: 
        bool checkIfFall(int x, int y, int z);
        bool checkSupport(int x, int y, int z); 
};

class BasicTower : public Cube{
    
 public: 
    shared_ptr<Sound> shot;
    static const int creatureType = 1; 

BasicTower(shared_ptr<VisibleEntity> e, int x, int y, int z, Map* m) : Cube(e,x,y,z,m){
        shot = G3D::Sound::create(System::findDataFile("shot.wav",false));
};
        //int color;
        virtual bool move();
        static const int occupancy = 100;
        static const int cost = 5000; 
        //NOTE: getting the object's type is different from checking its type 
        virtual int getType(); 
        virtual int getOccupancy();
        virtual void onSpawn();
        virtual void onDeath();

}; 

class Creature : public MapElement{
    public:


 Creature(shared_ptr<VisibleEntity> e, int x, int y, int z, Map* m) : MapElement(e,x,y,z,m){
        aaBox = AABox( Vector3(x-.5f,y-.5f,z-.5f), Vector3(x+.5f,y+.5f,z+.5f) ); 
};
        static const int type = 2; 
        static const int creatureType = 0;
        static const int occupancy = 20;
        static const int bounty = 1000;
        virtual bool move();
        virtual int getType(); 
        virtual int getOccupancy();
        virtual int getBounty();
        virtual void onSpawn();
        virtual void onDeath();
};



class Crawler : public Creature{
    public:
        static const int creatureType = 1;
        static const int occupancy = 15;
        static const int bounty = 200;
        Crawler(shared_ptr<VisibleEntity> e, int x, int y, int z, Map* m) : Creature(e,x,y,z,m){};
        IntVec3 getRandomDirection();
        virtual bool move();
        virtual int getType(); 
        virtual int getOccupancy();
        virtual int getBounty();
        virtual void onSpawn();
        virtual void onDeath();
};


class Creeper : public Creature{
    public:
        static const int creatureType = 2;
        static const int occupancy = 30;
        static const int bounty = 1000;
        IntVec3 dir;
        int currentSpeed;
        Creeper (shared_ptr<VisibleEntity> e, int x, int y, int z, Map* m) : Creature(e,x,y,z,m){ dir = getRandomDirection(); currentSpeed = 5;};
        IntVec3 getRandomDirection();
        virtual bool move();
        virtual int getType(); 
        virtual int getOccupancy();
        virtual int getBounty();
        virtual void onSpawn();
        virtual void onDeath();
};

class SmartCrawler : public Creature{
    public:
    bool movingUp = false; 
        static const int creatureType = 3;
        static const int occupancy = 25;
        static const int bounty = 50;
        SmartCrawler(shared_ptr<VisibleEntity> e, int x, int y, int z, Map* m) : Creature(e,x,y,z,m){};
        IntVec3 getRandomDirection();
        virtual bool move();
        virtual int getType(); 
        virtual int getOccupancy();
        virtual int getBounty();
        virtual void onSpawn();
        virtual void onDeath();
};

class Archer : public Creature{
    public:
        static const int creatureType = 4;
        static const int occupancy = 40;
        static const int bounty = 700;
        Archer(shared_ptr<VisibleEntity> e, int x, int y, int z, Map* m) : Creature(e,x,y,z,m){};
        virtual bool move();
        virtual int getType(); 
        virtual int getOccupancy();
        virtual int getBounty();
        virtual void onSpawn();
        virtual void onDeath();
};

class Bomber : public Creature{
    public:
        static const int creatureType = 5;
        static const int occupancy = 60;
        static const int bounty = 1000;
        Bomber(shared_ptr<VisibleEntity> e, int x, int y, int z, Map* m) : Creature(e,x,y,z,m){};
        virtual bool move();
        virtual int getType(); 
        virtual int getOccupancy();
        virtual int getBounty();
        virtual void onSpawn();
        virtual void onDeath();
};

class SuperCrawler : public Creature{
    public:
        static const int creatureType = 6;
        static const int occupancy = 30;
        static const int bounty = 500;
        SuperCrawler(shared_ptr<VisibleEntity> e, int x, int y, int z, Map* m) : Creature(e,x,y,z,m){};
        int search(IntVec3 currentPos, int costLeft);
        virtual bool move();
        virtual int getType(); 
        virtual int getOccupancy();
        virtual int getBounty();
        virtual void onSpawn();
        virtual void onDeath();
};
/*
Created by Map.cpp to manage the AABoxes of all of its MapElements 

Allows for raycasting on all AABoxes 

Has helper method for finding vacantCubePositions for placing Cubes 
Map will now handle add and remove of cubes 

Should only keep track of AABoxes 
 */
class AABoxManager{

 protected:

    std::vector<shared_ptr<MapElement> > * allElements; 

    //To be used to find boxes and creatures hit 
    std::vector< shared_ptr<MapElement> > getRayHitElements(Ray ray, int type); 
    



    shared_ptr<MapElement> getClosestElement(Ray ray, std::vector< shared_ptr<MapElement> > elements); 

    std::vector< AABox > getRayHitBoxes(Ray ray, std::vector<AABox> possibleBoxes ); 
    AABox getClosestBox(Ray ray, std::vector<AABox> hitBoxes ); 

    //For finding box placement for cube placement 
    AABox makeBox(int posX, int posY, int posZ);


    std::vector< AABox > groundCubeAA; 

    //NO LONGER NECESSARY, automatically done with MapElement 
    /*
    void addBox(int posX, int posY, int posZ); 
    void removeBox(AABox box); 
    */


 public:
    shared_ptr<MapElement> getRayHitElement(Ray ray, int type);      
    static shared_ptr<AABoxManager> create(  std::vector<shared_ptr<MapElement> > * elements ); 

    AABoxManager( std::vector<shared_ptr<MapElement> > * elements ); 

    shared_ptr<MapElement> findHitElement(Ray ray, int type); 

    IntVec3 findNewCubePos(const Ray& ray); 

    //bool checkLOS( Vector3 start, shared_ptr<MapElement>


};

/** \brief Abstract representation of all the current elements in an instance of a game
    A Map is a collection of MapElement as well as helper data structures. It stores all the information about a game. App provides with specific information about a game i.e. the creature to spawn, the default terrain, used inputs and etc. VisualControl comminucates with it to update the graphics.

    Information is stored in a 3D-grid. Currently this is a 15*15*15 Map
*/
class Map : public ReferenceCountedObject{

 public:
    /** X dimension of the map, currently hardcoded as 15 in many places */
    static const int dimX = 15;
    /** Y dimension of the map, currently hardcoded as 15 in many places */
    static const int dimY = 15;
    /** Z dimension of the map, currently hardcoded as 15 in many places */
    static const int dimZ = 15;

protected:
    void foo();
 public:
    /** Prints debug information if sets to true */
    const static bool debugMap = false; 
    /** Pointer to the scene */
    shared_ptr<DemoScene> m_scene; 
    /** Constructor 
        \param scene the scene the relates to the map
    */
    Map( shared_ptr<DemoScene> scene );
    /** Array of MapElement that is in the map */
    std::vector<shared_ptr<MapElement> > elements; 
    /** Location of Disco Ball */
    const IntVec3 discoBallLoc = IntVec3(7,1,7);
    /** HP left for Disco Ball */
    int discoBallHP;
    /** Manages the AABox for all the objects in the map */
    shared_ptr<AABoxManager> aaBoxManager; 

    shared_ptr<MapElement> getClosestLOSEntity( Vector3 pos, float maxDist, int type ); 

    /** Keeps track of the occupancy of each grid. If it exceeds 100, no more MapElement can be added */
    int gridOccupancy[15][15][15];
    /** Keeps track of if there is a box in the current grid. Used for implementing basic gravity on boxes */
    bool hasBox[15][15][15];
    /** Area damage on each grid towards the Cube (defenders). Reset every turn. Single-target damage is dealted directly in MapElement::run() */
    int attackDamage[15][15][15];
    /** Area damage on each grid towards the Creature (attackers). Reset every turn. Single-target damage is dealted directly in MapElement::run() */
    int defendDamage[15][15][15];
    /** Gold left. Used to construct Cube. Can't construct without gold */
    int gold;
    /** \brief Called by App. Computes and returns an array of changes to the VisibleEntity
        MapElement::run() and MapElement::updateHP() is called for every MapElement in elements. Any elements with HP below zero gets removed. Any elements that changed is included in the array of changes.
    */
    std::vector<shared_ptr<Change> > getChange();
    /** Wrapper for calling add(shared_ptr<VisibleEntity> e, int type, int creatureType, int x, int y, intz) to support old code.

        creatureType is set to 0
    */
    bool add(shared_ptr<VisibleEntity> e, int type, int x, int y, int z);
    /** Add a MapElement to the map.
       \param e corresponding VisibleEntity
       \param type type of MapElement
       \param creatureType subtype
       \param x,y,z initial location
    */
    bool add(shared_ptr<VisibleEntity> e, int type, int creatureType, int x, int y, int z, float HPFactor);
    /** Remove an element with the corresponding VisibleEntity
        \param e VisibleEntity to be removed
    */
    bool remove(shared_ptr<VisibleEntity> e);

    bool canAdd(int type, int creatureType); 

    /** Checks if we can add a box at the given location.

        Checks if there is any element in the grid already and if there is proper support for the box.
        \param x,y,z location to check
    */

    bool canAddBox(int x, int y, int z);
    /** Checks if we can spawn a creature at the given location.
        \param x,y,z location to check
        \param occupancy occupancy of the Creature to be spawned
    */
    bool canSpawnCreature(int x, int y, int z, int occupancy);
    IntVec3 getSelectedBuildPos(Ray ray); 
    shared_ptr<MapElement> getSelectedEntity(Ray ray, int type); 
    bool addBullet(int x, int y, int z, Vector3 velocity); 
    
    /** counts bullet for playing gun sound (can't play every turn other wise too noisy */
    int m_bulletCount = 0;
    //shared_ptr<Sound> gun;

};

#endif
