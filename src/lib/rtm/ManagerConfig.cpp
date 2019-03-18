// -*- C++ -*-
/*!
 * @file ManagerConfig.cpp
 * @brief RTC manager configuration
 * @date $Date: 2007-12-31 03:08:04 $
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2003-2008
 *     Noriaki Ando
 *     Task-intelligence Research Group,
 *     Intelligent Systems Research Institute,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id$
 *
 */

#include <rtm/RTC.h>
#include <rtm/ManagerConfig.h>
#include <coil/OS.h>
#include <coil/stringutil.h>
#include <rtm/DefaultConfiguration.h>
#include <cstdlib>

#include <fstream>
#include <iostream>

#ifdef __QNX__
using std::sprintf;
#endif

namespace RTC
{

  // The list of default configuration file path.
#ifdef RTM_OS_WIN32
  const char* ManagerConfig::config_file_path[] = 
    {
      "./rtc.conf",
      "${RTM_ROOT}bin/${RTM_VC_VERSION}/rtc.conf",
      NULL
    };
#else
  const char* ManagerConfig::config_file_path[] = 
    {
      "./rtc.conf",
      "/etc/rtc.conf",
      "/etc/rtc/rtc.conf",
      "/usr/local/etc/rtc.conf",
      "/usr/local/etc/rtc/rtc.conf",
      nullptr
    };
#endif
  
  // Environment value to specify configuration file
  const char* ManagerConfig::config_file_env = "RTC_MANAGER_CONFIG";

  /*!
   * @if jp
   * @brief コンストラクタ
   * @else
   * @brief Constructor
   * @endif
   */
  ManagerConfig::ManagerConfig()
    : m_isMaster(false)
  {
  }

  /*!
   * @if jp
   * @brief コンストラクタ
   * @else
   * @brief Constructor
   * @endif
   */
  ManagerConfig::ManagerConfig(int argc, char** argv)
    : m_isMaster(false)
  {
    init(argc, argv);
  }

  /*!
   * @if jp
   * @brief デストラクタ
   * @else
   * @brief Destructor
   * @endif
   */
  ManagerConfig::~ManagerConfig()
  {
  }

  /*!
   * @if jp
   * @brief 初期化
   * @else
   * @brief Initialization
   * @endif
   */
  void ManagerConfig::init(int argc, char** argv)
  {
    parseArgs(argc, argv);
  }

  /*!
   * @if jp
   * @brief Configuration 情報を Property に設定する
   * @else
   * @brief Specify the configuration information to the Property
   * @endif
   */
  void ManagerConfig::configure(coil::Properties& prop)
  {
    prop.setDefaults(default_config);
    if (findConfigFile())
      {
        std::ifstream f(m_configFile.c_str());
        if (f.is_open())
          {
            prop.load(f);
            f.close();
          }
      }
    setSystemInformation(prop);
    if (m_isMaster) { prop["manager.is_master"] = "YES"; }
    
    // Properties from arguments are marged finally
    prop << m_argprop;
    prop["config_file"] = m_configFile;
  }

  /*!
   * @if jp
   * @brief コマンド引数をパースする
   * @else
   * @brief Parse the command arguments
   * @endif
   */
  void ManagerConfig::parseArgs(int argc, char** argv)
  {
	bool ignoreNoConf(false);
    coil::GetOpt get_opts(argc, argv, "af:io:p:d", 0);
    int opt;



    while ((opt = get_opts()) > 0)
      {
        switch (opt)
          {
          case 'a':
            {
              m_argprop["manager.corba_servant"] = "NO";
            }
            break;
            // Specify configuration file not default
          case 'f':
            if (!fileExist(get_opts.optarg))
              {
                std::cerr << "Configuration file: " << m_configFile;
                std::cerr << " not found." << std::endl;
#ifndef __QNX__
                for (int i(0); i < argc; ++i)
                  {
                    std::string tmp(argv[i]);
                    if (tmp == "-i") { ignoreNoConf = true; }
                  }
                if (!ignoreNoConf) { exit(-1); }
#endif
              }
            m_configFile = get_opts.optarg;
            break;
            case 'i':
              ignoreNoConf = true;
              break;
          case 'o':
            {
              std::string opt_arg(get_opts.optarg);
              std::string::size_type pos(opt_arg.find(':'));
              if (pos != std::string::npos)
                {
                  std::string key = opt_arg.substr(0, pos);
                  std::string value = opt_arg.substr(pos + 1);

                  key = coil::unescape(key);
                  coil::eraseHeadBlank(key);
                  coil::eraseTailBlank(key);

                  value = coil::unescape(value);
                  coil::eraseHeadBlank(value);
                  coil::eraseTailBlank(value);

                  m_argprop[key] = value;
                }
            }
            break;
          case 'p':  // ORB's port number
            {
              int portnum(2810);
              if (coil::stringTo(portnum, get_opts.optarg))
                {
                  std::string arg(":"); arg += get_opts.optarg;
                  m_argprop["corba.endpoints"]  = arg;
                }
            }
            break;
          case 'd':
            m_isMaster = true;
            break;
          default:
            break;
          }
      }
    return;
  }

  /*!
   * @if jp
   * @brief Configuration file の検索
   * @else
   * @brief Find the configuration file
   * @endif
   */
  bool ManagerConfig::findConfigFile()
  {
    // Check existance of configuration file given command arg
    if (!m_configFile.empty())
      {
        if (fileExist(m_configFile))
          {
            return true;
          }
        std::cerr << "Configuration file: " << m_configFile;
        std::cerr << " not found." << std::endl;
      }
    // Search rtc configuration file from environment variable
    char* env = coil::getenv(config_file_env);
    if (env != nullptr)
      {
        if (fileExist(env))
          {
            m_configFile = env;
            return true;
          }
        std::cerr << "Configuration file: " << m_configFile;
        std::cerr << " not found." << std::endl;
      }
    // Search rtc configuration file from default search path
    int i = 0;
    while (config_file_path[i] != nullptr)
      {
        std::string cpath = coil::replaceEnv(config_file_path[i]);

        if (fileExist(cpath))
          {
            m_configFile = cpath;
            return true;
          }
        ++i;
      }
    return false;
  }

  /*!
   * @if jp
   * @brief システム情報を設定する
   * @else
   * @brief Set system information
   * @endif
   */
  void ManagerConfig::setSystemInformation(coil::Properties& prop)
  {
    //
    // Get system information by using ACE_OS::uname (UNIX/Windows)
    //
    coil::utsname  sysinfo;
    if (coil::uname(&sysinfo) != 0)
      {
        return;
      }

    //
    // Getting current proccess pid by using ACE_OS::getpid() (UNIX/Windows)
    //
    coil::pid_t pid = coil::getpid();
    std::string pidc = coil::otos(pid);

    prop.setProperty("os.name",     sysinfo.sysname);
    prop.setProperty("os.release",  sysinfo.release);
    prop.setProperty("os.version",  sysinfo.version);
    prop.setProperty("os.arch",     sysinfo.machine);
    prop.setProperty("os.hostname", sysinfo.nodename);
    prop.setProperty("manager.pid", pidc);

    return;
  }

  /*!
   * @if jp
   * @brief ファイルの存在確認
   * @else
   * @brief Check the file existence
   * @endif
   */
  bool ManagerConfig::fileExist(const std::string& filename)
  {
    std::ifstream infile;
    infile.open(filename.c_str(), std::ios::in);
    // fial() 0: ok, !0: fail
    if (static_cast<int>(infile.fail()) != 0)
      {
        infile.close();
        return false;
      }
    else
      {
        infile.close();
        return true;
      }
    return false;
  }
} // namespace RTC
