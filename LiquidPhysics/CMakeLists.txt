find_package(OpenCL REQUIRED)

set(miscFiles
	include/PhysicSolver.h
	src/PhysicSolver.cpp
	include/SPHSolver.h
	src/SPHSolver.cpp
    include/PCISPHSolver.h
	src/PCISPHSolver.cpp
    include/SPHSpatialGrid.h
	src/SPHSpatialGrid.cpp
    cl_kernels/SPHKernels.cl)

set(collisionFiles
	include/Collision/StaticCollisionObject.h
	src/Collision/StaticCollisionObject.cpp
	include/Collision/StaticCollisionBox.h
	src/Collision/StaticCollisionBox.cpp
    include/Collision/StaticCollisionSphere.h
	src/Collision/StaticCollisionSphere.cpp)

set(mathFiles
	include/Math/Vector3D.h
	src/Math/Vector3D.cpp)

set(parallelizationFiles
	include/Parallelization/ParallelComputationInterface.h
    include/Parallelization/OpenCLInterface.h
    src/Parallelization/OpenCLInterface.cpp
    include/Parallelization/ParallelSPHStructs.h)

set(particlesFiles
	include/Particles/SPHParticle.h
	src/Particles/SPHParticle.cpp
	include/Particles/PCISPHParticle.h
	src/Particles/PCISPHParticle.cpp
	include/Particles/SPHParticleEmitter.h
	src/Particles/SPHParticleEmitter.cpp)

set(kernelsFiles
	include/Kernels/DefaultKernel.h
	src/Kernels/DefaultKernel.cpp
	include/Kernels/PressureKernel.h
	src/Kernels/PressureKernel.cpp
    include/Kernels/ViscosityKernel.h
	src/Kernels/ViscosityKernel.cpp
    include/Kernels/SPHKernel.h
	src/Kernels/SPHKernel.cpp)

source_group("" FILES ${miscFiles})
source_group("\\Collision" FILES ${collisionFiles})
source_group("\\Math" FILES ${mathFiles})
source_group("\\Parallelization" FILES ${parallelizationFiles})
source_group("\\Particles" FILES ${particlesFiles})
source_group("\\Kernels" FILES ${kernelsFiles})

add_library(LiquidPhysics STATIC 
	${miscFiles}
	${collisionFiles}
	${mathFiles}
    ${parallelizationFiles}
	${particlesFiles}
    ${kernelsFiles})

target_link_libraries(LiquidPhysics PUBLIC OpenCL::OpenCL)
target_include_directories(LiquidPhysics PUBLIC "include")