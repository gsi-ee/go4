#!/bin/bash

cd /conf



echo "configure container"

# export TRB3_PORT=35
# export DAQOPSERVER=localhost:$TRB3_PORT
# export TRB3_SERVER=192.168.4.240
# export CTS_GUI_PORT=1148
# export CTS_ENDPOINT=0xc035
export DISPLAY=:3 # go4 window will be sent to $DISPLAY, if provide_vnc == yes , then it will be this x11 display


> conf_log.txt


provide_vnc=yes
vnc_password=
vnc_port=5903
vnc_geometry=1500x1024


#   tmux new-session -d -s x11vnc -n x11vnc "x11vnc -forever -create -rfbport 5902 #(nopasswd)"

# set paths for go4/ 
. /installations/go4/601-00/go4login


if [ $provide_vnc == "yes" ]; then
  mkdir -p $HOME/.vnc/
  echo $vnc_password | vncpasswd -f > $HOME/.vnc/passwd
  tmux new-session -d -s vnc -n vnc "echo starting vnc server on port $vnc_port for display $DISPLAY; vncserver $DISPLAY -rfbauth $HOME/.vnc/passwd -rfbport $vnc_port -geometry $vnc_geometry ;/bin/bash"
  
  echo ... >> conf_log.txt
  echo "started vnc server (e.g. for Go4 window)" >> conf_log.txt
  echo port=$vnc_port >> conf_log.txt
  echo DISPLAY=$DISPLAY >> conf_log.txt
  echo password=$vnc_password >> conf_log.txt
  echo >> conf_log.txt
  echo connect with:   vncviewer localhost:$vnc_port >> conf_log.txt
  echo >> conf_log.txt
fi




echo "done"
