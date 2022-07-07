# Marconi-Rosenblatt Framework for Intelligent Networks (MR-iNet Gym) For Rapid Design and Implementation of Distributed Multi-agent Reinforcement Learning Solutions for Wireless Networks
### Software encompassing a CDMA ns-3 module, ns3-gym scenario, and reinforcemnet learning framework

## CDMA Module
The CDMA module is in the `cdmanew` folder.

To use the CDMA module, copy `cdamnew` into the `src` directory of ns-3.

## Example Scenario
The folder `example_scenario` contains a simulation using our the CDMA module, ns3-gym, and our RL framework. Put the `example_scenario` folder into the `scratch` directory of ns-3.

## RL Framework
The RL framework is in the `rl` folder. Download this code locally so you can use it in the `agent.py` function for your simulations.

## Instructions for Use 
    * In one terminal, in the ns3 home directory, run `./waf --run example_scenario`. 
    * In another terminal, in the directory `(ns3 home directory)/scratch/example_scenario`, run `./agent.py`, or however you wish to run the python file.  
    * This will create a `data.csv` file in `example_scenario` folder
    * You can move this `data.csv` file into a new directory, e.g. `example_scenario/run2`, and copy the `example_scenario/run1/Plots.ipynb` Jupyter Notebook file into your new folder. Running this file will generae plots for the data.
    
## Software Versions
* ns-3 version 3.31
* TensorFlow version 2.2
* ns3-gym (https://github.com/tkn-tub/ns3-gym) from commit on 5/28/20

## Installation Instructions (Ubuntu)
**Installing ns-3 (This is the hard part, but copying and pasting these commands should help!). If you run into and issues check the installation documenation provided by ns-3 (https://www.nsnam.org/wiki/Installation).**

If on a new OS, update OS:

	sudo apt update && sudo apt upgrade

Dependencies:

	sudo apt-get install gcc g++ python python3 python3-dev
	sudo apt-get install python3-setuptools git mercurial
	sudo pip install --upgrade pip

Install all ns3 dependencies (everything at https://www.nsnam.org/wiki/Installation):

	sudo apt-get install qt5-default mercurial
	sudo apt-get install gir1.2-goocanvas-2.0 python-gi python-gi-cairo python-pygraphviz python3-gi python3-gi-cairo python3-pygraphviz gir1.2-gtk-3.0 ipython ipython3  	
	sudo apt-get install openmpi-bin openmpi-common openmpi-doc libopenmpi-dev
	sudo apt-get install autoconf cvs bzr unrar	
	sudo apt-get install gdb valgrind
	sudo apt-get install uncrustify 
	sudo apt-get install doxygen graphviz imagemagick  
	sudo apt-get install texlive texlive-extra-utils texlive-latex-extra texlive-font-utils texlive-lang-portuguese dvipng latexmk
	sudo apt-get install python3-sphinx dia 
	sudo apt-get install gsl-bin libgsl-dev libgsl23 libgslcblas0 
	sudo apt-get install tcpdump 
	sudo apt-get install sqlite sqlite3 libsqlite3-dev
	sudo apt-get install libxml2 libxml2-dev
	sudo apt-get install cmake libc6-dev libc6-dev-i386 libclang-6.0-dev llvm-6.0-dev automake python-pip
	python3 -m pip install --user cxxfilt 
	sudo apt-get install libgtk2.0-0 libgtk2.0-dev
	sudo apt-get install vtun lxc uml-utilities
	sudo apt-get install libboost-signals-dev libboost-filesystem-dev 

Install using bake:

	git clone https://gitlab.com/nsnam/bake 

Add as environment variables:

	export BAKE_HOME=`pwd`/bake 
	export PATH=$PATH:$BAKE_HOME
	export PYTHONPATH=$PYTHONPATH:$BAKE_HOME	
	
	bake.py check
	bake.py configure -e ns-allinone-3.31
	bake.py show   
	bake.py deploy

You should get this message after build:

"
Modules built:
antenna                   aodv                      applications              
bridge                    buildings                 config-store              
core                      csma                      csma-layout               
dsdv                      dsr                       energy                    
fd-net-device             flow-monitor              internet                  
internet-apps             lr-wpan                   lte                       
mesh                      mobility                  mpi                       
netanim (no Python)       network                   nix-vector-routing        
olsr                      opengym (no Python)       point-to-point            
point-to-point-layout     propagation               sixlowpan                 
spectrum                  stats                     tap-bridge                
test (no Python)          topology-read             traffic-control           
uan                       virtual-net-device        wave                      
wifi                      wimax                     

Modules not built (see ns-3 tutorial for explanation):
brite                     click                     openflow                  
visualizer "

**Option 1: Adding the ns3-gym module to an existing ns-3 install (recommended)**
ns3-gym: https://github.com/tkn-tub/ns3-gym (we recommend using the code from the commit on 5/28/20 if you get errors regarding ZMQ.)

From the NS3-Gym repository download the “opengym” folder (https://github.com/tkn-tub/ns3-gym/tree/master/src/opengym).

Move the opengym folder into the `src/` directory in your NS3 repository

Install the `setup.py` inside `opengym/model/ns3gym` gym, assuming you’re in that directory, run:

	pip install ns3gym

Note: for any new module that you add to NS3 (include NS3-Gym) the procudure is:

	Add the module to the src/ folder of NS3
	./waf clean
	./waf configure --enable-tests
	./waf


**Option 2: Clone the ns3-gym repository which comes with all of the ns-3 code as well.**

Install ns3-gym dependencies:

	sudo add-apt-repository ppa:maarten-fonville/protobuf
	sudo apt-get update

	apt-get install libzmq5 libzmq5-dev
	apt-get install libprotobuf-dev
	apt-get install protobuf-compiler

Clone ns3-gym. May have to cd out of bake and back into your main workspace:
 
	git clone https://github.com/tkn-tub/ns3-gym.git
	cd ns3-gym
	./waf configure --enable-tests
	./waf build

*Note: if you run into errors related to ZMQ when configuring or building, try to clone the ns3-gym release from May 28th, 2019, with the commit message "Update opengym-test-suite.cc".*

Install ns3gym:

	pip3 install ./src/opengym/model/ns3gym

Install tensorflow (optional):

	pip install --upgrade tensorflow

Run example from within ns3-gym directory (need TensorFlow):

	cd ./scratch/opengym
	./simple_test.py

## How to cite this dataset ? Please cite the original paper.
* **BibTeX** 
> >  @article{MRiNetcomnet2022, title={{Marconi-Rosenblatt Framework for Intelligent Networks (MR-iNet Gym): For Rapid Design and Implementation of Distributed Multi-agent Reinforcement Learning Solutions for Wireless Networks}}, author={ Farquhar, Collin and Kafle, Swatantra and Hamedani, Kian and Jagannath, Anu}, journal = {Computer Networks (Under Review)}, month = {June}, year = {2022} } </br >

> >  @inproceedings{JagannathWiseML22_MRiNet,
    author ={Jithin Jagannath, Kian Hamedani, Collin Farquhar, Keyvan Ramezanpour, Anu Jagannath},
    title ={{MR-iNet Gym: Framework for Edge Deployment of Deep Reinforcement Learning on Embedded Software Defined Radio}},
    booktitle ={Proc. of ACM Workshop on Wireless Security and Machine Learning (WiseML)},
    address ={San Antonio, Texas, USA},
    month ={May},
    year ={2022}}
* **Plain Text** 
> > C. Farquhar, S. Kafle; K. Hamedani; A. Jagannath. "Marconi-Rosenblatt Framework for Intelligent Networks (MR-iNet Gym): For Rapid Design and Implementation of Distributed Multi-agent Reinforcement Learning Solutions for Wireless Networks", Computer Networks (Under Review).</br >

> > J. Jagannath, K. Hamedani, C. Farquhar, K. Ramezanpour, A. Jagannath, "MR-iNet Gym: Framework for Edge Deployment of Deep Reinforcement Learning on Embedded Software Defined Radio", in Proc. of ACM Workshop on Wireless Security and Machine Learning (WiseML), San Antonio, Texas, USA May, 2022. [Best Paper Award].
