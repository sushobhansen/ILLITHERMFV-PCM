#include "ilthpcm.h"

void update_thermal_properties(vector<Layer> &layers)
{
	for(int i = 0; i < layers.size(); i++)
	{
		double kPCM, CPCM;
		if(layers[i].layerCode == 1) //if layer has PCMs
		{
			for(int j = 0; j < layers[i].numLayerElements; j++)
			{
				kPCM = (1.0 - layers[i].fl[j])*layers[i].PCMSolidThermalConductivity + layers[i].fl[j]*layers[i].PCMLiquidThermalConductivity;
				CPCM = (1.0 - layers[i].fl[j])*layers[i].PCMSolidHeatCapacity + layers[i].fl[j]*layers[i].PCMLiquidHeatCapacity;

				layers[i].effectiveThermalConductivity[j] = layers[i].solidMatrixFraction*layers[i].solidMatrixThermalConductivity + (1.0 - layers[i].solidMatrixFraction)*kPCM;
				layers[i].effectiveHeatCapacity[j] = layers[i].solidMatrixFraction*layers[i].solidMatrixHeatCapacity + (1.0 - layers[i].solidMatrixFraction)*CPCM;

				layers[i].effectiveAlpha[j] = layers[i].effectiveThermalConductivity[j]/(layers[i].effectiveDensity*layers[i].effectiveHeatCapacity[j]);
			}
		}
	}
}

void update_liquid_fraction(vector<Layer> &layers, vector<double> T)
{
	int elementOffset = 0;
	double sigmaS, sigmaL, nodeTemperature;

	for (size_t i = 0; i < layers.size(); i++)
	{
		if(layers[i].layerCode == 1) //if layer has PCMs
		{
			sigmaS = (sqrt(2.0)/4.0)*(layers[i].PCMIntermediateTransitionTemp - layers[i].PCMLowerTransitionTemp);
			sigmaL = (sqrt(2.0)/4.0)*(layers[i].PCMUpperTransitionTemp - layers[i].PCMIntermediateTransitionTemp);

			for(int j = 0; j < layers[i].numLayerElements; j++)
			{
				nodeTemperature = T[elementOffset + j];
				if(nodeTemperature <= layers[i].PCMIntermediateTransitionTemp)
				{
					layers[i].fl[j] = erf((nodeTemperature - layers[i].PCMIntermediateTransitionTemp)/sigmaS) + 1.0;
					layers[i].fl[j] *= sigmaS/(sigmaS + sigmaL);
				}
				else
				{
					layers[i].fl[j] = sigmaL*erf((nodeTemperature - layers[i].PCMIntermediateTransitionTemp)/sigmaL) + sigmaS;
					layers[i].fl[j] *= 1.0/(sigmaS + sigmaL);
				}
			}
		}
		
		elementOffset += layers[i].numLayerElements;
	}
	
}

void update_enthalpy(vector<double> T, vector<Layer> &layers)
{
    int elementOffset = 0;
	double nodeTemperature;

	for (size_t i = 0; i < layers.size(); i++)
	{
		if(layers[i].layerCode == 1) //if layer has PCMs
		{
			for(int j = 0; j < layers[i].numLayerElements; j++)
			{
				nodeTemperature = T[elementOffset + j];
                layers[i].enthalpy[j] = nodeTemperature*layers[i].PCMDensity*(layers[i].PCMLiquidHeatCapacity - layers[i].PCMSolidHeatCapacity) + layers[i].PCMDensity * layers[i].PCMLatentHeat;
			}
		}
		
		elementOffset += layers[i].numLayerElements;
	}
}

void assign_layer_to_element(vector<double>& alpha, vector<double>& deltaH, vector<Layer> layers, int noOfElements)
{
	int elementNum = 0;

	for (size_t i = 0; i < layers.size(); i++)
	{
		for(int j = 0; j < layers[i].numLayerElements; j++)
		{
			alpha[elementNum] = layers[i].effectiveAlpha[j] ;
			deltaH[elementNum] = layers[i].enthalpy[j];
			elementNum++;
		}
	}

	if(elementNum != noOfElements)
	{
		throw runtime_error("elementNum != noOfElements in assign_alpha");
	}
}