#ifndef CUBE_H
#define CUBE_H

#include "../Physics/physx_manager.h"

//todo 体积、密度等交给cube管理
class Cube {
public:
    Cube(int id, physx::PxVec3 postion)
        :_id(id), _physics(PhysxManager::Instance().create_cube(postion)){ }

    ~Cube(){_physics->release();}
    int id(){return _id;}

    physx::PxVec3 position();
    physx::PxVec4 rotation();

private:
    int _id;
    physx::PxRigidDynamic* _physics;
};

#endif //CUBE_H