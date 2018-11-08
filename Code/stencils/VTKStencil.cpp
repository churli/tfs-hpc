//
// Created by tommaso on 25/10/18.
//

#include <vector>
#include <cassert>
#include "VTKStencil.h"

VTKStencil::VTKStencil(const Parameters &parameters) : FieldStencil(parameters)
{
    // Now initialize the data structures for printing out VTKs
    auto gridSize = parameters.parallel.localSize;
    assert(gridSize[0] >= 0 && gridSize[1] >= 0 && gridSize[2] >= 0);
    
    int dim = parameters.geometry.dim;
    assert(dim == 2 || dim == 3);
    if (dim == 2)
    {
        numPoints = static_cast<unsigned int>((gridSize[0] + 1) * (gridSize[1] + 1));
        numElements = static_cast<unsigned int>(gridSize[0] * gridSize[1]);
    }
    else
    {
        numPoints = static_cast<unsigned int>((gridSize[0] + 1) * (gridSize[1] + 1) * (gridSize[2] + 1));
        numElements = static_cast<unsigned int>(gridSize[0] * gridSize[1] * gridSize[2]);
    }
    std::cout << "numPoints = " << numPoints << std::endl;
    std::cout << "numElements = " << numElements << std::endl;
    // Now fill the gridPoints
    populateGridPoints(parameters);
}

void VTKStencil::populateGridPoints(const Parameters &parameters)
{
    int i0 = parameters.parallel.firstCorner[0] + 2;
    int j0 = parameters.parallel.firstCorner[1] + 2;
    int iE = i0 + parameters.parallel.localSize[0] + 1; // +1 is because we are on wireframe, not on cells
    int jE = j0 + parameters.parallel.localSize[1] + 1; // +1 is because we are on wireframe, not on cells
    if (_parameters.geometry.dim == 2)
    {
        for (int j = j0; j < jE; ++j)
        {
            for (int i = i0; i < iE; ++i)
            {
                Triple<FLOAT> newGridPoint{};
                newGridPoint.x = _parameters.meshsize->getPosX(i, j);
                newGridPoint.y = _parameters.meshsize->getPosY(i, j);
                newGridPoint.z = 0;
                gridPoints.push_back(newGridPoint);
            }
            
        }
    }
    else
    {
        int k0 = parameters.parallel.firstCorner[2] + 2;
        int kE = k0 + parameters.parallel.localSize[2] + 1; // +1 is because we are on wireframe, not on cells
        for (int k = k0; k < kE; ++k)
        {
            for (int j = j0; j < jE; ++j)
            {
                for (int i = i0; i < iE; ++i)
                {
                    Triple<FLOAT> newGridPoint{};
                    newGridPoint.x = _parameters.meshsize->getPosX(i, j);
                    newGridPoint.y = _parameters.meshsize->getPosY(i, j);
                    newGridPoint.z = _parameters.meshsize->getPosZ(i, j, k);
                    gridPoints.push_back(newGridPoint);
                }
            }
        }
    }
}

void VTKStencil::apply(FlowField &flowField, int i, int j)
{
    const int obstacle = flowField.getFlags().getValue(i, j);
    if ((obstacle & OBSTACLE_SELF) == 0)
    {
        // If cell is fluid, print the actual values
        // Store the pressure value
        FLOAT tmpPress;
        FLOAT tmpVel[2];
        flowField.getPressureAndVelocity(tmpPress, tmpVel, i, j);
        pressureValues.push_back(tmpPress);
        
        // Store the velocity values
        Triple<FLOAT> v{};
        v.x = tmpVel[0];
        v.y = tmpVel[1];
        v.z = 0;
        velocityValues.push_back(v);
        geometryValues.push_back(0);
    }
    else
    {
        // If obstacle, everything is 0
        pressureValues.push_back(0);
        Triple<FLOAT> v{};
        v.x = 0;
        v.y = 0;
        v.z = 0;
        velocityValues.push_back(v);
        geometryValues.push_back(1);
    
    }
}

void VTKStencil::apply(FlowField &flowField, int i, int j, int k)
{
    const int obstacle = flowField.getFlags().getValue(i, j, k);
    if ((obstacle & OBSTACLE_SELF) == 0)
    {
        // If cell is fluid, print the actual values
        // Store the pressure value
        FLOAT tmpPress;
        FLOAT tmpVel[3];
        flowField.getPressureAndVelocity(tmpPress, tmpVel, i, j, k);
        pressureValues.push_back(tmpPress);
        
        // Store the velocity values
        Triple<FLOAT> v{};
        v.x = tmpVel[0];
        v.y = tmpVel[1];
        v.z = tmpVel[2];
        velocityValues.push_back(v);
        geometryValues.push_back(0);
    }
    else
    {
        // If obstacle, everything is 0
        pressureValues.push_back(0);
        Triple<FLOAT> v{};
        v.x = 0;
        v.y = 0;
        v.z = 0;
        velocityValues.push_back(v);
        geometryValues.push_back(1);
    }
}

void VTKStencil::write(FlowField &flowField, int timeStep)
{
    // Build filename and open file
    std::string filename = _parameters.vtk.prefix + "_" + std::to_string(timeStep) + ".vtk";
    std::ofstream outfile;
    outfile.open(filename);
    
    // Write header
    outfile << header << std::endl;
    
    // Write grid points
    // todo: check if we must do -2 on all this dimensions!
    int sizeX = _parameters.parallel.localSize[0];
    int sizeY = _parameters.parallel.localSize[1];
    int sizeZ = _parameters.parallel.localSize[2];
    outfile << "DATASET STRUCTURED_GRID" << std::endl;
    outfile << "DIMENSIONS"
            << " " << sizeX + 1
            << " " << sizeY + 1
            << " " << sizeZ + 1
            << std::endl;
    
    if (numPoints != gridPoints.size())
    {
        std::cout << "ERROR: numPoints != gridPoints.size() --> "
                  << numPoints << " != " << gridPoints.size() << std::endl;
    }
    assert(numPoints == gridPoints.size());
    
    outfile << "POINTS " << numPoints << " float" << std::endl;
    for (Triple<FLOAT> gp : gridPoints)
    {
        outfile << gp.x
                << " " << gp.y
                << " " << gp.z
                << std::endl;
    }
    outfile << "" << std::endl;
    
    if (numElements != pressureValues.size())
    {
        std::cout << "ERROR: numElements != pressureValues.size() --> "
                  << numElements << " != " << pressureValues.size() << std::endl;
    }
    if (numElements != velocityValues.size())
    {
        std::cout << "ERROR: numElements != velocityValues.size() --> "
                  << numElements << " != " << velocityValues.size() << std::endl;
    }
    if (numElements != geometryValues.size())
    {
        std::cout << "ERROR: numElements != geometryValues.size() --> "
                  << numElements << " != " << geometryValues.size() << std::endl;
    }
    assert(numElements == pressureValues.size());
    assert(numElements == velocityValues.size());
    assert(numElements == geometryValues.size());

    outfile << "CELL_DATA " << numElements << std::endl;
    // Write pressure values
    outfile << "SCALARS pressure float 1" << std::endl;
    outfile << "LOOKUP_TABLE default" << std::endl;
    for (FLOAT pv : pressureValues)
    {
        outfile << pv << std::endl;
    }
    outfile << "" << std::endl;
    
    // Write velocity values
    outfile << "VECTORS velocity float" << std::endl;
    for (Triple<FLOAT> vv : velocityValues)
    {
        outfile << vv.x
                << " " << vv.y
                << " " << vv.z
                << std::endl;
    }
    outfile << "" << std::endl;
    
    // Write geometry values
    outfile << "SCALARS geometry int 1" << std::endl;
    outfile << "LOOKUP_TABLE default" << std::endl;
    for (int gv : geometryValues)
    {
        outfile << gv << std::endl;
    }
    outfile << "" << std::endl;
    
    // Close file
    outfile.close();
    
    // Cleaning the data lists
    pressureValues.clear();
    velocityValues.clear();
    geometryValues.clear();
}

//eof
