#!/bin/bash
export CRPROPS=/home/reighley/misc/focus_game/focus_game.properties
/usr/bin/scala /home/reighley/misc/focus_game/controlserver.jar /home/reighley/misc/focus_game/controlserver.properties &
echo $! > /var/run/focus_game.pid
