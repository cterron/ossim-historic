#!/bin/sh
procpid=`ps -eo "%c%p" | grep -w -e "$1\(_eth[0-9]\)" -e "$1" | awk '{print $2}' | tr '\n' ' '`
kill -9 $procpid