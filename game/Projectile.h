#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "Vector2d.h"
#include "Player.h"

//
// In this class there is a static array of projectile objects,
// this way projectiles are global and nobody owns them
//

typedef struct {
    int kind;
    Vector2d pos; 
    Vector2d dir; 
    float speed;
    float time_alive;
    float max_time_alive;

    int is_alive;
    int is_allocated;
} Projectile;

enum PROJECTILE_KINDS {
    ZAP,
    NUM_PROJECTILE_KINDS,
};

void Proj_init_all();

//initialize a generic Projectile object, not connected to the internal system
void Proj_init(Projectile* self, Vector2d pos, Vector2d dir);

//launch projectile at given index
Projectile* Proj_launch_at_index(int kind, Vector2d pos, Vector2d dir, int index);

//this will automatically find an open index in the array,
//create a projectile in the private array, then
//fill index with the index of that projectile in the array
Projectile* Proj_launch(int kind, Vector2d pos, Vector2d dir, int* index);

void Proj_update_all(float dt);

int Proj_num_projectiles();
Projectile* Proj_get(int index);

#endif
