#include "ilthpcm.h"

void defineMesh(vector<double> &x, vector<double> &dx, vector<double> &alpha, vector<Layer> layerVector, int noOfElements){
	
	int elementCounter = 0, i, j;
	double dxlayer, alphalayer;
	
	x.resize(noOfElements);
	dx.resize(noOfElements);
	alpha.resize(noOfElements);
	
	for(i=0;i<layerVector.size();i++){
		dxlayer = layerVector[i].thickness/(double)layerVector[i].numLayerElements;
		alphalayer = layerVector[i].solidMatrixThermalConductivity/(layerVector[i].solidMatrixDensity*layerVector[i].solidMatrixHeatCapacity);
		
		for(j=0;j<layerVector[i].numLayerElements;j++){
			alpha[elementCounter] = alphalayer;
			dx[elementCounter] = dxlayer;
			if(elementCounter==0){
				//Surface element
				x[elementCounter] = dx[elementCounter]*0.5;
			}
			else{
				x[elementCounter] = x[elementCounter-1] + 0.5*(dx[elementCounter-1]+dx[elementCounter]);
			}
			elementCounter++;
		}
	}
}