# MegRest
## Installation
The intallation guide is targeted at Ubuntu 20.04/22.04 LTS on x86 arch

### Prerequisite
It is basically the ROOT prerequisite, check [this guide](https://root.cern/install/dependencies/).

### REST framework
#### Step 1: 
Clone the code.
```bash
# 1. clone and cd to the top directory
git clone https://github.com/MegMev/rest-framework && cd rest-framework
# 2. cd to the code tree
cd rest-framework
# 3. checkout the develop branch
git checkout develop
# 4. pull the submodules
python3 pull-submodules.py --clean --dontask
```

#### Step 2
Install ROOT, Geant4 and Garfield using provided scripts.
If you already have these packages installed, make sure they have current version (ROOT > 6.24, Geant4 > 11.0) and go to [Step 3](#step-3) directly.
```bash
cd scripts/installation
# 1. install ROOT first
./intallROOT.sh
source ~/.bashrc
# 2. install Geant4
./installGeant4.sh
source ~/.bashrc
# 3. install Garfield
./installGarfield.sh
source ~/.bashrc
```
These scripts will install the packages under `$HOME/app` and append some commands into your `.bashrc` or `.zshrc` to
set up the running environment automatically.

#### Step 3
Build and install the framework.
Make sure the running environment of ROOT, Geant4 and Garfield has been configured.
```bash
# go back to the top dircetory of code tree
cd $YOUR_REST_REPO
# 1. make a build directory and cd to it
mkdir build && cd build
# 2. generate Makefile
cmake -DCMAKE_INSTALL_PREFIX=$YOUR_INSTALL_DIR ..
# 3. build and install
make -j 6 install
```
### MegRest
```bash
# 1. clone and cd to the top directory
git clone https://github.com/MegMev/MegRest && cd MegRest
# 2. make a build directory and cd to it
mkdir build && cd build
# 3. build and install
make -j 6 install
```
