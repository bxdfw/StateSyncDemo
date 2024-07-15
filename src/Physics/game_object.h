#include <physx/PxPhysicsAPI.h>
#include "physx_manager.h"
class GameObject {
public:
    GameObject(const physx::PxTransform& transform, physx::PxShape& shape, physx::PxReal density) {
        _actor = PhysxManager::Instance().create_dynamic(transform, shape, density);
    }

    virtual ~GameObject() {
        // todo release
    }

    // 其他方法...

protected:
    physx::PxRigidDynamic* _actor; // 物体对应的PhysX对象
};
