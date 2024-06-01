# ILLITHERMFV-PCM
A repository for an advanced version of ILLITHERM-FV that also implements PCMs and thermochromic materials. Developed by Dr. Sushobhan Sen at the University of Pittsburgh and IIT Gandhinagar.

### Input file format - layers
**GENERAL SECTION**
This section starts with "*GENERAL" and contains information related to the time steps.
First input: number of time steps per hour of analysis (must be integer). Second input: under-relaxation factor For example:
```
*GENERAL
10 12 0.5 1e-4
```
Where 10 is the number of steps per hour, 12 is the number of inner iterations for updating liquid fraction, 0.5 is the under-relaxation factor, and 1e-4 is the tolerance for convergence of the liquid fraction update.

**SURFACE SECTION**
This section starts with "*SURFACE" and contains information related to the surface i.e., albedo and emissivity. This consists of 1 line of **space delimited** data, with the first value being a surface type code.
If the first value (surface type code, integer) is `0`, it represents a typical surface. In this case, the next two inputs are the albedo and emissivity. For example:
```
*SURFACE
0 0.2 0.93
```
Where 0 is the surface type code, 0.2 is the albedo, and 0.93 is the emissivity.
On the other hand, if the first value is `1`, it represents a thermochromic surface. This this case, the next four inputs are the low and high albedo transition temperatures (T1therm and T2therm, respectively, in C), the low and high albedo values (alphaL and alphaH, respectively). The last input is the emissivity as usual. For example:
```
*SURFACE
1 28 32 0.6 0.8 0.93
```
Where 1 is the surface type code, 28C and 32C are the low and high albedo transition temperatures, respectively, 0.6 and 0.8 are the low and high albedo, respectively, and 0.93 is the emissivity.

**LAYERS SECTION**
This section starts with "*LAYERS" and contains information related to the layer properties. The first line consists of just one input, which is the number of layers. This must be followed by one **space delimited** line for the properties of each layer. Each line is formatted as follows:
The first input is the layer type code (integer). If this is `0`, then the layer corresponds to one that does not undergo phase change. In this case, the next four inputs are the layer thickness (m), thermal conductivity (W/m K), heat capacity (J/kg K), and the density (kg/m3). The last input is the number of sub-layers in the analysis (integer).
On the other hand, if the first value is `1`, it represents a layer containing PCMs in a solid matrix. The next three inputs are the same as above for the solid matrix. The next input is the solid matrix fraction (must be less than one), followed by two inputs: the lower, indiate, and upper transition temperatures (in C), respectively. The next four inputs are for the PCMs themselves: the solid and liquid thermal conductivity (W/m K), and the solid and liquid heat capacity (J/kg K). The next input is the PCM density (kg/m3, assumed not to change with phase transition) and the PCM latent heat (J/kg). The last input is the number of sub-layers in the analysis (integer). For example:
```
*LAYERS
2
0 0.15 1.4 880 2330 10
1 0.25 1.4 880 2330 0.7 27 28 29 0.2 0.13 1500 2153 995 150000 10
```
The first line contains `2`, which indicates that there are two layers. This must then be followed by one line per layer.
The second line is for the first layer. The first input is `0` so this is a layer that does not undergo phase change. It has a thickness of 0.15 m, a thermal conductivity of 1.4 W/mK, a heat capacity of 880 J/kgK, and a density of 2330 kg/m3. The layer subdivided into 10 sub-layers.
The third line is for the second layer. The first input is `1` so this layer contains PCMs. It has a thickness of 0.25 m, and the solid matrix has  thermal conductivity of 1.4 W/mK, a heat capacity of 880 J/kgK, and a density of 2330 kg/m3. The solid matrix forms 0.7 parts by volume of the layer. The PCMs have a lower, intermediate, and upper transition temperature of 27C, 28C, and 29C, respectively; a solid and liquid thermal conductivity of 0.2 and 0.13 W/mK, respectively; a solid and liquid heat capacity of 1500 and 2153 J/kgK, respectively; a density of 995 kg/m3; a latent heat of 150,000 J/kg; and is subdivided into 10 sub-layers.

**SUBGRADE SECTION**
This section starts with "*SUBGRADE" and contains information related to the subgrade. The subgrade is a layer with a default layer code of `0`, a built-in thickness of 1000 m, and 10 sub-layers. The user only enters three inputs: the thermal conductivity (W/m K), heat capacity (J/kg K), and the density (kg/m3). For example:
```
*SUBGRADE
0.54 347 1450
```
Where the subgrade has a thermal conductivity of 0.54 W/mK, a heat capacity of 347 J/kgK, and a density of 1450 kg/m3.

### Input file format - weather
The weather data is a **space delimited** file consisting of one line for every hour of weather data to be analyzed. Each line starts with four entries: the year, month, day, and hour (with midnight as 0) of analysis. These four values are needed to estimate the amount of solar radiation incident on the surface. These inputs must be integers. The rest of the line consists of six entries: air temperature (in C), wind speed (in m/s), percent of sunshine (%, which is 100% - cloud cover), the dew point temperature (in C), the latitude (N is positive), and the longitude (E is positive). For example:
```
1979 1 1 2 1.777777778 5.811354493 1 -1.222222222 35.02 -106.37
```
This entry is for January 1, 1979 at 2:00 AM. The air temperature is 1.78C, wind speed is 5.81 m/s, there is only 1% of sunshine (i.e., 99% cloud cover), dew point is -1.22C, latitude is 35.02N, and longitude is 106.37W.

### Compile program
The program was developed on Linux (Ubuntu) with the g++ compiler based on the C++ 2011 standard. Successful compilation on other configurations is not guaranteed. A `Makefile` is included to compile the program, simple run the following command:
```
make ilthpcm
```
This will create ilthpcm.out, which is the executable. 

### Run program
The ilthpcm.out executable expects three command-line arguments: the input layers file, the input weather file, and the output file. The first two must be formatted as specified above. To run the program, simply call the executable with the command-line arguments. For example:
```
./ilthpcm.out layersinput1.txt weatherinput1.txt tempoutput1.txt
```

### Output file format
TBD