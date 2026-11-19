// -*- C++ -*-
/*!
 * @file rtcprof.cpp
 * @brief RT-Component profile dump command
 * @date $Date$
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2010
 *     Noriaki Ando
 *     Intelligent Systems Research Institute,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id$
 *
 */

#include <iterator>
#include <iostream>
#include <string>
#include <vector>

#include <coil/Properties.h>
#include <coil/File.h>

#include <rtm/Manager.h>


int main(int argc, char* argv[])
{
  if (argc != 2)
    {
      std::cerr << "usage: " << std::endl;
      std::cerr << argv[0] << " .so or .DLL" << std::endl;
      std::cerr << std::endl;
      return -1;
    }

  //
  // notice: coil::dirname brakes original string
  // 
  // file name with full path
  std::string fullname{coil::replaceString(*std::next(argv), "\\", "/")};
  // directory name
  std::string dirname(coil::dirname(const_cast<char*>(fullname.c_str())));
  // basename
  std::string basename(coil::basename(fullname.c_str()));

  // making command line option
  //   dummy -o manager.modules.load_path
  coil::vstring opts;
  opts.emplace_back("dummy");
  opts.emplace_back("-o");
  std::string load_path("manager.modules.load_path:");
  load_path += dirname;
  opts.emplace_back(load_path);
  opts.emplace_back("-o");
  opts.emplace_back("logger.enable:NO");
  opts.emplace_back("-o");
  opts.emplace_back("manager.corba_servant:NO");

  // Manager initialization
  RTC::Manager::init(static_cast<int>(opts.size()), coil::Argv(opts).get());
  RTC::Manager& mgr(RTC::Manager::instance());


  // loaded profile = old profiles - new profiles
  std::vector<coil::Properties> oldp(mgr.getFactoryProfiles());
  mgr.load(basename, "");
  std::vector<coil::Properties> newp(mgr.getFactoryProfiles());
  std::vector<coil::Properties> profs;

  for (auto & propnew : newp)
    {
      bool exists(false);
      for (auto & propold : oldp)
        {
          if (propold["implementation_id"] == propnew["implementation_id"] &&
              propold["type_name"]         == propnew["type_name"] &&
              propold["description"]       == propnew["description"] &&
              propold["version"]           == propnew["version"])
            {
              exists = true;
            }
        }
      if (!exists) { profs.emplace_back(std::move(propnew)); }
    }

  // loaded component profile have to be one
  if (profs.empty())
    {
      std::cerr << "Load failed." << std::endl;
      return -1;
    }

  if (profs.size() > 1)
    {
      std::cerr << "One ore more modules loaded." << std::endl;
      return -1;
    }

  coil::vstring keys(profs[0].propertyNames());

  for (const auto & key : keys)
    {
      std::cout << key << ": " << profs[0][key] << std::endl;
    }

  return 0;
}
