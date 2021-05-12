--------------------------------------------------------------
         Go4 Release Package v6.1.0 (build 60100)
                      12-May-2021
--------------------------------------------------------------
    The GSI Online Offline Object Oriented (Go4) Project
    Experiment Data Processing at EE department, GSI
--------------------------------------------------------------
Authors: Joern Adamczewski-Musch      (1999-)
         Mohammad Al-Turany           (2000-2004)
         Sven Augustin [Python]	      (2015-)
         Denis Bertini                (2001-2004)
         Hans Georg Essel             (1999-2010)
         Marc Hemberger               (1999-2000)
         Sergey Linev                 (2002-)
---------------------------------------------------------------
Copyright (C) 2000- Gesellschaft f. Schwerionenforschung, GSI
                    Planckstr. 1, 64291 Darmstadt, Germany
Contact:            http://go4.gsi.de
---------------------------------------------------------------
This software can be used under the license agreements as stated
in Go4License.txt file which is part of the distribution.
---------------------------------------------------------------

This package contains a Dockerfile with Go4 6.0 and ROOT 6.18/04 under OpenSuse Leap 15 system.
Go4 is build with Qt5 webcanvas and HDF5 support.
________________________________________________________________________________________________________
Many thanks to Michael Wiebusch (m.wiebusch@gsi.de) for the original docker container and several hints! 
He has created some more elaborate docker files that offer Go4, DABC and trb3 environment. 
Please see

https://jspc29.x-matter.uni-frankfurt.de/git/?p=dockerfiles.git;a=blob_plain;f=go4_trbnet/README.txt

for more information on this.

-----------------------------------------------------------------

Getting started:

##################################################
##                  get Docker                  ##
##################################################



-- step 1 --
install docker with your package manager

(ubuntu/debian)   $ sudo apt-get install docker
(opensuse)        $ sudo zypper install docker

-- step 2 --
add your user to the docker group:
$ sudo usermod -a -G docker <username>

(you might need to log out and back in for the changes
to become effective)

-- step 3 --
start docker daemon:
$ sudo service docker start




##################################################
##             Get Go46 dockerfiles             ##
##################################################

get the Go4V6 Dockerfile tarball at https://go4.gsi.de 
and unpack it:
$ tar zxvf go4v6.1_tumbleweed.tar.gz
$ cd go4v6.1_tumbleweed

In this directory you find:

  - the Dockerfile (go ahead, look inside, it is not scary)

  - two directories "conf/" and "workdir/". Both directories will be
    mounted in the container. Those directories wil NOT be reset
    when the container is stopped.

  - conf/
    contains conf.sh to optionally set environment variables
    and to start (or not start) the vncserver (for Go4 and other graphical applications)
    
  - workdir/ 
    ... this will be your workdir!
     ** This is the place where the Go4 USER ANALYSIS and any external data should be located! ***
    It contains an example start.sh, which starts the initial environment.





##################################################
##             start Go4V6 container            ##
##################################################

Build and run your container:
$ ./build_and_run.sh 

If enabled in conf.sh, a vnc server is started on port 5903 to work with the  Go4 GUI
Connect to it with 
$ vncviewer localhost:3
(Or with any other vncviewer client, also on other host OS!)


Optionally, you may just build the container without starting the vncserver by using
$ ./build_and_shell.sh
You will end up in a plain shell in the docker environment which might be sufficient for batch analysis.

##################################################
##             for Windows 10 users             ##
##################################################

Please install and start DockerDesktop as explained at
	https://docs.docker.com/docker-for-windows/install/
	
		
To run Go4 on Docker Desktop , please use following script in the
Windows command line terminal:  
D:\docker\go4v6.0_leap_15.0> build_and_run.bat


Connection to the VNC server of the go4 container can be done locally by
a Windows VNC viewer, e.g. UltraVNC - https://www.uvnc.com/

Please note 1: 
	Connection may require to set a valid vnc_password by editing file  conf/conf.sh before starting the Go4 container
Please note 2: 
	in the UltraVnc connection dialogue, the servername of the docker machine is not "localhost:5903", 
but just ":5903"



--- JAM(j.adamczewski@gsi.de) 6-Feb-2020

