#include "ilthpcm.h"

void readInputFile(int &numStepsPerHour, double &underrelax_factor, Surface &surface, vector<Layer> &layers, string inputFile)
{
    ifstream ifile;
    string buf, heading;
    int typecode, numlayers;

    try
    {
        ifile.open(inputFile, ios::in);

        if(!ifile.is_open())
        {
            throw runtime_error("Failed to open" + inputFile + ". Is the file and path correct?\n");
        }

        /*Read file*/
        //*GENERAL section
        getline(ifile, heading);
        if(!heading.compare("*GENERAL"))
        {
            throw runtime_error("Failed to find *GENERAL section. Check input file.\n");
        }
        getline(ifile, buf);
        sscanf(buf.c_str(), "%d %lf", &numStepsPerHour, &underrelax_factor);

        if(underrelax_factor > 1.0 || underrelax_factor < 0.0)
        {
            throw runtime_error("Under-relaxation factor must be between 0 and 1.\'n");
        }

        //*SURFACE section
        getline(ifile, heading);
        if(!heading.compare("*SURFACE"))
        {
            throw runtime_error("Failed to find *SURFACE section. Check input file.\n");
        }
        getline(ifile, buf);
        sscanf(buf.c_str(), "%d ", &typecode);
        if(typecode == -1) //Supposed to be a placeholder
        {
            throw runtime_error("Surface information has an invalid type code (first entry). Check *SURFACE section.\n");
        }
        else if(typecode == 0) //surface code is 0 - no thermochromic surface
        {
            sscanf(buf.c_str(), "%d %lf %lf ", &surface.surfaceCode, &surface.isothermalAlbedo, &surface.isothermalEmissivity);
        }
        else //surface code is 1 - thermochromic
        {
            sscanf(buf.c_str(), "%d %lf %lf %lf %lf %lf ", &surface.surfaceCode, &surface.T1therm, &surface.T2therm, &surface.albedoLow, &surface.albedoHigh, &surface.isothermalEmissivity);
        }

        //*LAYERS section
        getline(ifile, heading);
        if(!heading.compare("*LAYERS"))
        {
            throw runtime_error("Failed to find *LAYERS section. Check input file.\n");
        }
        getline(ifile, buf);
        sscanf(buf.c_str(), "%d ", &numlayers);

        for (int ilayer = 0; ilayer < numlayers; ilayer++)
        {
             getline(ifile, buf);
             sscanf(buf.c_str(), "%d ", &typecode);
             if(typecode == -1) //Supposed to be a placeholder
             {
                throw runtime_error("Layer information for layer " + to_string(ilayer+1) + " has an invalid type code (first entry). Check *LAYERS section.\n");
             }
             else if(typecode == 0) //No PCMs
             {
                Layer tempLayer;
                sscanf(buf.c_str(), "%d %lf %lf %lf %lf %d ", &tempLayer.layerCode, &tempLayer.thickness, &tempLayer.solidMatrixThermalConductivity, &tempLayer.solidMatrixHeatCapacity, &tempLayer.solidMatrixDensity, &tempLayer.numLayerElements);
                tempLayer.fl.assign(tempLayer.numLayerElements, 0.0);
                layers.push_back(tempLayer);

             }
             else //Layer has PCMs
             {
                Layer tempLayer;
                sscanf(buf.c_str(), "%d %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %d ", &tempLayer.layerCode, &tempLayer.thickness, &tempLayer.solidMatrixThermalConductivity, &tempLayer.solidMatrixHeatCapacity, &tempLayer.solidMatrixDensity, &tempLayer.solidMatrixFraction, &tempLayer.PCMLowerTransitionTemp, &tempLayer.PCMIntermediateTransitionTemp, &tempLayer.PCMUpperTransitionTemp, &tempLayer.PCMSolidThermalConductivity, &tempLayer.PCMLiquidThermalConductivity, &tempLayer.PCMSolidHeatCapacity, &tempLayer.PCMLiquidHeatCapacity, &tempLayer.PCMDensity, &tempLayer.PCMLatentHeat, &tempLayer.numLayerElements);
                
                if(tempLayer.solidMatrixFraction < 0.0 || tempLayer.solidMatrixFraction > 1.0)
                {
                    throw runtime_error("Solid matrix fraction must be between 0 and 1.");
                }

                //Need to check that lawer < intermediate < upper temps

                //Initialization of properties
                tempLayer.effectiveDensity = (tempLayer.solidMatrixFraction * tempLayer.solidMatrixDensity) + (1.0 - tempLayer.solidMatrixFraction) * tempLayer.PCMDensity;
                tempLayer.fl.assign(tempLayer.numLayerElements, 0.0);
                tempLayer.enthalpy.assign(tempLayer.numLayerElements, 0.0);
                tempLayer.effectiveThermalConductivity.assign(tempLayer.numLayerElements, tempLayer.solidMatrixThermalConductivity);
                tempLayer.effectiveHeatCapacity.assign(tempLayer.numLayerElements, tempLayer.solidMatrixHeatCapacity);
                layers.push_back(tempLayer);
             }
        }

        //*SUBGRADE section
        getline(ifile, heading);
        if(!heading.compare("*SUBGRADE"))
        {
            throw runtime_error("Failed to find *SUBGRADE section. Check input file.\n");
        }
        else //Adding an else here to force scope of tempLayer
        {
            Layer tempLayer;
            getline(ifile, buf);
            tempLayer.layerCode = 0; //Subgrade cannot have PCMs
            tempLayer.thickness = 1000.0; //Subgrade always has a thickness of 1000 m
            tempLayer.numLayerElements = 10; //Subgrade always has 10 sub-layers
            sscanf(buf.c_str(), "%lf %lf %lf ", &tempLayer.solidMatrixThermalConductivity, &tempLayer.solidMatrixHeatCapacity, &tempLayer.solidMatrixDensity);
            layers.push_back(tempLayer);
        }
    }
    catch(exception& e)
    {
        cout << "***Error in reading input layers file: \n";
        cout << e.what();
        cout << "Program exiting \n";
        exit(1);
    }
}

void readWeatherData(vector<Weather>& weatherData, string inputFile)
{
    ifstream ifile;
    string buf;
    Weather tempWeather;

    try
    {
        ifile.open(inputFile, ios::in);

        if(!ifile.is_open())
        {
            throw runtime_error("Failed to open" + inputFile + ". Is the file and path correct?\n");
        }

        /*Read file*/
        while(getline(ifile,buf))
        {
                sscanf(buf.c_str(), "%d %d %d %d %lf %lf %lf %lf %lf %lf ", &tempWeather.Year, &tempWeather.Month, &tempWeather.Day, &tempWeather.Hour, &tempWeather.AirTemp, &tempWeather.WindSpeed, &tempWeather.PctSun, &tempWeather.DewPoint, &tempWeather.Latitude, &tempWeather.Longitude);
                weatherData.push_back(tempWeather);
        }
    }
    catch(exception& e)
    {
        cout << "***Error in reading input weather file: \n";
        cout << e.what();
        cout << "Program exiting \n";
        exit(1);
    }
    
}