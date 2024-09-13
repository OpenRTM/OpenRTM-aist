#!/bin/sh

rtm_config_test()
{
echo "------------------------------------------------------------"
echo "cmd:" $*
echo "------------------------------------------------------------"
echo "Prefix        : " `$cmd --prefix`
echo "Exec Prefix   : " `$cmd --exec-prefix`
echo "Data Prefix   : " `$cmd --data-prefix`
echo "Version       : " `$cmd --version`
echo "CXX           : " `$cmd --cxx`
echo "Cflags        : " `$cmd --cflags`
echo "Cflags -I     : " `$cmd --cflags-only-I`
echo "Cflags other  : " `$cmd --cflags-only-other`
echo "Libs          : " `$cmd --libs`
echo "Libs -l       : " `$cmd --libs-only-l`
echo "Libs -L       : " `$cmd --libs-only-L`
echo "Libs other    : " `$cmd --libs-only-other`
echo "Lib Dir       : " `$cmd --libdir`
echo "ORB           : " `$cmd --orb`
echo "IDLC          : " `$cmd --idlc`
echo "IDLFLAGS      : " `$cmd --idlflags`
echo "IDLFLAGS -I   : " `$cmd --idlflags-only-I`
echo "IDLFLAGS other: " `$cmd --idlflags-only-other`
echo "RTM inc dir   : " `$cmd --rtm-includedir`
echo "RTM IDL dir   : " `$cmd --rtm-idldir`
echo "RTM lib dir   : " `$cmd --rtm-libdir`
echo "RTM data dir  : " `$cmd --rtm-datadir`
echo "RTM rtcd dir  : " `$cmd --rtm-rtcdir`
echo "RTM EC dir    : " `$cmd --rtm-ecdir`
echo "RTM rtm dir   : " `$cmd --rtm-rtmdir`
echo "RTM svc dir   : " `$cmd --rtm-svcdir`
echo "RTM doc dir   : " `$cmd --rtm-docdir`
echo "RTM exp dir   : " `$cmd --rtm-exampledir`
echo "coil inc dir  : " `$cmd --coil-includedir`
}

cmd="./rtm2-config"
rtm_config_test $cmd
cmd="./rtm2-config --prefix=/opt"
rtm_config_test $cmd
cmd="./rtm2-config --prefix=/opt --exec-prefix=/Applications"
rtm_config_test $cmd
cmd="./rtm2-config --prefix=/opt --exec-prefix=/Applications --data-prefix=/var"
rtm_config_test $cmd

echo "------------------------------------------------------------"
