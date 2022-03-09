// -*- C++ -*-
/*!
 * @file DefaultConfiguration.h
 * @brief RTC manager default configuration
 * @date $Date: 2007-12-31 03:08:03 $
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2006-2010
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
#ifndef RTM_DEFAULTCONFIGURATION_H
#define RTM_DEFAULTCONFIGURATION_H
#include "rtm/version.h"
#include "rtm/config_rtc.h"
/*!
 * @if jp
 * @brief Manager 用 デフォルト・コンフィギュレーション
 *
 * Managerクラス用デフォルトコンフィギュレーション。
 *
 * @since 0.4.0
 *
 * @else
 * @brief Default configuration for Manager
 *
 * Default configuration for Manager class
 *
 * @since 0.4.0
 *
 * @endif
 */
namespace RTC {
  const char* const default_config[] =
  {
    "config.version",                        openrtm_version,
    "openrtm.name",                          openrtm_name,
    "openrtm.version",                       openrtm_version,
    "manager.instance_name",                 "manager",
    "manager.name",                          "manager",
    "manager.naming_formats",                "%h.host_cxt/%n.mgr",
    "manager.pid",                           "",
    "os.name",                               "",
    "os.release",                            "",
    "os.version",                            "",
    "os.arch",                               "",
    "os.hostname",                           "",
    "logger.enable",                         "YES",
    "logger.file_name",                      "./rtc%p.log",
    "logger.date_format",                    "%b %d %H:%M:%S.%Q",
    "logger.log_level",                      "INFO",
    "logger.stream_lock",                    "NO",
    "logger.master_logger",                  "",
    "logger.escape_sequence_enable",         "NO",
    "module.conf_path",                      "",
    "module.load_path",                      "",
    "naming.enable",                         "YES",
    "naming.type",                           "corba",
    "naming.formats",                        "%h.host_cxt/%n.rtc",
    "naming.update.enable",                  "YES",
    "naming.update.interval",                "10.0",
    "timer.enable",                          "YES",
    "timer.tick",                            "0.1",
#ifdef ORB_IS_OMNIORB
    "corba.args",                            "-ORBclientCallTimeOutPeriod 10000",
    "corba.endpoints", "all",
#else
    "corba.args",                            "",
    "corba.endpoints", "",
#endif
    "corba.endpoint",                        "",  // hostname:port_number
    "corba.id",                              corba_name,
    "corba.nameservers",                     "localhost",
    "corba.master_manager",                  "localhost:2810",
    "corba.nameservice.replace_endpoint",    "NO",
    "corba.update_master_manager.enable",    "YES",
    "corba.update_master_manager.interval",  "10.0",
    "exec_cxt.periodic.type",                "PeriodicExecutionContext",
    "exec_cxt.periodic.rate",                "1000",
    "exec_cxt.sync_transition",              "YES",
    "exec_cxt.transition_timeout",           "0.5",
    "manager.modules.load_path",             "./",
    "manager.modules.abs_path_allowed",      "YES",
    "manager.is_master",                     "NO",
    "manager.corba_servant",                 "YES",
    "manager.shutdown_on_nortcs",            "YES",
    "manager.shutdown_auto",                 "YES",
    "manager.auto_shutdown_duration",        "20.0",
    "manager.termination_waittime",          "0.5",
    "manager.name",                          "manager",
    "manager.components.naming_policy",      "process_unique",
    "manager.command",                       "rtcd",
    "manager.nameservers",                     "default",
    "manager.language",                      "C++",
#ifdef WIN32
    "manager.supported_languages",           "C++, Python, Java",
#else
    "manager.supported_languages",           "C++, Python, Python3, Java",
#endif
    "manager.modules.C++.manager_cmd",       "rtcd",
    "manager.modules.C++.profile_cmd",       "rtcprof",
#ifdef WIN32
    "manager.modules.C++.suffixes",           "dll",
#else
#ifdef RTM_OS_DARWIN
    "manager.modules.C++.suffixes",           "dylib",
#else
    "manager.modules.C++.suffixes",           "so",
#endif
#endif
    "manager.modules.C++.load_paths",        "",
    "manager.modules.Python.manager_cmd",    "rtcd_python",
    "manager.modules.Python.profile_cmd",    "rtcprof_python",
    "manager.modules.Python.suffixes",       "py",
    "manager.modules.Python.load_paths",     "",
#ifndef WIN32
    "manager.modules.Python3.manager_cmd",    "rtcd_python3",
    "manager.modules.Python3.profile_cmd",    "rtcprof_python3",
    "manager.modules.Python3.suffixes",       "py",
    "manager.modules.Python3.load_paths",     "",
#endif
    "manager.modules.Java.manager_cmd",      "rtcd_java",
    "manager.modules.Java.profile_cmd",      "rtcprof_java",
    "manager.modules.Java.suffixes",         "class",
    "manager.modules.Java.load_paths",       "",
    "manager.modules.search_auto",       "YES",
    "manager.preload.modules",       "",
    "manager.components.precreate",       "",
    "manager.components.preconnect",       "",
    "manager.components.preactivation",       "",
    "manager.local_service.enabled_services","ALL",
    "sdo.service.provider.enabled_services",  "ALL",
    "sdo.service.consumer.enabled_services",  "ALL",
    ""
  };
} // namespace RTC
#endif  // RTM_DEFAULTCONFIGURATION_H
