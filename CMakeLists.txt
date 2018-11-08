# Refer to https://github.com/jedbrown/cmake-modules/blob/master/FindPETSc.cmake
# and to http://jacobmerson.com/2016/01/17/cmake-petsc2.html
# for hints on how to use CMake with PETSc

cmake_minimum_required(VERSION 3.12)
project(ns)

set(CMAKE_CXX_STANDARD 14)

list (APPEND CMAKE_MODULE_PATH "${ns_SOURCE_DIR}/cmake-modules")

find_package(PETSc REQUIRED)
message (STATUS "PETSC COMPILER ${PETSC_COMPILER}")

include_directories(${PETSC_INCLUDES})
add_definitions(${PETSC_DEFINITIONS})

include_directories(.)
include_directories(3rdparty/tinyxml2)
include_directories(parallelManagers)
include_directories(solvers)
include_directories(stencils)

link_libraries(${PETSC_LIBRARIES})

add_executable(ns
        3rdparty/tinyxml2/tinyxml2.cpp
        3rdparty/tinyxml2/tinyxml2.h
        parallelManagers/PetscParallelConfiguration.cpp
        parallelManagers/PetscParallelConfiguration.h
        solvers/PetscSolver.cpp
        solvers/PetscSolver.h
        solvers/SORSolver.cpp
        solvers/SORSolver.h
        stencils/BFInputStencils.cpp
        stencils/BFInputStencils.h
        stencils/BFStepInitStencil.cpp
        stencils/BFStepInitStencil.h
        stencils/FGHStencil.cpp
        stencils/FGHStencil.h
        stencils/InitTaylorGreenFlowFieldStencil.h
        stencils/MaxUStencil.cpp
        stencils/MaxUStencil.h
        stencils/MovingWallStencils.cpp
        stencils/MovingWallStencils.h
        stencils/NeumannBoundaryStencils.cpp
        stencils/NeumannBoundaryStencils.h
        stencils/ObstacleStencil.cpp
        stencils/ObstacleStencil.h
        stencils/PeriodicBoundaryStencils.cpp
        stencils/PeriodicBoundaryStencils.h
        stencils/RHSStencil.cpp
        stencils/RHSStencil.h
        stencils/StencilFunctions.h
        stencils/VelocityStencil.cpp
        stencils/VelocityStencil.h
        stencils/VTKStencil.h
        stencils/VTKStencil.cpp
        Configuration.cpp
        Configuration.h
        DataStructures.cpp
        DataStructures.h
        Definitions.h
        FlowField.cpp
        FlowField.h
        GlobalBoundaryFactory.cpp
        GlobalBoundaryFactory.h
        Iterators.h
        LinearSolver.cpp
        LinearSolver.h
        main.cpp
        Meshsize.cpp
        Meshsize.h
        MeshsizeFactory.h
        Parameters.h
        SimpleTimer.cpp
        SimpleTimer.h
        Simulation.h
        Stencil.h)
