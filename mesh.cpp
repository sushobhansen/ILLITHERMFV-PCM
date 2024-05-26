#include "ilthpcm.h"

void defineMesh(vector<double> &x, vector<double> &dx, vector<Layer> layerVector, int noOfElements){
	
	int elementCounter = 0, i, j;
	double dxlayer;
	
	x.resize(noOfElements);
	dx.resize(noOfElements);
	
	for(i=0;i<layerVector.size();i++){
		dxlayer = layerVector[i].thickness/(double)layerVector[i].numLayerElements;
		
		for(j=0;j<layerVector[i].numLayerElements;j++){
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