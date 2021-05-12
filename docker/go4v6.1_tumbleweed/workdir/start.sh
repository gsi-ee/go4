#!/bin/bash

echo "container started"

#echo "create main tmux session"

echo "run /conf/conf.sh"
. /conf/conf.sh # keep environment variables set in conf.sh
tail -n 50 /conf/conf_log.txt
cd /workdir

# open some applications on vnc server:
sleep 5
#lxpanel & 
xterm &

echo "drop user to shell"
/bin/bash
echo "terminate container"
