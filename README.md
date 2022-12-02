# MegRest
<!-- markdown-toc start - Don't edit this section. Run M-x markdown-toc-refresh-toc -->

    - [Installation](#installation)
        - [Prerequisite](#prerequisite)
        - [REST framework](#rest-framework)
        - [MegRest](#megrest-1)

<!-- markdown-toc end -->

## Installation
The intallation guide is targeted at Ubuntu 20.04/22.04 LTS on x86 arch

### Prerequisite
It is basically the ROOT prerequisite, check [this guide](https://root.cern/install/dependencies/).

### REST framework
#### Step 1
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
If you already have these packages installed, make sure they have current version (ROOT > 6.24, Geannt4 > 11.0) and go to [Step 3](#step-3) directly.
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
**NOTE:
MegRest is forced to be installed in the same directory as the REST framework.
This is required for the REST runtime to load MegRest's libraries automatically.**

## Development
The code contribution from developpers should be managed through pull request.
The code quality can then be reviewed before merging.
Developpers need to fork the targeted repository into his/her own account, check [this guide](https://docs.github.com/en/get-started/quickstart/contributing-to-projects).
A new branch should be created to keep the his/her personal modifications before pull request.

Before submitting the pull request, the developper should:
- synchronize his/her local repo with the latest development upstream
- [rebase](https://www.atlassian.com/git/tutorials/rewriting-history/git-rebase) his/her development branch onto the latest development
- [squash](https://www.baeldung.com/ops/git-squash-commits) commit history into one or several commits with meaningful log messages.

These steps are required to have a linear, clear and meaningful log history of the development.

Most development work should be done within the MegRest repository.
Contribution to the framework code follows the same pattern.

### MegRest
The _main_ branch is the target of pull request.
If you follow the above installation guide, you already have a local repo with a _main_ branch tracing  the project's MegRest _main_ branch.
Assuming that you have forked the project's MegRest repo, a typical workflow is:

#### 1. add your personal fork as a new remote in your local clone.
In this example, 'private' is the new remote's name, and url is your fork's address
```
git remote add private https://github.com/youraccount/MegRest
```
#### 2. add a new branch to record your developments
```
git checkout -b cool-feature private/main
```

#### 3. You may make multiple commits to deveplop this cool feature
```
git commit
git push
```

#### 4. prepare the pull requset

Before submitting the pull requst, some preparation is requied.
##### 4.a pull the latest code and rebase onto it
```
git checkout main && git pull
git checkout cool-feature && git rebase main
```
Solve all the conficts during the rebase before proceeding.

##### 4.b squash the development commits
To have a clear and meaningful log history, it's required to clean your commit history by squash.

###### method 1: 
If you don't need your 'cool-feature' branch after your pull request is merged, you can use the following command to squash all commits in
this branch into one.
```
# checkout the branch you wanna merge into
git checkout main 
# squash all commits into the changes
git merge --squash 
# commit these changes and provide meaningful log message for this final commit
git commit -a
```
NOTE: The 'cool-feature' branch is useless after the squash and should be deleted.
The 'main' branch should be used for the pull request in the next step.

###### method 2:
A more suitable and flexible way to re-organize your commit history is interactive rebase:
```
# switch to the branch you wanna create the pull request from
git checkout cool-feature
# rebase onto the branch which is synced with the remote you wanna push your pull request to
git rebase -i main
```

A text terminal will show up, just follow the guides there.
You can shrink your many small trivial commits into several meaningful commits using this method.
Also, the commit message can be edited.

#### 5 create and submit the pull request
Use GitHub website interface to create and submit the pull requset.

### Framework Core

### Framework Submodules
