# RHICfSTARSim
**RHICf+STAR simulation**

**This framework propagates the STAR simulation data into RHICf and STAR ZDC simulation**

---

RHICfSTARSim is designed to be compatible with StRHICfSimDst format data and STAR's scheduler.

The input file could need StRHICfSimDst format (HepMC format input to be updated)

## Preparing installation
1. ROOT
2. GEANT4 with GDML (version recommended below 10.6)

## Installation
1. Beforing the installation, make sure the ROOT and GEANT4 library 

* Source to geant4
  
```
source /path/to/geant4/install/geant4.csh

```

* Check the ROOT library path

```
${ROOTSYS}
```

    * Note: If you are not working on STAR server system, you should make sure the ROOT library path with CMake option.
    
    ```
        cmake -DROOTPATH=/path/to/ROOT/install
    ```


2. Compile

```
git clone https://github.com/ggfdsa10/RHICfSTARSim.git

cd RHICfSTARSim

mkdir build

cd build

cmake ../

make 

```

## Input options
Input options can be inserted as two types, argument, and text file.

### Argument type

* **-m (Input mode)**

    * **STARSim** is default option 

    * Available mode

        * STARSim : STAR simulation propagating mode (needed StRHICfSimDst input)

        * HepMC : HepMC format event generator file mode (To be updated)

* **-i (Input file)**

    * Path the input file (Only one file acceptable)

* **-r (RHICf run type)**

    * If **STARSim** mode, run-type set using the input file automatically 

    * Available mode

        * TS 
        * TL
        * TOP

* **-g (geometry directory)**

    * Path the simulation detector geometry (GDML format) directory path

    * If not setting, the simulation can find the geometry directory automatically

* **-t (table directory)**

    * Path the simulation correction tables directory path

    * If not setting, simulation can find tables directory automatically

* **-xxx (user customized option)**

    * Format : -OptionName OptionValue 

    * Available type : int, double, bool, string (see the <source/Util/RHICfSimOptions.hh>)

### Text file type

* Input text file extension : .txt, .dat, .par

* Format
   * OptionName = OptionValue
   * comment use to "#"
    
* Necessary options can see the **Argument type** part

## Runs
Now you can run the RHICf+STAR simulation.

* Example
  
```
cd build 

./mainRHICfSTARSim -i /path/to/StRHICfSimDSt.root # default STARSim mode with argument option

./mainRHICfSTARSim ./input.dat # run to specific options with text file option

```
