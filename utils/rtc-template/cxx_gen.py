#!/usr/bin/env python3
# -*- python -*-
#
#  @file cxx_gen.py
#  @brief rtc-template C++ source code generator class
#  @date $Date: 2008/01/13 10:29:34 $
#  @author Noriaki Ando <n-ando@aist.go.jp>
# 
#  Copyright (C) 2004-2007
#      Task-intelligence Research Group,
#      Intelligent Systems Research Institute,
#      National Institute of
#          Advanced Industrial Science and Technology (AIST), Japan
#      All rights reserved.
# 
#  $Id$
# 

import re
import os
import sys
import StringIO
import yat
import gen_base


def description():
	return "C++ component code generator"

def usage_short():
	"""
	C++ generator specific usage (short version)
	"""
	return """
Options for C++ backend:

    [--svc-impl-suffix[=suffix]]          Suffix of implementation class
    [--svc-skel-suffix[=suffix]]          Suffix of server skeleton files
    [--svc-stub-suffix[=suffix]]          Suffix of client stub files
"""

def usage():
	"""
	C++ generator specific usage
	"""
	return """
-------------------------------
  Help for C++ code geenrator
-------------------------------
    --svc-impl-suffix=[Suffix]:
        Specify the suffix for implementation class name. This suffix is also
        used for implementation class header file and code file.
    --svc-skel-suffix=[Suffix]:
        Specify the suffix for server skeleton files.
	
    --svc-stub-suffix=[Suffix]:
        Specify the suffix for client stub files.

C++ code generator generates the following files.
    [Component name].h.............Component class header
    [Component name].cpp...........Component class soruce code
    [Component name]Comp.cpp.......Component startup code
    [IDL basename]Skel.h...........Server skeleton header
    [IDL basename]Skel.cpp.........Server skeleton source code
    [IDL basename]Stub.h...........Client stub header
    [IDL basename]Stub.cpp.........Client stub source code
    [IDL basename]SVC_impl.h.......Server implementation header
    [IDL basename]SVC_impl.cpp.....Server implementation source code
    Makefile.[Component name]......Makefile to compile this codes
    README.[Component name]........Specification template of the component

 Suffixes (Skel, Stub, SVC_impl) can be specified --svc-*-suffix option

 Other CORBA implementation specific skeleton/stub code would be generated.
 --omniORB--
    [IDL basename].hh..............Client stub header
    [IDL basename]SK.cc............Server skeleton source code
    [IDL basename]DynSK.cc.........Dynamic server skeleton source code
 --TAO--
    [IDL basename]S.h..............Server skeleton header
    [IDL basename]S.h..............Server skeleton source code
    [IDL basename]C.h..............Client stub header
    [IDL basename]C.h..............Client stub source code
 --MICO--
    [IDL basename].h...............Server skeleton header
    [IDL basename]_skel.cc.........Server skeleton source code
    [IDL basename].h...............Client stub header
    [IDL basename].cc..............Client stub source code

"""

def get_opt_fmt():
	opt_args_fmt = ["svc-impl-suffix=",
			"svc-skel-suffix=",
			"svc-stub-suffix="]
	return opt_args_fmt

	
#------------------------------------------------------------
# Component header template code
#------------------------------------------------------------
comp_header = """// -*- C++ -*-
/*!
 * @file  [fname_h]
 * @brief [basicInfo.description]
 * @date  [rcs_date]
 
 *
 * [rcs_id]
 
 */
#ifndef [u_name]_H
#define [u_name]_H

#include <rtm/idl/BasicDataTypeSkel.h>
#include <rtm/Manager.h>
#include <rtm/DataFlowComponentBase.h>
#include <rtm/CorbaPort.h>
#include <rtm/DataInPort.h>
#include <rtm/DataOutPort.h>

// Service implementation headers
// <rtc-template block="service_impl_h">
// </rtc-template>

// Service Consumer stub headers
// <rtc-template block="consumer_stub_h">
// </rtc-template>

using namespace RTC;

class [basicInfo.name]
  : public RTC::DataFlowComponentBase
{
 public:
  [basicInfo.name](RTC::Manager* manager);
  ~[basicInfo.name]();

  // The initialize action (on CREATED->ALIVE transition)
  // formaer rtc_init_entry() 
 virtual RTC::ReturnCode_t onInitialize();

  // The finalize action (on ALIVE->END transition)
  // formaer rtc_exiting_entry()
  // virtual RTC::ReturnCode_t onFinalize();

  // The startup action when ExecutionContext startup
  // former rtc_starting_entry()
  // virtual RTC::ReturnCode_t onStartup(RTC::UniqueId ec_id);

  // The shutdown action when ExecutionContext stop
  // former rtc_stopping_entry()
  // virtual RTC::ReturnCode_t onShutdown(RTC::UniqueId ec_id);

  // The activated action (Active state entry action)
  // former rtc_active_entry()
  // virtual RTC::ReturnCode_t onActivated(RTC::UniqueId ec_id);

  // The deactivated action (Active state exit action)
  // former rtc_active_exit()
  // virtual RTC::ReturnCode_t onDeactivated(RTC::UniqueId ec_id);

  // The execution action that is invoked periodically
  // former rtc_active_do()
  // virtual RTC::ReturnCode_t onExecute(RTC::UniqueId ec_id);

  // The aborting action when main logic error occurred.
  // former rtc_aborting_entry()
  // virtual RTC::ReturnCode_t onAborting(RTC::UniqueId ec_id);

  // The error action in ERROR state
  // former rtc_error_do()
  // virtual RTC::ReturnCode_t onError(RTC::UniqueId ec_id);

  // The reset action that is invoked resetting
  // This is same but different the former rtc_init_entry()
  // virtual RTC::ReturnCode_t onReset(RTC::UniqueId ec_id);
  
  // The state update action that is invoked after onExecute() action
  // no corresponding operation exists in OpenRTm-aist-0.2.0
  // virtual RTC::ReturnCode_t onStateUpdate(RTC::UniqueId ec_id);

  // The action that is invoked when execution context's rate is changed
  // no corresponding operation exists in OpenRTm-aist-0.2.0
  // virtual RTC::ReturnCode_t onRateChanged(RTC::UniqueId ec_id);


 protected:
  // Configuration variable declaration
  // <rtc-template block="config_declare">
  // </rtc-template>

  // DataInPort declaration
  // <rtc-template block="inport_declare">
  // </rtc-template>

  // DataOutPort declaration
  // <rtc-template block="outport_declare">
  // </rtc-template>

  // CORBA Port declaration
  // <rtc-template block="corbaport_declare">
  // </rtc-template>

  // Service declaration
  // <rtc-template block="service_declare">
  // </rtc-template>

  // Consumer declaration
  // <rtc-template block="consumer_declare">
  // </rtc-template>

 private:

};


extern "C"
{
  DLL_EXPORT void [basicInfo.name]Init(RTC::Manager* manager);
};

#endif // [u_name]_H
"""


#------------------------------------------------------------
# Component source template code
#------------------------------------------------------------
comp_soruce = """// -*- C++ -*-
/*!
 * @file  [fname_cpp]
 * @brief [basicInfo.description]
 * [rcs_date]
 
 *
 * [rcs_id]
 
 */
#include "[fname_h]"

// Module specification
// <rtc-template block="module_spec">
// </rtc-template>

[basicInfo.name]::[basicInfo.name](RTC::Manager* manager)
    // <rtc-template block="initializer">
    // </rtc-template>
{
}

[basicInfo.name]::~[basicInfo.name]()
{
}


RTC::ReturnCode_t [basicInfo.name]::onInitialize()
{
  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // </rtc-template>

  // <rtc-template block="bind_config">
  // </rtc-template>
  return RTC::RTC_OK;
}


[for act in activity]
/*
RTC::ReturnCode_t [basicInfo.name]::[act.name]([act.args])
{
  return RTC::RTC_OK;
}
*/
[endfor]


extern "C"
{
 
  void [basicInfo.name]Init(RTC::Manager* manager)
  {
    coil::Properties profile([l_name]_spec);
    manager->registerFactory(profile,
                             RTC::Create<[basicInfo.name]>,
                             RTC::Delete<[basicInfo.name]>);
  }
  
};


"""


#------------------------------------------------------------
# Component soruce template
#------------------------------------------------------------
comp_compsrc = """// -*- C++ -*-
/*!
 * @file [basicInfo.name]Comp.cpp
 * @brief Standalone component
 * @date [rcs_date]
 
 *
 * [rcs_id]
 
 */
#include <rtm/Manager.h>
#include <iostream>
#include <string>
#include "[fname_h]"


void MyModuleInit(RTC::Manager* manager)
{
  [basicInfo.name]Init(manager);
  RTC::RtcBase* comp;

  // Create a component
  comp = manager->createComponent("[basicInfo.name]");


  // Example
  // The following procedure is examples how handle RT-Components.
  // These should not be in this function.

  // Get the component's object reference
  //  RTC::RTObject_var rtobj;
  //  rtobj = RTC::RTObject::_narrow(manager->getPOA()->servant_to_reference(comp));

  // Get the port list of the component
  // PortServiceList* portlist;
  // portlist = rtobj->get_ports();

  // getting port profiles
  // std::cout << "Number of Ports: ";
  // std::cout << portlist->length() << std::endl << std::endl; 
  // for (CORBA::ULong i(0), n(portlist->length()); i < n; ++i)
  //   {
  //     Port_ptr port;
  //     port = (*portlist)[[]i];
  //     std::cout << "Port" << i << " (name): ";
  //     std::cout << port->get_port_profile()->name << std::endl;
  //    
  //     RTC::PortInterfaceProfileList iflist;
  //     iflist = port->get_port_profile()->interfaces;
  //     std::cout << "---interfaces---" << std::endl;
  //     for (CORBA::ULong i(0), n(iflist.length()); i < n; ++i)
  //       {
  //         std::cout << "I/F name: ";
  //         std::cout << iflist[[]i].instance_name << std::endl;
  //         std::cout << "I/F type: ";
  //         std::cout << iflist[[]i].type_name << std::endl;
  //         const char* pol;
  //         pol = iflist[[]i].polarity == 0 ? "PROVIDED" : "REQUIRED";
  //         std::cout << "Polarity: " << pol << std::endl;
  //       }
  //     std::cout << "---properties---" << std::endl;
  //     NVUtil::dump(port->get_port_profile()->properties);
  //     std::cout << "----------------" << std::endl << std::endl;
  //  }

  return;
}

int main (int argc, char** argv)
{
  RTC::Manager* manager;
  manager = RTC::Manager::init(argc, argv);

  // Initialize manager
  manager->init(argc, argv);

  // Set module initialization proceduer
  // This procedure will be invoked in activateManager() function.
  manager->setModuleInitProc(MyModuleInit);

  // Activate manager and register to naming service
  manager->activateManager();

  // run the manager in blocking mode
  // runManager(false) is the default.
  manager->runManager();

  // If you want to run the manager in non-blocking mode, do like this
  // manager->runManager(true);

  return 0;
}
"""


#------------------------------------------------------------
# Makefile template
#------------------------------------------------------------
makefile = """# -*- Makefile -*-
#
# @file  Makefile.[basicInfo.name]
# @brief RTComponent makefile for "[basicInfo.name] component"
# @date  [rcs_date]

#
# This file is generated by rtc-template with the following argments.
#
[for args in fmtd_args]
#  [args] [if-index args is last][else]\\[endif]

[endfor]
#
#
# [rcs_id]

#
CXX      = `rtm2-config --cxx`
CXXFLAGS = `rtm2-config --cflags` -I.
LDFLAGS  = `rtm2-config --libs`
SHFLAGS  = -shared

IDLC     = `rtm2-config --idlc`
IDLFLAGS = `rtm2-config --idlflags` -I`rtm2-config --prefix`/include/rtm/idl
WRAPPER  = rtm2-skelwrapper
WRAPPER_FLAGS = --include-dir="" --skel-suffix=Skel --stub-suffix=Stub

SKEL_OBJ = [for sidl in service_idl][sidl.skel_basename].o [endfor] 
STUB_OBJ = [for cidl in consumer_idl][if-any cidl.stub_basename][cidl.stub_basename].o [endif][endfor] 
IMPL_OBJ = [for sidl in service_idl][sidl.impl_basename].o [endfor] 
OBJS     = [basicInfo.name].o $(SKEL_OBJ) $(STUB_OBJ) $(IMPL_OBJ)

.SUFFIXES : .so

all: [basicInfo.name].so [basicInfo.name]Comp


.cpp.o:
	rm -f $@
	$(CXX) $(CXXFLAGS) -c -o $@ $<

.o.so:
	rm -f $@
	$(CXX) $(SHFLAGS) -o $@ $(OBJS) $(LDFLAGS)

[basicInfo.name]Comp: [basicInfo.name]Comp.o $(OBJS)
	$(CXX) -o $@ $(OBJS) [basicInfo.name]Comp.o $(LDFLAGS) 


clean: clean_objs clean_skelstub
	rm -f *~

clean_objs:
	rm -f $(OBJS) [basicInfo.name]Comp.o [basicInfo.name].so [basicInfo.name]Comp

clean_skelstub:
	rm -f *[skel_suffix].h *[skel_suffix].cpp
	rm -f *[stub_suffix].h *[stub_suffix].cpp

[for sidl in service_idl][if-any sidl.skel_basename]
[sidl.skel_basename].cpp : [sidl.idl_fname] 
	$(IDLC) $(IDLFLAGS) [sidl.idl_fname] 
	$(WRAPPER) $(WRAPPER_FLAGS) --idl-file=[sidl.idl_fname] 
[sidl.skel_basename].h : [sidl.idl_fname] 
	$(IDLC) $(IDLFLAGS) [sidl.idl_fname] 
	$(WRAPPER) $(WRAPPER_FLAGS) --idl-file=[sidl.idl_fname] 
[endif][endfor]

[for cidl in consumer_idl][if-any cidl.stub_basename]
[cidl.stub_basename].cpp : [cidl.idl_fname] 
	$(IDLC) $(IDLFLAGS) [cidl.idl_fname] 
	$(WRAPPER) $(WRAPPER_FLAGS) --idl-file=[cidl.idl_fname] 
[cidl.stub_basename].h : [cidl.idl_fname] 
	$(IDLC) $(IDLFLAGS) [cidl.idl_fname] 
	$(WRAPPER) $(WRAPPER_FLAGS) --idl-file=[cidl.idl_fname] 
[endif][endfor]

[basicInfo.name].so: $(OBJS)
[basicInfo.name].o: [basicInfo.name].h [for sidl in service_idl][sidl.skel_basename].h [sidl.impl_basename].h [endfor]
[for cidl in consumer_idl][if-any cidl.stub_basename][cidl.stub_basename].h [endif][endfor] 
[basicInfo.name]Comp.o: [basicInfo.name]Comp.cpp [basicInfo.name].cpp [basicInfo.name].h [for sidl in service_idl][sidl.skel_basename].h [sidl.impl_basename].h [endfor] 

[for sidl in service_idl]
[sidl.impl_basename].o: [sidl.impl_basename].cpp [sidl.impl_basename].h [sidl.skel_basename].h [sidl.stub_basename].h
[sidl.skel_basename].o: [sidl.skel_basename].cpp [sidl.skel_basename].h [sidl.stub_basename].h
[sidl.stub_basename].o: [sidl.stub_basename].cpp [sidl.stub_basename].h
[endfor]

[for cidl in consumer_idl][if-any cidl.skel_basename]
[cidl.skel_basename].o: [cidl.skel_basename].cpp [cidl.skel_basename].h [cidl.stub_basename].h
[cidl.stub_basename].o: [cidl.stub_basename].cpp [cidl.stub_basename].h
[endif][endfor]

# end of Makefile
"""


#============================================================
# Replaced strings definition for <rtc-template> tags
#============================================================
service_impl_h = """[for sidl in service_idl]#include "[sidl.impl_h]"
[endfor]"""
consumer_stub_h = """[for cidl in consumer_idl][if-any cidl.stub_h]
#include "[cidl.stub_h]"
[endif][endfor]"""

module_spec = """static const char* const [l_name]_spec[] =
  {
    "implementation_id", "[basicInfo.name]",
    "type_name",         "[basicInfo.name]",
    "description",       "[basicInfo.description]",
    "version",           "[basicInfo.version]",
    "vendor",            "[basicInfo.vendor]",
    "category",          "[basicInfo.category]",
    "activity_type",     "[basicInfo.activityType]",
    "kind",              "[basicInfo.componentKind]",
    "max_instance",      "[basicInfo.maxInstances]",
    "language",          "C++",
    "lang_type",         "compile",
    // Configuration variables
[for config in configurationSet.configuration]    "conf.default.[config.name]", "[config.defaultValue]",
[endfor]
    ""
  };"""

config_declare = \
"""  [for config in configurationSet.configuration][config.type] m_[config.name];
  [endfor]"""
  

inport_declare = \
"""[for inport in dataPorts][if inport.portType is DataInPort]
  [inport.type] m_[inport.name];
  InPort<[inport.type]> m_[inport.name]In;
[endif][endfor]"""

outport_declare = \
"""[for outport in dataPorts][if outport.portType is DataOutPort]
  [outport.type] m_[outport.name];
  OutPort<[outport.type]> m_[outport.name]Out;
[endif][endfor]"""

corbaport_declare = \
"""[for corbaport in servicePorts]
  RTC::CorbaPort m_[corbaport.name]Port;
[endfor]"""
 
service_declare = \
"""[for service in servicePorts][for interface in service.serviceInterface]
[if interface.direction is Provided]
  [interface.type]SVC_impl m_[interface.name];
[endif]
[endfor][endfor]"""

consumer_declare = \
"""[for service in servicePorts][for interface in service.serviceInterface]
[if interface.direction is Required]
  RTC::CorbaConsumer<[interface.type]> m_[interface.name];
[endif]
[endfor][endfor]"""

initializer = """  : RTC::DataFlowComponentBase(manager)[if-any port_init],
[for port in port_init]
[if-any port.portType]
[if port.portType is DataInPort]
    m_[port.name]In("[port.name]", m_[port.name])[if-index port is last][else],[endif]

[endif]
[if port.portType is DataOutPort]
    m_[port.name]Out("[port.name]", m_[port.name])[if-index port is last][else],[endif]

[endif]
[else]
    m_[port.name]Port("[port.name]")[if-index port is last][else],[endif]

[endif]
[endfor]
[else][endif]
"""

registration = \
"""  // Set InPort buffers
[for inport in dataPorts][if inport.portType is DataInPort]
  addInPort("[inport.name]", m_[inport.name]In);
[endif][endfor]

  // Set OutPort buffer
[for outport in dataPorts][if outport.portType is DataOutPort]
  addOutPort("[outport.name]", m_[outport.name]Out);
[endif][endfor]

  // Set service provider to Ports
[for service in servicePorts][for interface in service.serviceInterface]
[if interface.direction is Provided]
  m_[service.name]Port.registerProvider("[interface.name]", "[interface.type]", m_[interface.name]);
[endif]
[endfor][endfor]

  // Set service consumers to Ports
[for consumer in servicePorts][for interface in consumer.serviceInterface]
[if interface.direction is Required]
  m_[consumer.name]Port.registerConsumer("[interface.name]", "[interface.type]", m_[interface.name]);
[endif]
[endfor][endfor]

  // Set CORBA Service Ports
[for corbaport in servicePorts]
  addPort(m_[corbaport.name]Port);
[endfor]"""

bind_config = \
"""  // Bind variables and configuration variable
[for config in configurationSet.configuration]
  bindParameter("[config.name]", m_[config.name], "[config.defaultValue]");
[endfor]"""

#------------------------------------------------------------


#============================================================
# Classes and Functions
#============================================================


def CreateSuffix(opts, dict):
	impl_suffix = "SVC_impl"
	skel_suffix = "Skel"
	stub_suffix = "Stub"
	for opt, arg in opts:
		if opt.find("--svc-impl-suffix") == 0:
			impl_suffix = arg
		if opt.find("--svc-skel-suffix") == 0:
			skel_suffix = arg
		if opt.find("--svc-stub-suffix") == 0:
			stub_suffix = arg
	dict["impl_suffix"] = impl_suffix
	dict["skel_suffix"] = skel_suffix
	dict["stub_suffix"] = stub_suffix

def CreateServiceIDL(dict):
	for d in dict["service_idl"]:
		d["impl_basename"] = d["idl_basename"] + dict["impl_suffix"]
		d["impl_h"]        = d["impl_basename"] + ".h"
		d["impl_cpp"]      = d["impl_basename"] + ".cpp"
		d["skel_basename"] = d["idl_basename"] + dict["skel_suffix"]
		d["skel_h"]        = d["skel_basename"] + ".h"
		d["skel_cpp"]      = d["skel_basename"] + ".cpp"
		d["stub_suffix"]   = dict["stub_suffix"]
		d["stub_basename"] = d["idl_basename"] + dict["stub_suffix"]
		d["stub_h"]        = d["stub_basename"] + ".h"
		d["stub_cpp"]      = d["stub_basename"] + ".cpp"

def CreateConsumerIDL(dict):
	conslist = []
	for cons in dict["consumer_idl"]:
		dup = False
		for svc in dict["service_idl"]:
			if cons["idl_fname"] == svc["idl_fname"]:
				dup = True
		if not dup:
			tmp = cons
			tmp["skel_basename"] = tmp["idl_basename"] + \
			    dict["skel_suffix"]
			tmp["skel_h"]        = tmp["skel_basename"] + ".h"
			tmp["skel_cpp"]      = tmp["skel_basename"] + ".cpp"
			tmp["stub_suffix"]   = dict["stub_suffix"]
			tmp["stub_basename"] = tmp["idl_basename"] + \
			    dict["stub_suffix"]
			tmp["stub_h"]        = tmp["stub_basename"] + ".h"
			tmp["stub_cpp"]      = tmp["stub_basename"] + ".cpp"
			conslist.append(tmp)
			


def CreateActivityFuncs(dict):
	acts = (("onFinalize",    ""), \
		("onStartup",     "RTC::UniqueId ec_id"), \
		("onShutdown",    "RTC::UniqueId ec_id"), \
		("onActivated",   "RTC::UniqueId ec_id"), \
		("onDeactivated", "RTC::UniqueId ec_id"), \
		("onExecute",     "RTC::UniqueId ec_id"), \
		("onAborting",    "RTC::UniqueId ec_id"), \
		("onError",       "RTC::UniqueId ec_id"), \
		("onReset",       "RTC::UniqueId ec_id"), \
		("onStateUpdate", "RTC::UniqueId ec_id"), \
		("onRateChanged", "RTC::UniqueId ec_id"))
	actlist = []
	for name, args in acts:
		a = {}
		a["name"] = name
		a["args"] = args
		actlist.append(a)

	dict["activity"] = actlist


def PortInitializer(dict):
	dict["port_init"] = []
	for d in dict["dataPorts"]:
		dict["port_init"].append(d)
	for d in dict["servicePorts"]:
		dict["port_init"].append(d)
	if len(dict["port_init"]) == 0:
		dict.pop("port_init")


class cxx_gen(gen_base.gen_base):
	"""
	C++ component source code generator
	"""
	_fname_space = 16
	def __init__(self, data, opts):
		self.data = data
		self.opts = opts
                CreateSuffix(opts, self.data)
		CreateServiceIDL(self.data)
		CreateConsumerIDL(self.data)
		CreateActivityFuncs(self.data)
		PortInitializer(self.data)
		self.data["rcs_date"] = "$" + "Date" + "$"
		self.data["rcs_id"] = "$" + "Id" + "$"
		self.data["fname"] = self.data["basicInfo"]["name"]
		self.data["fname_h"] = self.data["fname"] + ".h"
		self.data["fname_cpp"] = self.data["fname"] + ".cpp"
		self.data["fname_comp"] = self.data["fname"] + "Comp.cpp"
		self.data["makefile"] = "Makefile." + self.data["fname"]
		self.data["u_name"] = self.data["fname"].upper()
		self.data["l_name"] = self.data["fname"].lower()

		self.tags = {}
                self.tags["service_impl_h"]    = service_impl_h
		self.tags["consumer_stub_h"]   = consumer_stub_h
		self.tags["module_spec"]       = module_spec
		self.tags["config_declare"]    = config_declare
		self.tags["inport_declare"]    = inport_declare
		self.tags["outport_declare"]   = outport_declare
		self.tags["corbaport_declare"] = corbaport_declare
		self.tags["service_declare"]   = service_declare
		self.tags["consumer_declare"]  = consumer_declare
		self.tags["initializer"]       = initializer
		self.tags["registration"]      = registration
		self.tags["bind_config"]       = bind_config

		self.gen_tags(self.tags)
		return


	def print_header(self):
		"""
		Generate component class header
		"""
		self.gen(self.data["fname_h"],
			 comp_header, self.data, self.tags)
		return


	def print_source(self):
		"""
		Generate component class source code
		"""
		self.gen(self.data["fname_cpp"],
			 comp_soruce, self.data, self.tags)
		return


	def print_compsrc(self):
		"""
		Generate component source code
		"""
		self.gen(self.data["fname_comp"],
			 comp_compsrc, self.data, self.tags)
		return


	def print_makefile(self):
		"""
		Generate Makefile
		"""
		self.gen(self.data["makefile"],
			 makefile, self.data, self.tags)

	def print_vcproj(self):
		"""
		Generate vcproj and sln
		"""
		import vcproj_gen
		v = vcproj_gen.vcproj_gen(self.data, self.opts)
		v.print_all()


	def print_impl(self):
		for svc_idl in self.data["service_idl"]:
			fd_h = None
			fd_cpp = None
			lines_h = None
			lines_cpp = None
			if not os.access(svc_idl["idl_fname"], os.F_OK):
				sys.stderr.write("Error: IDL file \"" \
						 + svc_idl["idl_fname"] \
						 + "\" not found.\n")
				sys.exit(1)
			fd_h, lines_h = \
			    self.check_overwrite(svc_idl["impl_h"])
			fd_cpp, lines_cpp = \
			    self.check_overwrite(svc_idl["impl_cpp"])
			if not fd_h:
				sys.stderr.write("Cannot open file:" + 
						 svc_idl["impl_h"] + "\n")
				sys.exit(1)
			if not fd_cpp:
				sys.stderr.write("Cannot open file:" + 
						 svc_idl["impl_cpp"] + "\n")
				sys.exit(1)
			if lines_h or lines_cpp:
				sys.stderr.write("Merge of service impl." +
						 "code is not supported.\n")
				return

			try:
				idl_include = self.data["idl_include"]
				impl_suffix = self.data["impl_suffix"]
				skel_suffix = self.data["skel_suffix"]
				import cxx_svc_impl
				ifs = cxx_svc_impl.generate(svc_idl["idl_fname"],
							    idl_include,
							    impl_suffix,
							    skel_suffix,
							    fd_h, fd_cpp)
				print "  File \"" \
				    + svc_idl["impl_h"] \
				    + "\" was generated."
				print "  File \"" \
				    + svc_idl["impl_cpp"] \
				    + "\" was generated."
			except:
				sys.stderr.write("Generate error: " \
						 + svc_idl["impl_h"]
						 + ", "
						 + svc_idl["impl_cpp"] + "\n")
				
	def print_all(self):
		self.print_impl()
		self.print_header()
		self.print_source()
		self.print_compsrc()
		self.print_makefile()
		self.print_vcproj()
		return


class idl2char:
	def __init__(self, filename = None):
		self.lines = ""
		self.inComment = False
		if filename:
			f = open(filename)
			l = f.readlines()
			self.erase_comments(l)
			f.close()

	def open_file(self, filename):
		f = open(filename)
		l = f.readlines()
		self.erase_comments(l)
		f.close()

	def cleanup():
		self.lines = ""

	def erase_comments(self, lines):
		for l in lines:
			m = re.search("/\*.*\*/", l)
			if m:
				l = l.replace(m.group(), "")
				
			m = re.search(".*\*/", l)
			if m:
				l = l.replace(m.group(), "")
				if self.inComment == True:
					self.inComment = False

			m = re.search("/\*.*$", l)
			if m:
				l = l.replace(m.group(), "")
				if self.inComment == False:
					self.inComment = True

			m = re.search("//.*$", l)
			if m:
				l = l.replace(m.group(), "")

			m = re.search("\s+$", l)
			if m:
				l = l.replace(m.group(), "")
				l = l + "\n"

			m = re.search("\S", l)
			if m:
				if not self.inComment:
					self.lines += l

					
	def get_lines(self):
		return self.lines

	def make_chardata(self, var_name):
		self.lines = self.lines.replace("\n","\\n\\\n")
		self.lines = self.lines.replace("\"","\\\"")
		self.lines = "static char* " + var_name  \
		    + " = \"\\\n" + self.lines
		self.lines += "\";\n"
		return self.lines

	def add_slash(self):
		self.lines = self.lines.replace("\n","\\\n")
		return self.lines

	def write_file(self, filename):
		f = open(filename, "w")
		f.write(self.lines)
		f.close()
        
        
