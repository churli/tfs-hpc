#ifndef _VTK_STENCIL_H_
#define _VTK_STENCIL_H_

#include "../Definitions.h"
#include "../Parameters.h"
#include "../Stencil.h"
#include "../FlowField.h"
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

/** TODO WS1: Stencil for writing VTK files
 *
 * When iterated with, creates a VTK file.
 */
class VTKStencil : public FieldStencil<FlowField>
{

public:
    unsigned int numPoints, numElements;
    std::vector<Triple<double>> gridPoints;
    std::vector<FLOAT> pressureValues;
    std::vector<Triple<FLOAT>> velocityValues;
    std::vector<int> geometryValues;
    const std::string header = "# vtk DataFile Version 2.0\n"
                               "Really cool turbulent flow simulation! :)\n"
                               "ASCII\n";
    
public:
    
    /** Constructor
     *
     * @param prefix String with the prefix of the name of the VTK files
     */
    VTKStencil(const Parameters &parameters);
    
    /** 2D operation for one position
     *
     * @param flowField State of the flow field
     * @param i Position in the x direction
     * @param j Position in the y direction
     */
    void apply(FlowField &flowField, int i, int j);
    
    /** 3D operation for one position
     *
     * @param flowField State of the flow field
     * @param i Position in the x direction
     * @param j Position in the y direction
     * @param k Position in the z direction
     */
    void apply(FlowField &flowField, int i, int j, int k);
    
    /** Writes the information to the file
     * @param flowField Flow field to be written
     */
    void write(FlowField &flowField, int timeStep);

private:
    void populateGridPoints(const Parameters &parameters);
};

#endif
