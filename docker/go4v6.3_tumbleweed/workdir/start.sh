#!/bin/bash

echo "container started"


echo "run /conf/conf.sh"
. /conf/conf.sh # keep environment variables set in conf.sh
tail -n 50 /conf/conf_log.txt
cd /workdir

# open some applications on vnc server:
/usr/bin/startlxqt &
sleep 5
xterm &

echo "drop user to shell"
/bin/bash
echo "terminate container"
