#ifndef PHYXS_MANAGER_H
#define PHYXS_MANAGER_H

#include <physx/PxPhysicsAPI.h>

class PhysxManager {
public:
    static PhysxManager& Instance() {
        static PhysxManager instance;
        return instance;
    }

    void initialize();
    void update(float timeStep);
    physx::PxRigidDynamic* create_dynamic(
		const physx::PxTransform& transform, 
		physx::PxShape& shape,
		physx::PxReal density
	);

    physx::PxShape* create_shape(const physx::PxGeometry& geometry, physx::PxMaterial& material);
	physx::PxMaterial* create_material(physx::PxReal staticFriction = 0.5f, physx::PxReal dynamicFriction = 0.5f, physx::PxReal restitution = 0.5f);

    physx::PxRigidDynamic* create_new_player();
    physx::PxRigidDynamic* create_cube(physx::PxVec3 position);

private:
    PhysxManager() {}
    ~PhysxManager() { clean(); }

    PhysxManager(const PhysxManager&) = delete;
    PhysxManager& operator=(const PhysxManager&) = delete;

    void clean();

private:
    physx::PxDefaultAllocator 		_default_allocator_callback;
    physx::PxDefaultErrorCallback 	_default_error_callback;
    physx::PxDefaultCpuDispatcher* 	_dispatcher = nullptr;
    physx::PxTolerancesScale 		_tolerance_scale;
    physx::PxFoundation* 			_foundation = nullptr;
    physx::PxPhysics*	 			_physics = nullptr;
    physx::PxScene* 				_scene = nullptr;
    physx::PxPvd* 					_pvd = nullptr;
};

#endif // PHYXS_MANAGER_H

