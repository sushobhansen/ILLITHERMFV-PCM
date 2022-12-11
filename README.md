# ILLITHERMFV-PCM
A repository for an advanced version of ILLITHERM-FV that also implements PCMs and thermochromic materials

### Input file format - layers
**GENERAL SECTION**
This section starts with "*GENERAL" and contains information related to the time steps.
First input: number of time steps per hour of analysis (must be integer). For example:
```
*GENERAL
10
```
Where 10 is the number of steps per hour

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
The first input if the layer type code (integer). If this is `0`, then the layer corresponds to one that does not undergo phase change. In this case, the next four inputs are the layer thickness (m), thermal conductivity (W/m K), heat capacity (J/kg K), and the density (kg/m3). The last input is the number of sub-layers in the analysis (integer).
On the other hand, if the first value is `1`, it represents a layer containing PCMs in a solid matrix. The next three inputs are the same as above for the solid matrix. The next input is the solid matrix fraction (must be less than one), followed by two inputs: the lower and upper transition temperatures (in C), respectively. The next four inputs are for the PCMs themselves: the solid and liquid thermal conductivity (W/m K), and the solid and liquid heat capacity (J/kg K). The next two inputs are the density (kg/m3, assumed not to change with phase transition) and the latent heat (J/kg). The last input is the number of sub-layers in the analysis (integer). For example:
```
*LAYERS
2
0 0.15 1.4 880 2330 10
1 0.25 1.4 880 2330 0.7 29 27 0.2 0.13 1500 2153 995 150000 10
```
The first line contains `2`, which indicates that there are two layers. This must then be followed by one line per layer.
The second line is for the first layer. The first input is `0` so this is a layer that does not undergo phase change. It has a thickness of 0.15 m, a thermal conductivity of 1.4 W/mK, a heat capacity of 880 J/kgK, and a density of 2330 kg/m3. The layer subdivided into 10 sub-layers.
The third line is for the second layer. The first input is `1` so this layer contains PCMs. It has a thickness of 0.25 m, and the solid matrix has  thermal conductivity of 1.4 W/mK, a heat capacity of 880 J/kgK, and a density of 2330 kg/m3. The solid matrix forms 0.7 parts by volume of the layer. The PCMs have a lower and upper transition temperature of 27C and 29C, respectively; a solid and liquid thermal conductivity of 0.2 and 0.13 W/mK, respectively; a solid and liquid heat capacity of 1500 and 2153 J/kgK, respectively; a density of 995 kg/m3; a latent heat of 150,000 J/kg; and is subdivided into 10 sub-layers.

**SUBGRADE SECTION**
This section starts with "*SUBGRADE" and contains information related to the subgrade. The subgrade is a layer with a default layer code of `0`, a built-in thickness of 1000 m, and 10 sub-layers. The user only enters three inputs: the thermal conductivity (W/m K), heat capacity (J/kg K), and the density (kg/m3). For example:
```
*SUBGRADE
0.54 347 1450
```
Where the subgrade has a thermal conductivity of 0.54 W/mK, a heat capacity of 347 J/kgK, and a density of 1450 kg/m3.

### Input file format - weather