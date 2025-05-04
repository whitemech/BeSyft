# MBeSyft

MBeSyft is a tool for symbolic minimal best-effort synthesis in FOND domains.

# Usage

The output of `syft4fond --help` is the following:

```
syft4fond-ltlf: a tool for LTLf reactive synthesis in FOND planning domains
Usage: ./syft4fond [OPTIONS]

Options:
  -h,--help                   Print this help message and exit
  -d,--domain-file TEXT:FILE REQUIRED
                              Path to PDDL domain file
  -p,--problem-file TEXT:FILE REQUIRED
                              Path to PDDL problem file
  -g,--goal-file TEXT:FILE REQUIRED
                              Path to LTLf goal file
  -i,--interactive BOOLEAN    Executes the synthesized strategy in interactive mode
  -o,--out-file TEXT          Path to output .csv file. Stores:
                              1. PDDL domain file
                              2. PDDL problem file
                              3. PDDL parsing (secs)
                              4. PDDL2DFA (secs)
                              5. Synthesis (secs)
                              6. Run time (secs)
  -s,--synthesizer INT REQUIRED  Specifies type of synthesis to use:
                              Best-Effort Synthesis=1;
                              Minimal Best-Effort Synthesis=2

```

LTLf formulas in agent and environment files should be written in Lydia's syntax. For further details, refer to https://github.com/whitemech/lydia . 

To perform mbe-synthesis for an LTLf goal in some FOND planning domain, you have to provide the path to the domain specification, problem specification, and the agent goal e.g., `Examples/domain-triangle.pddl`, `Examples/test.pddl`, and `Examples/test.ltlf`.

For instance, the command:

```
./syft4fond -d Examples/domain-triangle.pddl -p Examples/test.pddl -g Examples/test.ltlf -s 2 -i 0
```

Performs mbe-synthesis using the symbolic-compositional algorithm.

# Build from source

Compilation instruction using CMake (https://cmake.org/). We recommend the use of Ubuntu 20.04 LTS. Problems can occur between some libraries on which MBeSyft relies and newer versions of Ubuntu (more information below).

## Install the dependencies

### Flex and Bison

The project uses Flex and Bison for parsing purposes.

First check that you have them: `whereis flex bison`


If no item occurs, then you have to install them: `sudo apt-get install -f flex bison`

### CUDD 3.0.0

The project depends on CUDD 3.0.0. To install it, run the following commands

```
wget https://github.com/whitemech/cudd/releases/download/v3.0.0/cudd_3.0.0_linux-amd64.tar.gz
tar -xf cudd_3.0.0_linux-amd64.tar.gz
cd cudd_3.0.0_linux-amd64
sudo cp -P lib/* /usr/local/lib/
sudo cp -Pr include/* /usr/local/include/
```

Otherwise, build from source (customize `PREFIX` variable as you see fit).

```
git clone https://github.com/whitemech/cudd && cd cudd
PREFIX="/usr/local"
./configure --enable-silent-rules --enable-obj --enable-dddmp --prefix=$PREFIX
sudo make install
```

If you get an error about aclocal, this might be due to either

* Not having automake: `sudo apt-get install automake`
* Needing to reconfigure, do this before `configuring: autoreconf -i`
* Using a version of aclocal other than 1.14: modify the version 1.14 in configure accordingly.

### MONA

The projects depends on the MONA library, version v1.4 (patch 19). We require that the library is compiled with different values for parameters such as `MAX_VARIABLES`, and `BDD_MAX_TOTAL_TABLE_SIZE` (you can have a look at the details at https://github.com/whitemech/MONA/releases/tag/v1.4-19.dev0).

To install the MONA library, run the following commands:

```
wget https://github.com/whitemech/MONA/releases/download/v1.4-19.dev0/mona_1.4-19.dev0_linux-amd64.tar.gz
tar -xf mona_1.4-19.dev0_linux-amd64.tar.gz
cd mona_1.4-19.dev0_linux-amd64
sudo cp -P lib/* /usr/local/lib/
sudo cp -Pr include/* /usr/local/include
```

### SPOT

The project relies on SPOT (https://spot.lre.epita.fr/). To install it, follows the instructions at https://spot.lre.epita.fr/install.html

### Graphviz

The project uses Graphviz to display automata and strategies. Follow the install instructions on the official website: https://graphviz.gitlab.io/download/.

On Ubuntu, this should work:

```
sudo apt-get install libgraphviz-dev
```

### Syft

MBeSyft depends on Syft. First, install the Boost libraries.

```
sudo apt-get install libboost-dev-all
```

For further information see https://www.boost.org/ 

Install Syft with

```
git clone https://github.com/whitemech/Syft.git
cd Syft
git checkout v0.1.1
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j
sudo make install
```

### Lydia:

Unzip the repository and move into it `cd MBeSyft`

Clone Lydia within the submodules folder.

```
mkdir submodules
cd submodules 
git clone https://github.com/whitemech/lydia.git --recursive
```


NOTE: Users of Ubuntu 22.04 LTS might encounter compilation errors due to incompatibilities Lydia's Catch2 library and newer versions of Ubuntu. To address it, take the following steps

I. Go to https://github.com/catchorg/Catch2/tree/v2.x and get the .zip file.

II. unzip the file.

III. substitute in repository `submodules/lydia/third_party/Catch2` with the unzipped folder of Catch2.

IV. delete any CMakeCache.txt file which may have been generated by previous compilation processes.

### Building MBeSyft

To build, run the following commands.

```
cd ..
mkdir build && cd build
cmake ..
make -j2
```

## Performing the Experiments

To plot the results of the experiments on a FOND domain (i.e. COFFEE) execute:

```
cd EmpiricalResults/COFFEE
python plot.py
```
