#!/bin/sh
rm /tmp/gazebo_all.info
rm /tmp/gazebo.info
rm -rf /tmp/coverage

lcov -d build/src -c -o /tmp/gazebo_all.info
lcov -r /tmp/gazebo_all.info "/usr/*" -o /tmp/gazebo.info
genhtml  /tmp/gazebo.info -o /tmp/coverage
scp -r /tmp/coverage root@gazebosim.org:/www/live/htdocs
