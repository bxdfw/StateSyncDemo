#include "cube.h"

physx::PxVec3 Cube::position(){
    return _physics->getGlobalPose().p;
}

physx::PxVec4 Cube::rotation(){
    auto q = _physics->getGlobalPose().q;
    return physx::PxVec4(q.x,q.y,q.z,q.w);
}