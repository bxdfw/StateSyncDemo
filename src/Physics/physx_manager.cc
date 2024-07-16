#include "physx_manager.h"

void PhysxManager::update(float timeStep){
    _scene->simulate(timeStep);
    _scene->fetchResults(true);
}

void PhysxManager::initialize(){
	_foundation = PxCreateFoundation(PX_PHYSICS_VERSION, _default_allocator_callback, _default_error_callback);
    if (!_foundation) {
        throw("PxCreateFoundation failed!");
    }

#ifdef _DEBUG
    _pvd = PxCreatePvd(*_foundation);
    physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
    _pvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);
#endif

    _tolerance_scale.length = 1;
    _tolerance_scale.speed = 1;

#ifdef _DEBUG
    _physics = PxCreatePhysics(PX_PHYSICS_VERSION, *_foundation, _tolerance_scale, true, _pvd);
#else
    _physics = PxCreatePhysics(PX_PHYSICS_VERSION, *_foundation, _tolerance_scale, true, nullptr);
#endif
    if (!_physics) {
        throw("PxCreatePhysics failed!");
    }
	//创建场景
    physx::PxSceneDesc sceneDesc(_physics->getTolerancesScale());
    sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
    _dispatcher = physx::PxDefaultCpuDispatcherCreate(2);
    sceneDesc.cpuDispatcher = _dispatcher;
    sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
    _scene = _physics->createScene(sceneDesc);
	
    if (!_scene) {
        throw("createScene failed!");
    }

#ifdef _DEBUG
    physx::PxPvdSceneClient* pvd_client = _scene->getScenePvdClient();
    if (pvd_client) {
        pvd_client->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
        pvd_client->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
        pvd_client->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
    }
#endif

    //todo
    physx::PxTransform cubePose = physx::PxTransform(physx::PxVec3(0.0f, -0.1f, 0.0f), physx::PxIdentity);

    physx::PxBoxGeometry cubeGeometry(25.0f, 0.1f, 25.0f); // 大小为10x1x10
    physx::PxRigidStatic* cube = physx::PxCreateStatic(*_physics, cubePose, cubeGeometry, *_physics->createMaterial(0.5f, 0.5f, 0.5f));
    // 将立方体添加到场景中
    _scene->addActor(*cube);
}

physx::PxRigidDynamic* PhysxManager::create_dynamic(
	const physx::PxTransform& transform, 
	physx::PxShape& shape,
	physx::PxReal density)
{
    physx::PxRigidDynamic* dynamic = physx::PxCreateDynamic(*_physics,transform,shape,density);
    _scene->addActor(*dynamic);

    return dynamic;
}

physx::PxShape* PhysxManager::create_shape(const physx::PxGeometry& geometry, physx::PxMaterial& material){
    return _physics->createShape(geometry, material);
}

physx::PxMaterial* PhysxManager::create_material(physx::PxReal staticFriction, physx::PxReal dynamicFriction, physx::PxReal restitution){
    return _physics->createMaterial(staticFriction,dynamicFriction,restitution);
}

// 返回RigidDynamic  由用户release()
physx::PxRigidDynamic* PhysxManager::create_new_player(){
    physx::PxTransform transform(physx::PxVec3(0.0f, 1.0f, 0.0f), physx::PxQuat(physx::PxIdentity));
    physx::PxSphereGeometry geometry(0.5f);
    physx::PxMaterial* material = create_material(0.5f,0.5f,0.5f);
    physx::PxShape* shape = create_shape(geometry, *material);
    physx::PxReal volume = (4.0f / 3.0f) * physx::PxPi * pow(0.5f, 3);
    physx::PxReal density = 1.0f / volume;
    physx::PxRigidDynamic* rigidBody = create_dynamic(transform, *shape, density);
    rigidBody->setAngularDamping(0.05f);

    return rigidBody;
}

physx::PxRigidDynamic* PhysxManager::create_cube(physx::PxVec3 position){
    //todo 把密度等参数变成传参 给cube类管理
    physx::PxTransform transform(position, physx::PxQuat(physx::PxIdentity));
    physx::PxVec3 dimensions(0.3f, 0.3f, 0.3f);
    physx::PxBoxGeometry geometry(dimensions);
    physx::PxMaterial* material = create_material(0.01f,0.01f,0.01f);
    physx::PxShape* shape = create_shape(geometry, *material);
    physx::PxReal density = 0.0001f/pow(0.1f, 3);
    physx::PxRigidDynamic* rigidBody = create_dynamic(transform, *shape, density);

    return rigidBody;
}


void PhysxManager::clean(){
	if (_scene) {
        _scene->release();
        _scene = nullptr;
    }
    if (_dispatcher) {
        _dispatcher->release();
        _dispatcher = nullptr;
    }
    if (_physics) {
        _physics->release();
        _physics = nullptr;
    }
    if (_pvd) {
        _pvd->release();
        _pvd = nullptr;
    }
    if (_foundation) {
        _foundation->release();
        _foundation = nullptr;
    }
}