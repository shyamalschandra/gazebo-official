#!/bin/sh

# Jenkins will pass -xml, in which case we want to generate XML output
xmlout=0
if test "$1" = "-xmldir" -a -n "$2"; then
  xmlout=1
  xmldir=$2
  mkdir -p $xmldir
  rm -rf $xmldir/*.xml
  # Assuming that Jenkins called, the `build` directory is sibling to src dir
  builddir=../build
else
  # This is a heuristic guess; not everyone puts the `build` dir in the src dir
  builddir=./build
fi

SUPPRESS=/tmp/gazebo_cpp_check.suppress
echo "*:gazebo/sdf/interface/parser.cc:544" > $SUPPRESS
echo "*:gazebo/common/STLLoader.cc:94" >> $SUPPRESS
echo "*:gazebo/common/STLLoader.cc:105" >> $SUPPRESS
echo "*:gazebo/common/STLLoader.cc:126" >> $SUPPRESS
echo "*:gazebo/common/STLLoader.cc:149" >> $SUPPRESS
echo "*:gazebo/common/Plugin.hh:145" >> $SUPPRESS
echo "*:gazebo/common/Plugin.hh:118" >> $SUPPRESS
echo "*:examples/plugins/custom_messages/custom_messages.cc:22" >> $SUPPRESS
# Not defined FREEIMAGE_COLORORDER
echo "*:gazebo/common/Image.cc:1" >> $SUPPRESS

#cppcheck
CPPCHECK_BASE="cppcheck -q --suppressions-list=$SUPPRESS"
CPPCHECK_FILES=`\
  find ./plugins ./gazebo ./tools ./examples ./test/integration ./interfaces \
    -name "*.cc"`
CPPCHECK_INCLUDES="-I gazebo/rendering/skyx/include -I . -I $builddir"\
" -I $builddir/gazebo/msgs -I deps -I deps/opende/include -I test"
CPPCHECK_RULES="--rule-file=./tools/cppcheck_rules/issue_581.rule"
CPPCHECK_CMD1="-j 4 --enable=style,performance,portability,information"\
" $CPPCHECK_RULES $CPPCHECK_FILES"
# This command used to be part of the script but was removed since our API
# provides many functions that Gazebo does not use internally
CPPCHECK_CMD2="--enable=unusedFunction $CPPCHECK_FILES"
CPPCHECK_CMD3="-j 4 --enable=missingInclude $CPPCHECK_FILES"\
" $CPPCHECK_INCLUDES --check-config"
if [ $xmlout -eq 1 ]; then
  # Performance, style, portability, and information
  ($CPPCHECK_BASE --xml $CPPCHECK_CMD1) 2> $xmldir/cppcheck.xml

  # Check the configuration
  ($CPPCHECK_BASE --xml $CPPCHECK_CMD3) 2> $xmldir/cppcheck-configuration.xml
else
  # Performance, style, portability, and information
  $CPPCHECK_BASE $CPPCHECK_CMD1 2>&1

  # Check the configuration
  $CPPCHECK_BASE $CPPCHECK_CMD3 2>&1
fi

# cpplint
CPPLINT_DIRS=\
"./gazebo ./tools ./plugins ./examples ./test/integration ./interfaces"
CPPLINT_FILES=`\
  find $CPPLINT_DIRS -name "*.cc" -o -name "*.hh" -o -name "*.c" -o -name "*.h"`
if [ $xmlout -eq 1 ]; then
  (echo $CPPLINT_FILES | xargs python tools/cpplint.py 2>&1) \
    | python tools/cpplint_to_cppcheckxml.py 2> $xmldir/cpplint.xml
else
  echo $CPPLINT_FILES | xargs python tools/cpplint.py 2>&1
fi

# msg_check.py
if [ $xmlout -eq 1 ]; then
  ./tools/msg_check.py xml 2> $xmldir/msg_check.xml
else
  ./tools/msg_check.py 2>&1
fi
