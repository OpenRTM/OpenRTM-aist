#!/usr/bin/env python3
# -*- python -*-
#
#  @file cxx_gen.py
#  @brief rtc-template test case generator
#  @date $Date$
#  @author Noriaki Ando <n-ando@aist.go.jp>
# 
#  Copyright (C) 2008
#      Task-intelligence Research Group,
#      Intelligent Systems Research Institute,
#      National Institute of
#          Advanced Industrial Science and Technology (AIST), Japan
#      All rights reserved.
# 
#  $Id$
# 

import yat
import random
import os

myservice_idl = """
typedef sequence<string> EchoList;
typedef sequence<float> ValueList;
interface MyService
{
  string echo(in string msg);
  EchoList get_echo_history();
  void set_value(in float value);
  float get_value();
  ValueList get_value_history();
};
"""

myservice_impl_h = """
#include "MyServiceSkel.h"

#ifndef MYSERVICESVC_IMPL_H
#define MYSERVICESVC_IMPL_H
 
/*
 * Example class implementing IDL interface MyService
 */
class MyServiceSVC_impl
 : public virtual POA_MyService,
   public virtual PortableServer::RefCountServantBase
{
 private:
   // Make sure all instances are built on the heap by making the
   // destructor non-public
   //virtual ~MyServiceSVC_impl();

 public:
   // standard constructor
   MyServiceSVC_impl();
   virtual ~MyServiceSVC_impl();

   // attributes and operations
   char* echo(const char* msg)
     throw (CORBA::SystemException);
   EchoList* get_echo_history()
     throw (CORBA::SystemException);
   void set_value(CORBA::Float value)
     throw (CORBA::SystemException);
   CORBA::Float get_value()
     throw (CORBA::SystemException);
   ValueList* get_value_history()
     throw (CORBA::SystemException);

private:
  CORBA::Float m_value;
  EchoList m_echoList;
  ValueList m_valueList;
};



#endif // MYSERVICESVC_IMPL_H
"""

myservice_impl_cpp = """\
#include "MyServiceSVC_impl.h"
#include <rtm/CORBA_SeqUtil.h>
#include <iostream>

template <class T>
struct seq_print
{
  seq_print() : m_cnt(0) {};
  void operator()(T val)
  {
    std::cout << m_cnt << ": " << val << std::endl;
    ++m_cnt;
  }
  int m_cnt;
};

/*
 * Example implementational code for IDL interface MyService
 */
MyServiceSVC_impl::MyServiceSVC_impl()
{
  // Please add extra constructor code here.
}


MyServiceSVC_impl::~MyServiceSVC_impl()
{
  // Please add extra destructor code here.
}


/*
 * Methods corresponding to IDL attributes and operations
 */
char* MyServiceSVC_impl::echo(const char* msg)
  throw (CORBA::SystemException)
{
  CORBA_SeqUtil::push_back(m_echoList, msg);
  std::cout << "MyService::echo() was called." << std::endl;
  std::cout << "Message: " << msg << std::endl;
  return CORBA::string_dup(msg);
}

EchoList* MyServiceSVC_impl::get_echo_history()
  throw (CORBA::SystemException)
{
  std::cout << "MyService::get_echo_history() was called." << std::endl;
  CORBA_SeqUtil::for_each(m_echoList, seq_print<const char*>());
  
  EchoList_var el;
  el = new EchoList(m_echoList);
  return el._retn();
}

void MyServiceSVC_impl::set_value(CORBA::Float value)
  throw (CORBA::SystemException)
{
  CORBA_SeqUtil::push_back(m_valueList, value);
  m_value = value;

  std::cout << "MyService::set_value() was called." << std::endl;
  std::cout << "Current value: " << m_value << std::endl;

  return;
}

CORBA::Float MyServiceSVC_impl::get_value()
  throw (CORBA::SystemException)
{
  std::cout << "MyService::get_value() was called." << std::endl;
  std::cout << "Current value: " << m_value << std::endl;

  return m_value;
}

ValueList* MyServiceSVC_impl::get_value_history()
  throw (CORBA::SystemException)
{
  std::cout << "MyService::get_value_history() was called." << std::endl;
  CORBA_SeqUtil::for_each(m_valueList, seq_print<CORBA::Float>());

  ValueList_var vl;
  vl = new ValueList(m_valueList);
  return vl._retn();
}
"""


yourservice_idl = """
interface YourService {
  void func0();

  void func1(in short val);
  void func2(inout short val);
  void func3(out short val);

  void func4(in long val);
  void func5(inout long val);
  void func6(out long val);

  void func7(in float val);
  void func8(inout float val);
  void func9(out float val);

  void func10(in double val);
  void func11(inout double val);
  void func12(out double val);

};
"""

vector_convert_h = """\
// -*- C++ -*-

#include <istream>
#include <ostream>
#include <vector>
#include <string>
#include <coil/stringutil.h>

template<typename T>
std::istream& operator>>(std::istream& is, std::vector<T>& v)
{
  std::string s;
  coil::vstring sv;
  is >> s;
  sv = coil::split(s ,",");
  v.resize(sv.size());
  for (int i(0), len(sv.size()); i < len; ++i)
    {
      T tv;
      if (coil::stringTo(tv, sv[i].c_str()))
        {
          v[i] = tv;
        }
    }
  return is;
}
"""

gen_main = """\
rtc-template -bcxx
    --module-name=[name] --module-desc="Test RTC for rtc-template"
    --module-version=1.0.0 --module-vendor=AIST --module-category=Test
    --module-comp-type=DataFlowComponent --module-act-type=SPORADIC
    --module-max-inst=10
[for ip in inports]
    --inport="[ip.name]:[ip.type]"
[endfor]

[for op in outports]
    --outport="[op.name]:[op.type]"
[endfor]

[for sp in svcports]
    --[sp.direction]="[sp.port_name]:[sp.inst_name]:[sp.type]"
    --[sp.direction]-idl="[sp.idl_file]"
[endfor]

[for cf in configs]
    --config="[cf.name]:[cf.type]:[cf.default]"
[endfor]
"""

rtm_config = """\
#!/bin/sh
#
# this is for only Linux!!
#
rtm_prefix="/usr/local"
rtm_exec_prefix="/usr/local"
rtm_cxx="g++"
rtm_cflags="-Wall -fPIC -O2 -I../../../../src/lib -I../../../../src/lib/rtm/idl -I../../../../src/lib/coil/include"
rtm_libs="-export-dynamic -luuid -ldl -L../../../../src/lib/rtm/.libs -L../../../../src/lib/coil/lib/.libs -lpthread -lomniORB4 -lomnithread -lomniDynamic4 -lRTC -lcoil"
rtm_libdir=""
rtm_version="1.0.0"
rtm_orb="omniORB"
rtm_idlc="omniidl"
rtm_idlflags="-bcxx -Wba -nf -Wbshortcut"

usage()
{
        cat <<EOF
Usage: rtm-config [OPTIONS]
Options:
        [--prefix[=DIR]]
        [--exec-prefix[=DIR]]
        [--version]
        [--cxx]
        [--cflags]
        [--libs]
        [--libdir]
        [--orb]
        [--idlc]
        [--idlflags]
EOF
        exit $1
}

if test $# -eq 0; then
        usage 1 1>&2
fi


while test $# -gt 0; do
  case "$1" in
  -*=*) optarg=`echo "$1" | sed 's/[-_a-zA-Z0-9]*=//'` ;;
  *) optarg= ;;
  esac

  case $1 in
    --prefix=*)
      prefix=$optarg
      if test $exec_prefix_set = no ; then
        exec_prefix=$optarg
      fi
      ;;
    --prefix)
      echo_prefix=yes
      ;;
    --exec-prefix=*)
      rtm_exec_prefix=$optarg
      exec_prefix_set=yes
      ;;
    --exec-prefix)
      echo_exec_prefix=yes
      ;;
    --version)
      echo $rtm_version
      ;;
    --cxx)
      echo_cxx=yes
      ;;
    --cflags)
      echo_cflags=yes
      ;;
    --libs)
      echo_libs=yes
      ;;
    --libdir)
      echo_libdir=yes
	  ;;
    --orb)
      echo_orb=yes
      ;;
    --idlc)
      echo_idlc=yes
      ;;
    --idlflags)
      echo_idlflags=yes
      ;;
    *)
      usage 1 1>&2
      ;;
  esac
  shift
done

if test "$echo_prefix" = "yes"; then
        echo $rtm_prefix
fi

if test "$echo_exec_prefix" = "yes"; then
        echo $rtm_exec_prefix
fi

if test "$echo_cxx" = "yes"; then
      echo $rtm_cxx
fi

if test "$echo_cflags" = "yes"; then
      echo $rtm_cflags
fi

if test "$echo_libs" = "yes"; then
      echo $rtm_libs
fi      

if test "$echo_libdir" = "yes"; then
      echo $rtm_libdir
fi      

if test "$echo_orb" = "yes"; then
      echo $rtm_orb
fi      

if test "$echo_idlc" = "yes"; then
      echo $rtm_idlc
fi      

if test "$echo_idlflags" = "yes"; then
      echo $rtm_idlflags
fi      

"""

bat_header = """\
@set PATH="%RTM_ROOT%\\utils\\rtc-template";%PATH%
@set PYTHONPATH="%RTM_ROOT%\\utils\\rtc-template"

del /F /Q *.cpp *.h *.hh *.cc *.sln *.vcproj *.vsprops *.yaml
del /F /Q copyprops.bat Makefile.* README.* 

rmdir /Q /S [name]

rmdir /Q /S [name]Comp

"""

sh_header = """\
#!/bin/sh
export PYTHONPATH=../../:../../py_helper
export RTM_ROOT=../../../../
export PATH=./:../../:../../../rtm2-skelwrapper:$PATH

rm -rf *.cpp *.h *.hh *.cc *.sln *.vcproj *.vsprops *.yaml
rm -rf copyprops.bat Makefile.* README.*
rm -rf [name] [name]Comp

"""

bat_footer = """

call copyprops.bat
for %%x in (*.tmp) do copy /y %%x %%~nx

echo #include "VectorConvert.h" >> [name].h

"""

sh_footer = """

for tmpname in *.tmp
do
    fname=`basename $tmpname .tmp`
    cp -f $tmpname $fname
done

echo '#include "VectorConvert.h"' >> [name].h

make -f Makefile.*
"""

build_vc8_bat = """\
@set PATH="C:\\Program Files\\Microsoft Visual Studio 8\\VC\\vcpackages";%PATH%

[for proj in projects]
cd [proj] 
call gen.bat
vcbuild [proj]_vc8.sln
cd ..
[endfor]
"""

build_vc9_bat = """\
@set PATH="C:\\Program Files\\Microsoft Visual Studio 9.0\\VC\\vcpackages";%PATH%

[for proj in projects]
cd [proj] 
call gen.bat
vcbuild [proj]_vc9.sln
cd ..
[endfor]
"""

build_sh = """\
#!/bin/sh

[for proj in projects]
cd [proj] 
sh gen.sh
#make -f Makefile.[proj] 
if test -f [proj]Comp; then
  echo [proj]: OK >> ../build_status
else
  echo [proj]: NG >> ../build_status
fi
cd ..
[endfor]
"""

inports = [
    {"name": "in00", "type": "TimedShort"},
    {"name": "in01", "type": "TimedLong"},
    {"name": "in02", "type": "TimedUShort"},
    {"name": "in03", "type": "TimedULong"},
    {"name": "in04", "type": "TimedFloat"},
    {"name": "in05", "type": "TimedDouble"},
    {"name": "in06", "type": "TimedChar"},
    {"name": "in07", "type": "TimedBoolean"},
    {"name": "in08", "type": "TimedOctet"},
    {"name": "in09", "type": "TimedString"},
    {"name": "in10", "type": "TimedShortSeq"},
    {"name": "in11", "type": "TimedLongSeq"},
    {"name": "in12", "type": "TimedUShortSeq"},
    {"name": "in13", "type": "TimedULongSeq"},
    {"name": "in14", "type": "TimedFloatSeq"},
    {"name": "in15", "type": "TimedDoubleSeq"},
    {"name": "in16", "type": "TimedCharSeq"},
    {"name": "in17", "type": "TimedBooleanSeq"},
    {"name": "in18", "type": "TimedOctetSeq"},
    {"name": "in19", "type": "TimedStringSeq"}
    ]

outports = [
    {"name": "out00", "type": "TimedShort"},
    {"name": "out01", "type": "TimedLong"},
    {"name": "out02", "type": "TimedUShort"},
    {"name": "out03", "type": "TimedULong"},
    {"name": "out04", "type": "TimedFloat"},
    {"name": "out05", "type": "TimedDouble"},
    {"name": "out06", "type": "TimedChar"},
    {"name": "out07", "type": "TimedBoolean"},
    {"name": "out08", "type": "TimedOctet"},
    {"name": "out09", "type": "TimedString"},
    {"name": "out10", "type": "TimedShortSeq"},
    {"name": "out11", "type": "TimedLongSeq"},
    {"name": "out12", "type": "TimedUShortSeq"},
    {"name": "out13", "type": "TimedULongSeq"},
    {"name": "out14", "type": "TimedFloatSeq"},
    {"name": "out15", "type": "TimedDoubleSeq"},
    {"name": "out16", "type": "TimedCharSeq"},
    {"name": "out17", "type": "TimedBooleanSeq"},
    {"name": "out18", "type": "TimedOctetSeq"},
    {"name": "out19", "type": "TimedStringSeq"}
    ]

configs = [
    {"name": "conf_short", "type": "short",
     "default": "12"},
    {"name": "conf_int", "type": "int",
     "default": "1234"},
    {"name": "conf_long", "type": "long",
     "default": "-12345"},
    {"name": "conf_ushort", "type": "unsigned short",
     "default": "987"},
    {"name": "conf_uint", "type": "unsigned int",
     "default": "9876"},
    {"name": "conf_ulong", "type": "unsigned long",
     "default": "98765"},
    {"name": "conf_float", "type": "float",
     "default": "3.14159265"},
    {"name": "conf_double", "type": "double",
     "default": "0.0000000001"},
    {"name": "conf_string", "type": "std::string",
     "default": "OpenRTM-aist"},
    {"name": "conf_vshort", "type": "std::vector<short>",
     "default": "0,1,2,3,4,5,6,7,8,9"},
    {"name": "conf_vint", "type": "std::vector<int>",
     "default": "9,8,7,6,5,4,3,2,1,0"},
    {"name": "conf_vlong", "type": "std::vector<long>",
     "default": "0,10,20,30,40,50,60,70,80,90"},
    {"name": "conf_vushort", "type": "std::vector<unsigned short>",
     "default": "1,2"},
    {"name": "conf_vuint", "type": "std::vector<unsigned int>",
     "default": "0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20"},
    {"name": "conf_vulong", "type": "std::vector<unsigned long>",
     "default": "0,10000,20000,30000,40000,50000,60000,70000,80000,90000"},
    {"name": "conf_vfloat", "type": "std::vector<float>",
     "default": "0.1,0.01,0.001,0.0001,0.00001,0.000001,0.0000001,0.00000001"},
    {"name": "conf_vdouble", "type": "std::vector<double>",
     "default": "1.1,1.01,1.001,1.0001,1.00001,1.000001,1.0000001,1.00000001"}
]

svcports = [
    {"type": "MyService", "direction": "service",
     "port_name": "sp0", "inst_name": "my0",
     "idl_file": "MyService.idl", "idl_data": myservice_idl,
     "impl_h": myservice_impl_h, "impl_cpp": myservice_impl_cpp},
    {"type": "MyService", "direction": "consumer",
     "port_name": "sp0", "inst_name": "my1",
     "idl_file": "MyService.idl", "idl_data": myservice_idl,
     "impl_h": myservice_impl_h, "impl_cpp": myservice_impl_cpp},
    {"type": "MyService", "direction": "service",
     "port_name": "sp1", "inst_name": "my2",
     "idl_file": "MyService.idl", "idl_data": myservice_idl,
     "impl_h": myservice_impl_h, "impl_cpp": myservice_impl_cpp},
    {"type": "MyService", "direction": "consumer",
     "port_name": "sp1", "inst_name": "my3",
     "idl_file": "MyService.idl", "idl_data": myservice_idl,
     "impl_h": myservice_impl_h, "impl_cpp": myservice_impl_cpp},
    {"type": "MyService", "direction": "service",
     "port_name": "sp2", "inst_name": "my4",
     "idl_file": "MyService.idl", "idl_data": myservice_idl,
     "impl_h": myservice_impl_h, "impl_cpp": myservice_impl_cpp},
    {"type": "MyService", "direction": "consumer",
     "port_name": "sp2", "inst_name": "my5",
     "idl_file": "MyService.idl", "idl_data": myservice_idl,
     "impl_h": myservice_impl_h, "impl_cpp": myservice_impl_cpp},
    {"type": "YourService", "direction": "service",
     "port_name": "sp0", "inst_name": "your0",
     "idl_file": "YourService.idl", "idl_data": yourservice_idl,
     "impl_h": None, "impl_cpp": None},
    {"type": "YourService", "direction": "consumer",
     "port_name": "sp0", "inst_name": "your1",
     "idl_file": "YourService.idl", "idl_data": yourservice_idl,
     "impl_h": None, "impl_cpp": None},
    {"type": "YourService", "direction": "service",
     "port_name": "sp1", "inst_name": "your2",
     "idl_file": "YourService.idl", "idl_data": yourservice_idl,
     "impl_h": None, "impl_cpp": None},
    {"type": "YourService", "direction": "consumer",
     "port_name": "sp1", "inst_name": "your3",
     "idl_file": "YourService.idl", "idl_data": yourservice_idl,
     "impl_h": None, "impl_cpp": None},
    {"type": "YourService", "direction": "service",
     "port_name": "sp2", "inst_name": "your4",
     "idl_file": "YourService.idl", "idl_data": yourservice_idl,
     "impl_h": None, "impl_cpp": None},
    {"type": "YourService", "direction": "consumer",
     "port_name": "sp2", "inst_name": "your5",
     "idl_file": "YourService.idl", "idl_data": yourservice_idl,
     "impl_h": None, "impl_cpp": None}
    ]
    

nums = [0, 1, 2, 10]

params = {
    "inports": inports,
    "outports": outports,
    "svcports": svcports,
    "configs": configs
    }

def _mkdir(newdir):
    """works the way a good mkdir should :)
    - already exists, silently complete
    - regular file in the way, raise an exception
    - parent directory(ies) does not exist, make them as well
    """
    if os.path.isdir(newdir):
        pass
    elif os.path.isfile(newdir):
        raise OSError("a file with the same name as the desired " \
                          "dir, '%s', already exists." % newdir)
    else:
        head, tail = os.path.split(newdir)
        if head and not os.path.isdir(head):
            _mkdir(head)
        #print "_mkdir %s" % repr(newdir)
        if tail:
            os.mkdir(newdir)

class TestGen:
    def __init__(self, ip_num, op_num, sp_num, cf_num):
        self.ip_num = ip_num
        self.op_num = op_num
        self.sp_num = sp_num
        self.cf_num = cf_num
        for key in params.keys():
            random.shuffle(params[key])
        self.params = params
        self.pjnames = []

    def generate(self):
        self.gen()
        self.craete_build_sh()

    def craete_build_sh(self):
        dict = {}
        dict["projects"] = self.pjnames
        builds = {"build.sh": build_sh,
                  "build_vc8.bat": build_vc8_bat,
                  "build_vc9.bat": build_vc9_bat
                  }
        for key in builds.keys():
            fd = open(key, "w")
            fd.write(yat.Template(builds[key]).generate(dict))
            fd.close()
            os.chmod(key, 0755)

        
    def mkdir(self, dirname):
        _mkdir(dirname)

    def dict(self, ip, op, sp, cf):
        name = "Sample_%d_%d_%d_%d" % (ip, op, sp, cf)
        dict = {"name": name}
        p = ["inports", "outports", "svcports", "configs"]
        n = [ip, op, sp, cf]
        for i in range(len(n)):
            dict[p[i]] = self.params[p[i]][0:n[i]]
        return dict

    def create_gen_sh(self, dict):
        fname = dict["name"] + "/gen.sh"
        fd = open(fname, "w")
        head = yat.Template(sh_header).generate(dict)
        body = yat.Template(gen_main).generate(dict)
        body = self.add_cmark(body, "\\")
        foot = yat.Template(sh_footer).generate(dict)

        sh_all = head + body + foot
        fd.write(sh_all)
        fd.close()
        os.chmod(fname, 0755)
        
    def create_gen_bat(self, dict):
        fname = dict["name"] + "/gen.bat"
        fd = open(fname, "w")
        head = yat.Template(bat_header).generate(dict)
        body = yat.Template(gen_main).generate(dict)
        body = body.replace("rtc-template", "rtc-template.py")
        body = self.add_cmark(body, "^")
        foot = yat.Template(bat_footer).generate(dict)

        bat_all = head + body + foot
        bat_all.replace("\r\n", "\n").replace("\n", "\r\n")
        fd.write(bat_all)
        fd.close()
        os.chmod(fname, 0755)

    def add_cmark(self, text, mark):
        lines = [t for t in text.split("\n") if t != ""]
        return (mark + "\n").join(lines)

    def create_idl(self, dict):
        idls = []
        for svc in dict["svcports"]:
            if not svc["idl_file"] in idls:
                fname = dict["name"] + "/" + svc["idl_file"]
                fd = open(fname, "w")
                fd.write(svc["idl_data"])
                fd.close()
                idls.append(svc["idl_file"])

                if svc["impl_h"] != None:
                    cpp_fname = dict["name"] + "/" + \
                        svc["idl_file"].replace(".idl","SVC_impl.cpp.tmp")
                    fd = open(cpp_fname, "w")
                    fd.write(svc["impl_cpp"])
                    fd.close

                    h_fname = dict["name"] + "/" + \
                        svc["idl_file"].replace(".idl","SVC_impl.h.tmp")
                    fd = open(h_fname, "w")
                    fd.write(svc["impl_h"])
                    fd.close

    def create_vecconv(self, dict):
        fname = dict["name"] + "/" + "VectorConvert.h.tmp"
        fd = open(fname, "w")
        fd.write(vector_convert_h)
        fd.close()

    def create_rtmconfig(self, dict):
        fname = dict["name"] + "/rtm2-config"
        fd = open(fname, "w")
        fd.write(rtm_config)
        fd.close()
        os.chmod(fname, 0755)

    def create_gen(self, dict):
        self.create_gen_sh(dict)
        self.create_gen_bat(dict)
        self.create_idl(dict)
        self.create_vecconv(dict)
        self.create_rtmconfig(dict)

    def gen(self):
        for ip in self.ip_num:
            for op in self.op_num:
                for sp in self.sp_num:
                    for cf in self.cf_num:
                        dict = self.dict(ip, op, sp, cf)
                        self.mkdir(dict["name"])
                        self.create_gen(dict)
                        self.pjnames.append(dict["name"])


if __name__ == "__main__":
    t = TestGen([0,1,5],[0,1,5],[0,1,5],[0,1,5])
#    t = TestGen([5],[5],[5],[5])
    t.generate()
