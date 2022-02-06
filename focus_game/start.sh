#!/bin/bash
export CRPROPS=/home/reighley/Code/misc/focus_game/focus_game.properties
/usr/bin/scala /home/reighley/Code/misc/focus_game/controlserver.jar /home/reighley/Code/misc/focus_game/controlserver.properties &
echo $! > /var/run/focus_game.pid
