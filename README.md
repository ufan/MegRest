# MegRest
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
If you follow the above installation guide, you already have a local repo with a local _main_ branch tracing the project's MegRest (i.e. _orgin/main_)
Assuming that you have forked the project's MegRest repo, then a typical workflow is:

#### 1. Add the forked repo as a new remote in your local clone.
Assuming 'private' is selected as the name of the remote tracing youraccount's fork of MegRest repo.
```
# cd to the local repo's directory, then:
git remote add private https://github.com/youraccount/MegRest
```

Now there are two remotes in your local repo:
- _origin_ : pointing to the project's MegRest repo (i.e. https://github.com/megmev/megRest/)
- _private_ : pointing to youraccount's MegRest repo (i.e. https://github.com/youraccount/MegRest)

_origin_ is used to keep updated with latest developments from others and _private_ is used to keep your own development before pull request.

#### 2. Create a new branch to record your developments
For example, 'cool-feature' is the name of the new branch
```
# make sure the code is updated
git switch main && git pull
# then, create the branch based on main branch
git checkout -b cool-feature
# setup its push remote to pravite/cool-feature
git push -u private cool-feature
```
NOTE: You may also create the new branch based on _private/main_. In this case, an extra step is needed to synchronize _private/main_ with _origin/main_
either through GitHub's website interface or in command line. See [this guide](https://docs.github.com/en/pull-requests/collaborating-with-pull-requests/working-with-forks/syncing-a-fork).

#### 3. Make changes and keep them as commits
You are now free to modify the code base as your wish (commit, push, new branch, merge). The following commands may be useful in your development.
```
git commit
git push
git checkout -b sub-cool-feature cool-feature
git switch cool-feature
git merge sub-cool-feature
git log
git diff
git restore
```

Keep in mind that all the local modificatioins should be pushed to _private_, not _origin_.
The _origin_ repo is configured to accept pull requst from normal developpers.
Only developpers who have been granted the privilege can push directly to _origin_, normally these developpers are the administrators of the repo.

#### 4. Prepare the pull requset
Ultimately, you are happy with your new feature and ready to publish it.
Before submitting the pull requst, some preparation is requied.
Most importantly, a thorough test should be done in your local machine so that you are confident that the new code functions as expected.

Then, extra clean work is needed:
##### 4.a Sync with latest updates from other developpers
At the same time as you develop the cool feature, other developpers are also working and may already publish their work before you.
It's important to sync with these latest updates to make sure your modification is compatible with them.
```
# sync with upstream repo
git switch main && git pull
# rebase your work upon there new development
git switch cool-feature && git rebase origin/main
```
Most of time, you are working on different parts of the code tree from other developpers, the rebase shall be smooth.
If not, there may be conflicts.
Then, solve all these conficts before proceeding.
You may need to communicate with other developpers to understand their modifications to solve these conflicts.

##### 4.b Clean development history by squash commits
It's not unusual to make many small and trivial commits during the development, e.g. backup, quick hot-fix etc.
These commits are meaningless to other developpers and end-users.
To have a clear and meaningful development history, it's required to clean your local commit history by squash.

```
# switch to the branch you wanna create the pull request from
git switch cool-feature
# rebase onto main branch again, but this time interactiveli
git rebase -i main
```

Interactive rebase allows you to squash many trivial commits into several meaningful commits, modify the log message and much more.
Basically, you can re-organize the commit history. There are guides on the terminal showing all behaviors available.

_rebase_ is a history modification manueover. After rebase, the local branch and the remote tracing branch (e.g. _cool-feature_ and _private/cool-feature_)
may diverge. A force push is needed to keep them coverged (this is mandatory):
```
git push --force private cool-feature
```

#### 5. Create and submit the pull request
Now, you can create the pull request using GitHub website interface.
The _main_ branch of the project account's MegRest repo should be selected as the base repo of the pull request.
The _cool-feature_ branch of youraccount's MegRest repo should be selected as the head repo of the pull request.

### Framework Core
### Framework Submodules
