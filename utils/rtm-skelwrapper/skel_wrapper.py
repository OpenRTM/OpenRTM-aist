#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
#  @file skel_wrapper.py
#  @brief CORBA skelton/stub wrapper generator module
#  @date $Date: 2008-03-06 06:51:10 $
#  @author Noriaki Ando <n-ando@aist.go.jp>
# 
#  Copyright (C) 2004-2011
#      Intelligent Systems Research Institute,
#      National Institute of
#          Advanced Industrial Science and Technology (AIST), Japan
#      All rights reserved.
# 
#  $Id$
# 

from __future__ import print_function

import yat
import re
import sys
import os
import time


#------------------------------------------------------------
# Server Skelton header template
#------------------------------------------------------------
skel_h = """// -*- C++ -*-
/*!
 *
 * THIS FILE IS GENERATED AUTOMATICALLY!! DO NOT EDIT!!
 *
 * @file [skel_h]
 
 * @brief [basename] server skeleton header wrapper code
 * @date [date]
 
 *
 */

#ifndef [skel_h_inc_guard]

#define [skel_h_inc_guard]


[config_inc]

#if   defined ORB_IS_TAO
#  include "[include_dir][basename]C.h"
#  include "[include_dir][basename]S.h"
#elif defined ORB_IS_OMNIORB
[dependencies]
#  if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#    ifdef USE_stub_in_nt_dll
#        undef USE_stub_in_nt_dll
#    endif
#    ifdef [category]_IMPORT_SYMBOL
#        define USE_stub_in_nt_dll
#    endif
#  endif
#  include "[include_dir][basename].hh"
[enable_copydatafunc]#  include "[include_dir][basename]Util.h"
#elif defined ORB_IS_MICO
#  include "[include_dir][basename].h"
#elif defined ORB_IS_ORBIT2
#  include "[include_dir]/[basename]-cpp-stubs.h"
#  include "[include_dir]/[basename]-cpp-skels.h"
#elif defined ORB_IS_RTORB
#  include "[include_dir][basename].h"
#elif defined ORB_IS_ORBEXPRESS
#  include "[include_dir][basename].h"
#else
#  error "NO ORB defined"
#endif

#endif // [skel_h_inc_guard]

"""


#------------------------------------------------------------
# Server Skelton source template
#------------------------------------------------------------
skel_cpp = """// -*- C++ -*-
/*!
 *
 * THIS FILE IS GENERATED AUTOMATICALLY!! DO NOT EDIT!!
 *
 * @file [skel_cpp]
 
 * @brief [basename] server skeleton wrapper
 * @date [date]
 
 *
 */

#include "[skel_h]"

#if defined ORB_IS_TAO
#  include "[include_dir][basename]C.cpp"
#  include "[include_dir][basename]S.cpp"
#elif defined ORB_IS_OMNIORB
#  include "[include_dir][basename]SK.cc"
#  include "[include_dir][basename]DynSK.cc"
[enable_copydatafunc]#  include "[include_dir][basename]Util.cpp"
#elif defined ORB_IS_MICO
#  include "[include_dir][basename].cc"
#  include "[include_dir][basename]_skel.cc"
#elif defined ORB_IS_ORBIT2
#  include "[include_dir][basename]-cpp-stubs.cc"
#  include "[include_dir][basename]-cpp-skels.cc"
#elif defined ORB_IS_RTORB
// [include_openrtm_idl_decls]
#  include "[include_dir][basename]-common.c"
#  include "[include_dir][basename]-skels.c"
#  include "[include_dir][basename]-skelimpl.c"
#elif defined ORB_IS_ORBEXPRESS
#  include "[include_dir][basename].cc"
#  include "[include_dir][basename]_c.cc"
#else
#  error "NO ORB defined"
#endif

// end of [skel_cpp]

"""

#------------------------------------------------------------
# Client Stub header template
#------------------------------------------------------------
stub_h = """// -*- C++ -*-
/*!
 *
 * THIS FILE IS GENERATED AUTOMATICALLY!! DO NOT EDIT!!
 *
 * @file [stub_h]
 
 * @brief [basename] client stub header wrapper code
 * @date [date]
 
 *
 */

#ifndef [stub_h_inc_guard]

#define [stub_h_inc_guard]


[config_inc]

#if   defined ORB_IS_TAO
#  include "[basename]C.h"
#elif defined ORB_IS_OMNIORB
#  if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
[dependencies]
#    ifdef USE_stub_in_nt_dll
#        undef USE_stub_in_nt_dll
#    endif
#    ifdef [category]_IMPORT_SYMBOL
#        define USE_stub_in_nt_dll
#    endif
#  endif
#  include "[include_dir][basename].hh"
#elif defined ORB_IS_MICO
#  include "[include_dir][basename].h"
#elif defined ORB_IS_ORBIT2
#  include "[include_dir][basename]-cpp-stubs.h"
#elif defined ORB_IS_RTORB
#  include "[include_dir][basename].h"
#elif defined ORB_IS_ORBEXPRESS
#  include "[include_dir][basename]_c.h"
#else
#  error "NO ORB defined"
#endif

#endif // [stub_h_inc_guard]

"""
 
#------------------------------------------------------------
# Client Stub soruce template
#------------------------------------------------------------
stub_cpp = """// -*- C++ -*-
/*!
 *
 * THIS FILE IS GENERATED AUTOMATICALLY!! DO NOT EDIT!!
 *
 * @file [stub_cpp]
 
 * @brief [basename] client stub wrapper code
 * @date [date]
 
 *
 */


#include "[stub_h]"

#if   defined ORB_IS_TAO
#  include "[include_dir][basename]C.cpp"
#elif defined ORB_IS_OMNIORB
#  include "[include_dir][basename]SK.cc"
#  include "[include_dir][basename]DynSK.cc"
#elif defined ORB_IS_MICO
#  include "[include_dir][basename].cc"
#elif defined ORB_IS_ORBIT2
#  include "[include_dir][basename]-cpp-stubs.cc"
#elif defined ORB_IS_RTORB
// [include_openrtm_idl_decls]
#  include "[include_dir][basename]-common.c"
#  include "[include_dir][basename]-stubs.c"
#elif defined ORB_IS_ORBEXPRESS
#  include "[include_dir][basename]_c.cc"
#else
#  error "NO ORB defined"
#endif

// end of [stub_cpp]

"""


config_inc = """
#include <[rtm_dir]config_rtc.h>
#undef PACKAGE_BUGREPORT
#undef PACKAGE_NAME
#undef PACKAGE_STRING
#undef PACKAGE_TARNAME
#undef PACKAGE_VERSION
"""

class skel_wrapper:
	def __init__(self, idl_fname, skel_suffix = "Skel",
		     stub_suffix = "Stub", include_dir = "",
		     config_inc = "", output_dir = "./", category = "OpenRTM", dependencies="", verbose=True):
		self.data = {}
		self.data["include_dir"] = include_dir
		self.data["output_dir"] = output_dir
		self.data["idl_fname"] = idl_fname
		self.data["category"] = category
		if not dependencies:
			self.data["dependencies"] = ""
		else:
			includes = ["#    include<"+s+"Skel.h>\n" for s in dependencies.split(":")]
			self.data["dependencies"] = ''.join(includes)
		self.verbose_print = print if verbose else lambda *a, **k: None

		m = re.search("\\.[iI][dD][lL]$", idl_fname)
		if m:
			basename = idl_fname.replace(m.group(0), "")
		else:
			sys.stderr.write("Invalid IDL file name specified.\n")
			sys.exit(1)
			
		dirname  = os.path.dirname(basename)
		if dirname:
			dirname += "/"
		basename = os.path.basename(basename)
		self.data["basename"] = basename
		self.data["skel_h"]   = basename + skel_suffix + ".h"
		self.data["skel_cpp"] = basename + skel_suffix + ".cpp"
		self.data["stub_h"]   = basename + stub_suffix + ".h"
		self.data["stub_cpp"] = basename + stub_suffix + ".cpp"

		skel_h_guard = dirname + self.data["skel_h"].replace(".","_")
		skel_h_guard = skel_h_guard.replace("/","_")
		count = 0
		for i in range(len(skel_h_guard)):
			if skel_h_guard[i] != "_":
				count = i
				break
		skel_h_guard = skel_h_guard[count:]
		skel_h_guard = skel_h_guard.upper()
		stub_h_guard = dirname + self.data["stub_h"].replace(".","_")
		stub_h_guard = stub_h_guard.replace("/","_")
		count = 0
		for i in range(len(stub_h_guard)):
			if stub_h_guard[i] != "_":
				count = i
				break
		stub_h_guard = stub_h_guard[count:]
		stub_h_guard = stub_h_guard.upper()
		self.data["skel_h_inc_guard"] = skel_h_guard
		self.data["stub_h_inc_guard"] = stub_h_guard
		
		self.data["date"] = time.ctime()
		self.data["config_inc"] = config_inc
		if basename == "OpenRTM-aist" :
			self.data["include_openrtm_idl_decls"] = ""
		else:
			self.data["include_openrtm_idl_decls"] = "#  include \"OpenRTM-aist-decls.h\"\n"
		if os.path.exists(os.path.join(include_dir, basename+"Util.cpp")) and os.path.exists(os.path.join(include_dir, basename+"Util.h")):
			self.data["enable_copydatafunc"] = ""
		else:
			self.data["enable_copydatafunc"] = "//"
		return

	def gen(self, fname, temp_txt):
		data = self.data
		t = yat.Template(temp_txt)
		text = t.generate(data)

		if os.access(fname, os.F_OK): # file exists
			f = open(fname, "r")
			oldtext = f.read()
			f.close()

			newtext = re.sub(" \\@date.*?\n", "", text)
			oldtext2 = re.sub(" \\@date.*?\n", "", oldtext)
			if newtext == oldtext2:
				self.verbose_print("\"" + fname + \
			    "\" exists and contents is same.")
				self.verbose_print("No need to generate the file.")
				return
			else:
				self.verbose_print("\"", fname, \
			    "\" already exists but contents are not same")

		f = open(fname, "w")
		f.write(text)
		f.close()
		self.verbose_print("\"" + fname + "\"" " was generated.")
		return

	def print_skel_h(self):
		fname = self.data["output_dir"] + self.data["skel_h"]
		self.gen(fname, skel_h)
		return

	def print_skel_cpp(self):
		fname = self.data["output_dir"] + self.data["skel_cpp"]
		self.gen(fname, skel_cpp)
		return

	def print_stub_h(self):
		fname = self.data["output_dir"] + self.data["stub_h"]
		self.gen(fname, stub_h)
		return

	def print_stub_cpp(self):
		fname = self.data["output_dir"] + self.data["stub_cpp"]
		self.gen(fname, stub_cpp)
		return

	def print_all(self):
		self.print_skel_h()
		self.print_skel_cpp()
		self.print_stub_h()
		self.print_stub_cpp()
		return

	def generate(self):
		self.print_all()


if __name__ == "__main__":
	w = skel_wrapper("Hoge.idl", "_Skel", "_Stub", "rtm/idl/", "")
	w.generate()
