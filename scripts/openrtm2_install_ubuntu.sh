#!/bin/sh
#
# @file openrtm2_install_ubuntu.sh
# @brief OpenRTM-aist dependent packages install script for Ubuntu
# @author Noriaki Ando <n-ando@aist.go.jp>
#         Nobu   Kawauchi
#
# Global variables
# 
# - OPT_RUNTIME  : installing runtime packages
# - OPT_DEVEL    : installing developers' staff
# - OPT_SRCPKG   : installing tools for make source package
# - OPT_COREDEVEL: installing tools for core developers
# = OPT_UNINST   : uninstallation
#

VERSION=2.0.2.04
FILENAME=openrtm2_install_ubuntu.sh

#
#---------------------------------------
# usage
#---------------------------------------
op_r_msg="install only runtime libraries and environment for RTC"
op_d_msg="install packages for RTC developer"
op_s_msg="install tool_packages for building OpenRTM from source"
op_c_msg="install tool_packages for OpenRTM core developer"
usage()
{
  cat <<EOF
  Usage: 

    ${FILENAME} -l {all|c++} [-r|-d|-s|-c] [-e ros|ros2|all] [--ros|--ros2] [-u|--yes]
    ${FILENAME} [-u]
    ${FILENAME} -l {python} [-r|-d|-c] [-u|--yes]
    ${FILENAME} -l {java} [-r|-d|-c] [-u|--yes]
    ${FILENAME} -l {openrtp|rtshell} [-d] [-u|--yes]
    ${FILENAME} {--help|-h|--version}

  Example:
    ${FILENAME} [= ${FILENAME} -l all -d]
    ${FILENAME} -l all -d
    ${FILENAME} -l c++ -c --yes
    ${FILENAME} -l all -u
    ${FILENAME} -l c++ -d -e ros
    ${FILENAME} -l c++ -d --ros2

  Options:
    -l <argument>  language or tool [c++|python|java|openrtp|rtshell|all]
	all        install packages of all the supported languages and tools
                   (openrtp is not supported in aarch64 environment.)
    -r             ${op_r_msg}
    -d             ${op_d_msg} [default]
    -e <argument>  install extension packages [ros|ros2|all]
    --ros          install extension package for ROS
    --ros2         install extension package for ROS2
    -s             ${op_s_msg}
    -c             ${op_c_msg}
    -u             uninstall packages
    --yes          force yes
    --help, -h     print this
    --version      print version number
EOF
}

version()
{
  echo ${VERSION}
}


#---------------------------------------
# パッケージリスト
#---------------------------------------
ace="libace libace-dev"
openrtm04="openrtm-aist=0.4.2-1 openrtm-aist-doc=0.4.2-1 openrtm-aist-dev=0.4.2-1 openrtm-aist-example=0.4.2-1 python-yaml"

default_reposerver="openrtm.org"
reposervers="openrtm.org"
reposerver=""

openrtm_py="openrtm-aist-python3-doc openrtm-aist-python3-example openrtm-aist-python3"
openrtm_java="openrtm-aist-java-doc openrtm-aist-java-example openrtm-aist-java"

ARCH=`arch`
#--------------------------------------- C++
autotools="autoconf libtool libtool-bin"
base_tools="bc iputils-ping net-tools"
common_devel="python3-yaml"
cxx_devel="gcc g++ make $common_devel"
cmake_tools="cmake doxygen graphviz nkf"
build_tools="subversion git"
deb_pkg="uuid-dev libboost-filesystem-dev"
pkg_tools="build-essential debhelper devscripts"
fluentbit19="td-agent-bit"
fluentbit="fluent-bit"
omni_devel="libomniorb4-dev omniidl"
omni_runtime="omniorb-nameserver"
openrtm2_devel="openrtm2-doc openrtm2-idl openrtm2-dev"
openrtm2_runtime="openrtm2 openrtm2-naming openrtm2-example"
openrtm2_ros="openrtm2-ros-tp"
openrtm2_ros2="openrtm2-ros2-tp"

#--------------------------------------- Python
omnipy="omniidl-python3"
python_runtime="python3 python3-omniorb-omg python3-tk tix-dev"
python_devel="python3-pip $cmake_tools $base_tools $omnipy $common_devel"
openrtm2_py_devel="openrtm2-python3-doc"
openrtm2_py_runtime="openrtm2-python3 openrtm2-python3-example"

#--------------------------------------- Java
java_build="ant"
openrtm2_j_devel="openrtm2-java-doc"
openrtm2_j_runtime="openrtm2-java openrtm2-java-example"

#--------------------------------------- OpenRTP
openrtp_pkgs="openrtp2"

#---------------------------------------
# Script options, argument analysis
#---------------------------------------
init_param()
{
OPT_RUNTIME=false
OPT_DEVEL=false
OPT_SRCPKG=false
OPT_COREDEVEL=false
OPT_UNINST=true
OPT_OLD_RTM=false
OPT_ROS=false
OPT_ROS2=false
install_pkgs=""
uninstall_pkgs=""
arg_all=false
arg_cxx=false
arg_python=false
arg_java=false
arg_openrtp=false
arg_rtshell=false
err_message=""
rts_msg=""
select_opt_c=""
}


check_arg()
{
  local arg=$1 tmp
  arg_err=0
  
  case "$arg" in
    all ) arg_all=true ;;
    c++ ) arg_cxx=true ;;
    python ) arg_python=true ;;
    java ) arg_java=true ;;
    openrtp ) arg_openrtp=true
              if test "x${ARCH}" = "xaarch64"; then
                arg_openrtp=false
                msg="[WARNING] openrtp is not supported in aarch64 environment."
                echo $msg
                tmp="$err_message$LF$msg"
                err_message=$tmp
              fi
	      shift ;;
    rtshell ) arg_rtshell=true ;;
    *) arg_err=-1 ;;
  esac
}

check_ros_arg()
{
  local arg=$1
  arg_err=0

  case "$arg" in
    all ) OPT_ROS2=true
          res=`grep 18.04 /etc/lsb-release`
          res1=`grep 20.04 /etc/lsb-release`
          if test ! "x$res" = "x" ||
             test ! "x$res1" = "x" ; then
            OPT_ROS=true
          fi
          shift ;;
    ros ) OPT_ROS=true ;;
    ros2 ) OPT_ROS2=true ;;
    *) arg_err=-1 ;;
  esac
}

set_old_rtm_pkgs()
{
  local ver=$1 LF='\n'
  local msg tmp
  arg_err=""

  if test "x$ver" = "x$RTM_OLD_VER" ; then
    OPT_OLD_RTM=true
    if test "x$arg_cxx" = "xtrue" ||
       test "x$arg_all" = "xtrue" ; then
      openrtm_devel=$old_openrtm_devel
      openrtm_runtime=$old_openrtm_runtime
      arg_cxx=true
    fi
    if test "x$arg_python" = "xtrue" ||
       test "x$arg_all" = "xtrue"    ; then
      omnipy="omniidl-python"
      python_runtime="python python-omniorb-omg"
      python_devel="python-pip $cmake_tools $base_tools $omnipy"
      openrtm_py_devel=$old_openrtm_py_devel
      openrtm_py_runtime=$old_openrtm_py_runtime
      arg_python=true
    fi
    if test "x$arg_java" = "xtrue" ||
       test "x$arg_all" = "xtrue" ; then
      msg="[ERROR] Installation of older version of OpenRTM-aist-Java is not supported."
      echo $msg
      tmp="$err_message$LF$msg"
      err_message=$tmp
      arg_java=false
    fi
    if test "x$arg_openrtp" = "xtrue" ||
       test "x$arg_all" = "xtrue" ; then
      msg="[ERROR] Installation of older version of OpenRTP-aist is not supported."
      echo $msg
      tmp="$err_message$LF$msg"
      err_message=$tmp
      arg_openrtp=false
    fi
    if test "x$arg_rtshell" = "xtrue" ||
       test "x$arg_all" = "xtrue" ; then
      msg="[ERROR] Installation of older version of rtshell is not supported."
      echo $msg
      tmp="$err_message$LF$msg"
      err_message=$tmp
      arg_rtshell=false
    fi
  else
    arg_err=$ver
  fi
}

get_opt()
{ 
  # オプション指定が無い場合のデフォルト設定
  if [ $# -eq 0 ] ; then
    arg_all=true
    OPT_DEVEL=true
  fi
  arg_num=$#
 
  OPT=`getopt -o l:rcsdte:hu -l help,yes,version,ros,ros2 -- $@` > /dev/null 2>&1
  # return code check
  if [ $? -ne 0 ] ; then
    echo "[ERROR] Invalid option '$1'"
    usage
    exit
  fi
  # 引数1個の場合
  if [ $arg_num -eq 1 ] ; then
    # オプション指定が -r/-s/-c のみの場合
    if test "x$1" = "x-r" ||
       test "x$1" = "x-s" ||
       test "x$1" = "x-c" ; then
      echo "[ERROR] Invalid option '$1'. '-l' option is required."
      usage
      exit
    fi
    if test "x$1" = "x--yes" ; then
      arg_all=true
      OPT_DEVEL=true
    fi
  fi
  eval set -- $OPT

  while true
  do
    case "$1" in
        -h|--help ) usage ; exit ;;
        --version ) version ; exit ;;
        --yes ) FORCE_YES=true ;;
        --ros ) OPT_ROS=true ;;
        --ros2 ) OPT_ROS2=true ;;
        -l )  if [ -z "$2" ] ; then
                echo "$1 option requires an argument." 1>&2
                exit
              fi
              check_arg $2
              if [ "$arg_err" = "-1" ]; then
                echo "[ERROR] Invalid argument '$2'"
                usage
                exit
              fi
              shift ;;
        -e )  if [ -z "$2" ] ; then
                echo "$1 option requires an argument." 1>&2
                exit
              fi
              check_ros_arg $2
              if [ "$arg_err" = "-1" ]; then
                echo "[ERROR] Invalid argument '$2'"
                usage
                exit
              fi
              shift ;;
        -t )  if [ -z "$2" ] ; then
                echo "$1 option requires an argument." 1>&2
                exit
              fi
              set_old_rtm_pkgs $2
              if test ! "x$arg_err" = "x" ; then
                echo "[ERROR] Invalid argument '$2'. Only $RTM_OLD_VER supported. "
                exit
              fi
              shift ;;
        -r )  OPT_RUNTIME=true ;;
        -d )  OPT_DEVEL=true ;;
        -s )  OPT_SRCPKG=true ;;
        -c )  OPT_COREDEVEL=true ;;
        -u )  OPT_UNINST=false ;;
        -- ) shift ; break ;;                 
        * )
          echo "Internal Error" 1>&2
          exit ;;
    esac
    shift
  done

  # オプション指定が -u のみの場合
  if [ $arg_num -eq 1 ] ; then
    if test "x$OPT_UNINST" = "xfalse" ; then 
      arg_all=true
    fi
  fi
}

#---------------------------------------
# ロケールの言語確認
#---------------------------------------
check_lang()
{
lang="en"

locale | grep ja_JP > /dev/null && lang="jp"

if test "$lang" = "jp" ;then
    msg1="ディストリビューションを確認してください。\nDebianかUbuntu以外のOSの可能性があります。"
    msg2="コードネーム :"
    msg3="このOSはサポートしておりません。"
    msg4="OpenRTM-aist のリポジトリが登録されていません。"
    msg5="Source.list に OpenRTM-aist のリポジトリ: "
    msg6="を追加します。よろしいですか？(y/n)[y] "
    msg7="中断します。"
    msg8="ルートユーザーで実行してください。"
    msg9="インストール中です..."
    msg10="完了"
    msg11="アンインストール中です."
else
    msg1="This distribution may not be debian/ubuntu."
    msg2="The code name is : "
    msg3="This OS is not supported."
    msg4="No repository entry for OpenRTM-aist is configured in your system."
    msg5="repository entry for OpenRTM-aist: "
    msg6="Do you want to add new repository entry for OpenRTM-aist in source.list? (y/n) [y] "
    msg7="Abort."
    msg8="This script should be run as root."
    msg9="Now installing: "
    msg10="done."
    msg11="Now uninstalling: "
fi

}

#----------------------------------------
# 近いリポジトリサーバを探す
#----------------------------------------
check_reposerver()
{
  minrtt=65535
  nearhost=''
  for host in $reposervers; do
    rtt=`ping -c 1 $host | grep 'time=' | sed -e 's/^.*time=\([0-9\.]*\) ms.*/\1/' 2> /dev/null`
    if test "x$rtt" = "x"; then
      rtt=65535
    fi
    if test `echo "scale=2 ; $rtt < $minrtt" | bc` -gt 0; then
      minrtt=$rtt
      nearhost=$host
    fi
  done
  if test "x$nearhost" = "x"; then
    echo "Repository servers unreachable.", $hosts
    echo "Check your internet connection. (or are you using proxy?)"
    nearhost=$default_reposerver
  fi
  reposerver=$nearhost
}

#---------------------------------------
# リポジトリサーバ
#---------------------------------------
create_srclist () {
  codename=`sed -n /DISTRIB_CODENAME=/p /etc/lsb-release`
  cnames=`echo "$codename" | sed 's/DISTRIB_CODENAME=//'`
  for c in $cnames; do
    if test -f "/etc/apt/sources.list"; then
      res=`grep $c /etc/apt/sources.list`
      res2=`grep -r $c /etc/apt/sources.list.d`
    else
      echo $msg1
      exit
    fi
    if test ! "x$res" = "x" ||
       test ! "x$res2" = "x" ; then
      code_name=$c
    fi
  done
  if test ! "x$code_name" = "x"; then
    echo $msg2 $code_name
  else
    echo $msg3
    exit
  fi
  openrtm_repo="deb [arch=$(dpkg --print-architecture) signed-by=/etc/apt/keyrings/openrtm.key] http://$reposerver/pub/Linux/ubuntu/ $code_name main"
  fluent_repo="deb [arch=$(dpkg --print-architecture) signed-by=/usr/share/keyrings/fluentbit-keyring.gpg] https://packages.fluentbit.io/ubuntu/$code_name $code_name main"
}

#---------------------------------------
# ソースリスト更新関数の定義
#---------------------------------------
update_source_list () {
  rtmsite1=`grep $reposerver /etc/apt/sources.list`
  rtmsite2=`grep -r $reposerver /etc/apt/sources.list.d`
  if test "x$rtmsite1" = "x" &&
     test "x$rtmsite2" = "x" ; then
    echo $msg4
    echo $msg5
    echo "  " $openrtm_repo
    read -p "$msg6" kick_shell

    if test "x$kick_shell" = "xn" ; then
      echo $msg7
      exit 0
    else
      echo $openrtm_repo | sudo tee /etc/apt/sources.list.d/openrtm.list > /dev/null
      if [ ! -d /etc/apt/keyrings ]; then
        sudo mkdir -p /etc/apt/keyrings
      fi
      sudo wget --secure-protocol=TLSv1_2 --no-check-certificate https://openrtm.org/pub/openrtm.key -O /etc/apt/keyrings/openrtm.key
    fi
  elif test "x$rtmsite2" != "x" &&
       [ ! -e /etc/apt/keyrings/openrtm.key ]; then
    sudo wget --secure-protocol=TLSv1_2 --no-check-certificate https://openrtm.org/pub/openrtm.key -O /etc/apt/keyrings/openrtm.key
  fi
  fluentsite=`apt-cache policy | grep "https://packages.fluentbit.io"`
  if test "x$fluentsite" = "x" &&
     test "x$OPT_COREDEVEL" = "xtrue" ; then
    echo $fluent_repo | sudo tee /etc/apt/sources.list.d/fluentbit.list > /dev/null
    wget -O - https://packages.fluentbit.io/fluentbit.key | gpg --dearmor | sudo tee /usr/share/keyrings/fluentbit-keyring.gpg >/dev/null
  fi
}

#----------------------------------------
# パッケージインストール関数
#----------------------------------------
install_packages () {
#    echo $*
#    exit 0
  for p in $*; do
    echo $msg9 $p
    echo $install_pkgs | grep -x $p > /dev/null 2>&1
    if [ $? -ne 0 ]; then
      tmp_pkg="$install_pkgs $p"
      install_pkgs=$tmp_pkg
    fi

    if test "x$FORCE_YES" = "xtrue" ; then
      sudo apt install --assume-yes --allow-unauthenticated $p
    else
      sudo apt install $p
    fi
    if [ $? -ne 0 ]; then
      echo $msg7
      exit
    fi
    echo $msg10
    echo ""
  done
}

#------------------------------------------------------------
# リストを逆順にする
#------------------------------------------------------------
reverse () {
  for i in $*; do
    echo $i
  done | sed '1!G;h;$!d'
}

#----------------------------------------
# パッケージをアンインストールする
#----------------------------------------
uninstall_packages () {
  for p in $*; do
    echo $msg11 $p
    echo $uninstall_pkgs | grep -x $p > /dev/null 2>&1
    if [ $? -ne 0 ]; then
      tmp_pkg="$uninstall_pkgs $p"
      uninstall_pkgs=$tmp_pkg
    fi
    sudo apt --purge remove $p
    if test "$?" != 0; then
      sudo apt purge $p
    fi
    echo $msg10
    echo ""
  done
}

#-------------------------------------------------
# If OpenRTM-aist 1.2 is installed, uninstall its.
# Target: openrtm-aist-python3, openrtm-aist-java
#         rtshell
#-------------------------------------------------
uninstall_openrtm12 ()
{
  if test "x$arg_python" = "xtrue" ; then
    res=`dpkg -l | grep openrtm-aist-python3`
    if test ! "x$res" = "x" ; then
      echo "Uninstall openrtm-aist-python3."
      sudo apt --purge remove $openrtm_py
      uninstall_pkgs=$openrtm_py
    fi
  fi

  if test "x$arg_java" = "xtrue" ; then
    res=`dpkg -l | grep openrtm-aist-java`
    if test ! "x$res" = "x" ; then
      echo "Uninstall openrtm-aist-java."
      sudo apt --purge remove $openrtm_java
      tmp_pkg="$uninstall_pkgs $openrtm_java"
      uninstall_pkgs=$tmp_pkg
    fi
  fi

  if test "x$arg_rtshell" = "xtrue" ; then
    res=`pip3 list | grep rtshell`
    if test ! "x$res" = "x" ; then
      echo "Uninstall rtshell."
      rtshell="rtshell-aist rtctree-aist rtsprofile-aist"
      sudo pip3 uninstall -y $rtshell
      tmp_pkg="$uninstall_pkgs rtshell"
      uninstall_pkgs=$tmp_pkg
    fi
  fi
}

#---------------------------------------
# set_package_content
#---------------------------------------
set_package_content()
{
#--------------------------------------- C++
runtime_pkgs="$omni_runtime $openrtm2_runtime"
u_runtime_pkgs=$openrtm2_runtime

src_pkgs="$cxx_devel $cmake_tools $deb_pkg $base_tools $omni_runtime $omni_devel"
u_src_pkgs="$omni_runtime $omni_devel"

dev_pkgs="$runtime_pkgs $src_pkgs $openrtm2_devel"
u_dev_pkgs="$u_runtime_pkgs $openrtm2_devel"

if test "x$code_name" = "xfocal" || test "x$code_name" = "xjammy" ; then
  core_pkgs="$src_pkgs $autotools $build_tools $pkg_tools $fluentbit19"
else
  core_pkgs="$src_pkgs $autotools $build_tools $pkg_tools"
fi
u_core_pkgs="$u_src_pkgs"

ros_pkg="$openrtm2_ros"
u_ros_pkg=$ros_pkg

ros2_pkg="$openrtm2_ros2"
u_ros2_pkg=$ros2_pkg

#--------------------------------------- Python
python_runtime_pkgs="$omni_runtime $python_runtime $openrtm2_py_runtime"
u_python_runtime_pkgs="$openrtm2_py_runtime"

python_dev_pkgs="$python_runtime_pkgs $python_devel"
u_python_dev_pkgs="$u_python_runtime_pkgs"

python_core_pkgs="$omni_runtime $python_runtime $python_devel $build_tools $pkg_tools"
u_python_core_pkgs="$omni_runtime $omnipy"

#--------------------------------------- Java
java_runtime_pkgs="$omni_runtime $openrtm2_j_runtime"
u_java_runtime_pkgs="$openrtm2_j_runtime"

java_dev_pkgs="$java_runtime_pkgs $cmake_tools $base_tools $openrtm2_j_devel"
u_java_dev_pkgs="$u_java_runtime_pkgs $openrtm2_j_devel"

java_core_pkgs="$omni_runtime $cmake_tools $base_tools $build_tools $java_build $pkg_tools"
u_java_core_pkgs="$omni_runtime"
}

#---------------------------------------
# install_proc
#---------------------------------------
install_proc()
{
  local msg tmp

  if test "x$arg_cxx" = "xtrue" ; then
    if test "x$OPT_COREDEVEL" = "xtrue" ; then
      select_opt_c="[c++] ${op_c_msg}"
      install_packages $core_pkgs
    elif test "x$OPT_SRCPKG" = "xtrue" ; then
      select_opt_c="[c++] ${op_s_msg}"
      install_packages $src_pkgs
    elif test "x$OPT_RUNTIME" = "xtrue" ; then
      select_opt_c="[c++] ${op_r_msg}"
      install_packages $runtime_pkgs
    else
      OPT_DEVEL=true
      select_opt_c="[c++] ${op_d_msg}"
      install_packages $dev_pkgs
    fi
    if test "x$OPT_ROS" = "xtrue" ; then
      if test "x$OPT_DEVEL" = "xtrue" || test "x$OPT_RUNTIME" = "xtrue" ; then
        select_opt_c="$select_opt_c\n[c++] install ROS expansion package"
        install_packages $ros_pkg
      fi
    fi
    if test "x$OPT_ROS2" = "xtrue" ; then
      if test "x$OPT_DEVEL" = "xtrue" || test "x$OPT_RUNTIME" = "xtrue" ; then
        select_opt_c="$select_opt_c\n[c++] install ROS2 expansion package"
        install_packages $ros2_pkg
      fi
    fi
  fi

  if test "x$arg_python" = "xtrue" ; then
    if test "x$OPT_COREDEVEL" = "xtrue" ; then
      select_opt_p="[python] ${op_c_msg}"
      install_packages $python_core_pkgs
      pip3 install fluent-logger
      tmp_pkg="$install_pkgs fluent-logger"
      install_pkgs=$tmp_pkg
    elif test "x$OPT_RUNTIME" = "xtrue" ; then
      select_opt_p="[python] ${op_r_msg}"
      install_packages $python_runtime_pkgs
    else
      select_opt_p="[python] ${op_d_msg}"
      install_packages $python_dev_pkgs
    fi
  fi

  if test "x$arg_java" = "xtrue" ; then
    if test "x$OPT_COREDEVEL" = "xtrue" ; then
      select_opt_j="[java] ${op_c_msg}"
      install_packages $java_core_pkgs
    elif test "x$OPT_RUNTIME" = "xtrue" ; then
      select_opt_j="[java] ${op_r_msg}"
      install_packages $java_runtime_pkgs
    else
      select_opt_j="[java] ${op_d_msg}"
      install_packages $java_dev_pkgs
    fi
  fi

  if test "x$arg_openrtp" = "xtrue" ; then
    select_opt_rtp="[openrtp] install"
    install_packages $openrtp_pkgs
  fi

  if test "x$arg_rtshell" = "xtrue" ; then
    select_opt_shl="[rtshell] install"
    install_packages python3-pip
    rtshell_ret=`sudo python3 -m pip install rtshell-aist`
    if test "x$rtshell_ret" != "x"; then
      sudo rtshell_post_install -n
    else
      rts_msg="[ERROR] Failed to install rtshell-aist."
      rts_msg2="Please add the following text to /etc/pip.conf and run the script again."
      rts_msg3="[global]"
      rts_msg4="break-system-packages = true"
    fi
  fi
}

#---------------------------------------
# uninstall_proc
#---------------------------------------
uninstall_proc()
{
  local msg tmp

  if test "x$arg_cxx" = "xtrue" ; then
    if test "x$OPT_COREDEVEL" = "xtrue" ; then
      select_opt_c="[c++] un${op_c_msg}"
      uninstall_packages `reverse $u_core_pkgs`
    elif test "x$OPT_SRCPKG" = "xtrue" ; then
      select_opt_c="[c++] un${op_s_msg}"
      uninstall_packages `reverse $u_src_pkgs`
    elif test "x$OPT_RUNTIME" = "xtrue" ; then
      select_opt_c="[c++] un${op_r_msg}"
      uninstall_packages `reverse $u_runtime_pkgs`
    else
      OPT_DEVEL=true
      select_opt_c="[c++] un${op_d_msg}"
      uninstall_packages `reverse $u_dev_pkgs`
    fi
    if test "x$OPT_ROS" = "xtrue" ; then
      if test "x$OPT_DEVEL" = "xtrue" || test "x$OPT_RUNTIME" = "xtrue" ; then
        select_opt_c="$select_opt_c\n[c++] uninstall ROS expansion package"
        uninstall_packages `reverse $ros_pkg`
      fi
    fi
    if test "x$OPT_ROS2" = "xtrue" ; then
      if test "x$OPT_DEVEL" = "xtrue" || test "x$OPT_RUNTIME" = "xtrue" ; then
        select_opt_c="$select_opt_c\n[c++] uninstall ROS2 expansion package"
        uninstall_packages `reverse $ros2_pkg`
      fi
    fi
  fi

  if test "x$arg_python" = "xtrue" ; then
    if test "x$OPT_COREDEVEL" = "xtrue" ; then
      select_opt_p="[python] un${op_c_msg}"
      uninstall_packages `reverse $u_python_core_pkgs`
    elif test "x$OPT_RUNTIME" = "xtrue" ; then
      select_opt_p="[python] un${op_r_msg}"
      uninstall_packages `reverse $u_python_runtime_pkgs`
    else
      select_opt_p="[python] un${op_d_msg}"
      uninstall_packages `reverse $u_python_dev_pkgs`
    fi
  fi

  if test "x$arg_java" = "xtrue" ; then
    if test "x$OPT_COREDEVEL" = "xtrue" ; then
      select_opt_j="[java] un${op_c_msg}"
      uninstall_packages `reverse $u_java_core_pkgs`
    elif test "x$OPT_RUNTIME" = "xtrue" ; then
      select_opt_j="[java] un${op_r_msg}"
      uninstall_packages `reverse $u_java_runtime_pkgs`
    else
      select_opt_j="[java] un${op_d_msg}"
      uninstall_packages `reverse $u_java_dev_pkgs`
    fi
  fi

  if test "x$arg_openrtp" = "xtrue" ; then
    select_opt_rtp="[openrtp] uninstall"
    uninstall_packages $openrtp_pkgs
  fi

  if test "x$arg_rtshell" = "xtrue" ; then
    select_opt_shl="[rtshell] uninstall"
    rtshell_ret=`sudo python3 -m pip uninstall -y rtshell-aist rtctree-aist rtsprofile-aist`
    if test "x$rtshell_ret" = "x"; then
      msg="\n[ERROR] Failed to uninstall rtshell-aist."
      tmp="$err_message$msg"
      err_message=$tmp
    fi
  fi
}

#---------------------------------------
# print_option
#---------------------------------------
print_option()
{
  cat <<EOF

=============================================
 Selected options are ...
=============================================
EOF

  if test ! "x$select_opt_c" = "x" ; then
    echo -e $select_opt_c
  fi
  if test ! "x$select_opt_p" = "x" ; then
    echo $select_opt_p
  fi
  if test ! "x$select_opt_j" = "x" ; then
    echo $select_opt_j
  fi
  if test ! "x$select_opt_rtp" = "x" ; then
    echo $select_opt_rtp
  fi
  if test ! "x$select_opt_shl" = "x" ; then
    echo $select_opt_shl
  fi
}

#---------------------------------------
# install_result
#---------------------------------------
install_result()
{
  print_option
  cat <<EOF

=============================================
 Installed packages are ...
=============================================
EOF
  if [ $# -eq 0 ] && test "x$OPT_UNINST" = "xfalse"; then
    echo "There is no installed package."
    return
  fi

  for p in $*; do
    echo $p
  done
  if test "x$arg_rtshell" = "xtrue" && test "x$OPT_UNINST" = "xtrue"; then
    if test "x$rtshell_ret" != "x"; then
      echo "rtshell" 
    fi
  fi
}

#---------------------------------------
# uninstall_result
#---------------------------------------
uninstall_result()
{
  cat <<EOF

=============================================
 Uninstalled packages are ...
=============================================
EOF
  if [ $# -eq 0 ] && test "x$OPT_UNINST" = "xtrue"; then
    if test "x$uninstall_pkgs" = "x"; then
      echo "There is no uninstalled package."
      return
    fi
  fi

  for p in $*; do
    echo $p
  done
  if test "x$arg_rtshell" = "xtrue" && test "x$OPT_UNINST" = "xfalse"; then
    if test "x$rtshell_ret" != "x"; then
      echo "rtshell" 
    fi
  fi
}

#---------------------------------------
# メイン
#---------------------------------------
init_param
get_opt $@

# 最終オプション確認
if test "x$arg_all" = "xfalse" ; then
  if test "x$arg_cxx" = "xfalse" ; then
    if test "x$arg_python" = "xfalse" ; then
      if test "x$arg_java" = "xfalse" ; then
        if test "x$arg_openrtp" = "xfalse" ; then
          if test "x$arg_rtshell" = "xfalse" ; then
            exit
          fi
        fi
      fi
    fi
  fi
fi

check_lang
check_reposerver
create_srclist
update_source_list
sudo apt autoclean
sudo apt update

if test "x$arg_all" = "xtrue" &&
   test "x$OPT_OLD_RTM" = "xfalse" ; then
  arg_cxx=true
  arg_python=true
  arg_java=true
  if test "x${ARCH}" = "xaarch64"; then
    arg_openrtp=false
    msg="[WARNING] openrtp is not supported in aarch64 environment."
    tmp="$err_message$LF$msg"
    err_message=$tmp
  else
    arg_openrtp=true
  fi
  arg_rtshell=true

  if test "x$OPT_RUNTIME" != "xtrue" && 
     test "x$OPT_DEVEL" != "xtrue" &&
     test "x$OPT_SRCPKG" != "xtrue" && 
     test "x$OPT_COREDEVEL" != "xtrue" ; then
    # set default option
    OPT_DEVEL=true
  fi
fi

set_package_content

if test "x$OPT_UNINST" = "xtrue" ; then
  uninstall_openrtm12
  install_proc
else
  uninstall_proc
fi

# install openjdk-8-jdk
if test "x$OPT_UNINST" = "xtrue" ; then
  sudo apt -y install openjdk-8-jdk
  JAVA8=`update-alternatives --list java | grep java-8`
  sudo update-alternatives --set java ${JAVA8}
fi

if test "x$OPT_COREDEVEL" = "xtrue" ; then
  if test "x$code_name" = "xfocal" || test "x$code_name" = "xjammy" ; then
    sudo systemctl enable td-agent-bit
    sudo systemctl start td-agent-bit
  fi
fi

install_result $install_pkgs
uninstall_result $uninstall_pkgs
if test ! "x$err_message" = "x" ; then
  ESC=$(printf '\033')
  echo $LF
  echo "${ESC}[33m${err_message}${ESC}[m"
fi

if test ! "x$rts_msg" = "x" ; then
  ESC=$(printf '\033')
  echo $LF
  echo "${ESC}[33m${rts_msg}${ESC}[m"
  echo "${ESC}[33m${rts_msg2}${ESC}[m"
  echo "${ESC}[33m${rts_msg3}${ESC}[m"
  echo "${ESC}[33m${rts_msg4}${ESC}[m"
fi

ESC=$(printf '\033')
if test "x$OPT_UNINST" = "xtrue" &&
   test "x$arg_cxx" = "xtrue" &&
   test "x$OPT_COREDEVEL" = "xfalse" &&
   test "x$code_name" != "xnoble" ; then
  echo "code_name = "$code_name
  msg1='To use the log collection extension using the Fluentd logger,'
  msg2='please install Fluent Bit by following the steps on the following web page.'
  msg3='https://docs.fluentbit.io/manual/installation/linux/ubuntu'
  echo $LF
  echo "${ESC}[33m${msg1}${ESC}[m"
  echo "${ESC}[33m${msg2}${ESC}[m"
  echo "${ESC}[33m${msg3}${ESC}[m"
fi
if test "x$OPT_UNINST" = "xfalse" ; then
  msg1='omniorb or other OpenRTM dependent packages may still exist. '
  msg2='If you want to remove them, please do “apt autoremove” later.'
  echo $LF
  echo "${ESC}[33m${msg1}${ESC}[m"
  echo "${ESC}[33m${msg2}${ESC}[m"
fi
