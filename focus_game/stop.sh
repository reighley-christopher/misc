#!/bin/bash
PID=`cat /var/run/focus_game.pid`
PGID=`ps --no-header -o pgrp $PID | awk '{print $1}'`
kill -- -$PGID
rm /var/run/focus_game.pid
