// -*- C++ -*-
/*!
 * @file OS_win32.h
 * @brief OS class
 * @date $Date$
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2008
 *     Task-intelligence Research Group,
 *     Intelligent Systems Research Institute,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id$
 *
 */

#ifndef COIL_OS_H
#define COIL_OS_H


#include <windows.h>
#include <string.h>
#include <stdio.h>
#include <process.h>
#include <stdlib.h>
#include <winbase.h>
#include <iostream>
#include <map>


extern "C"
{
  extern char *optarg;
};

namespace coil
{

#define COIL_UTSNAME_LENGTH 65
  struct utsname
  {
    char sysname[COIL_UTSNAME_LENGTH];
    char nodename[COIL_UTSNAME_LENGTH];
    char release[COIL_UTSNAME_LENGTH];
    char version[COIL_UTSNAME_LENGTH];
    char machine[COIL_UTSNAME_LENGTH];
  };

  class osversion
  {
  public:
      osversion() : major(0), minor(0)
      {

      }
      osversion(const osversion& obj) : major(obj.major), minor(obj.minor)
      {

      }
      osversion(DWORD majar_version,DWORD minor_version) : major(majar_version), minor(minor_version)
      {
      }
      DWORD major;
      DWORD minor;
  };

  /*!
  * @if jp
  *
  * @brief RtlGetVersion APIによりOSの情報取得
  * RtlGetVersion関数によりOSのメジャーバージョン、マイナーバージョン、
  * ビルドナンバー、サービスパックの情報を取得する
  * RtlGetVersion関数アドレス取得失敗、OSの情報取得失敗の場合は-1を返す
  * この関数はrtlgetinfo関数で使用するためのものであり、基本的に外部からは使用しない
  *
  *
  * @param name 構造体名称
  *
  * @return 0: 成功, -1: 失敗
  *
  * @else
  *
  * @brief Obtain OS information using RtlGetVersion API
  * Get major version, minor version, build number, service pack information of OS by RtlGetVersion function
  * RtlGetVersion function Returns -1 in the case of address acquisition failure, OS information acquisition failure.
  *
  * @param name Name of structure
  *
  * @return 0: Successful, -1: failed
  *
  * @endif
  */
  inline bool rtlgetinfo(utsname* name)
  {
      HMODULE handle = GetModuleHandle("ntdll.dll");

      if (handle)
      {
          typedef LONG(WINAPI* RtlGetVersionFunc)(PRTL_OSVERSIONINFOW lpVersionInformation);

          RTL_OSVERSIONINFOW version_info;
          RtlGetVersionFunc RtlGetVersion = (RtlGetVersionFunc)GetProcAddress(handle, "RtlGetVersion");
          if (RtlGetVersion != nullptr)
          {
              if (RtlGetVersion(&version_info) == 0)
              {
                  const char *os;
                  if (version_info.dwPlatformId == VER_PLATFORM_WIN32_NT)
                  {
                      os = "Windows NT %d.%d";
                  }
                  else
                  {
                      os = "Windows CE %d.%d";
                  }

                  sprintf_s(name->release, sizeof(name->release), os,
                      static_cast<int>(version_info.dwMajorVersion),
                      static_cast<int>(version_info.dwMinorVersion));

                  sprintf_s(name->version, sizeof(name->release), "Build %d %ls",
                      static_cast<int>(version_info.dwBuildNumber),
                      version_info.szCSDVersion);

                  return true;
              }
          }
          FreeLibrary(handle);
      }
      return false;
  }

  /*!
   * @if jp
   *
   * @brief システム情報取得
   *
   * システム情報を構造体に設定して返す。
   * OSのバージョンはRtlGetVersionにより取得する
   * RtlGetVersionはRTL_OSVERSIONINFOWにOSのメジャーバージョン、
   * マイナーバージョン、ビルドナンバー、サービスパックの情報などを格納する
   * ただし、ライブラリ(ntdll.dll)が存在せず利用できない場合は
   * VerifyVersionInfoによるバージョン比較を行う。
   * VerifyVersionInfoは設定したOSのバージョンと指定のバージョンを比較する関数である。
   * VerifyVersionInfoを使う場合はビルドナンバーは設定されない
   *
   * @param name 構造体名称
   *
   * @return 0: 成功, -1: 失敗
   *
   * @else
   *
   * @brief Get System information
   *
   * Return a system information to a structure.
   * Set system information to structure and return it.
   * Obtain version of OS by RtlGetVersion function.
   * RtlGetVersion function stores the major version, minor version, build number, service pack information etc of OS in RTL_OSVERSIONINFOW.
   * However, if the library (ntdll.dll) does not exist and can not be used, version comparison by VerifyVersionInfo function is performed.
   * VerifyVersionInfo function is a function to compare the version of the specified OS with the specified version.
   * When using VerifyVersionInfo function, the build number is not set.
   *
   * @param name Name of structure
   *
   * @return 0: Successful, -1: failed
   *
   * @endif
   */
  inline int uname(utsname* name)
  {
    if (name == NULL) return 0;
    int ret(0);

    // name.sysname
    ::strcpy_s(name->sysname, sizeof(name->sysname), "Win32");


    
    if(!rtlgetinfo(name))
    {

        std::map<std::string, osversion> oslist;
        oslist["Windows 2000"] = osversion(5, 0);
        oslist["Windows XP 32bit"] = osversion(5, 1);
        oslist["Windows XP 64bit"] = osversion(5, 2);
        oslist["Windows Vista"] = osversion(6, 0);
        oslist["Windows 7"] = osversion(6, 1);
        oslist["Windows 8"] = osversion(6, 2);
        oslist["Windows 8.1"] = osversion(6, 3);
        oslist["Windows 10"] = osversion(10, 0);

        for(auto & o : oslist)
        {
            // name.release, name.version
            OSVERSIONINFOEX version_info;
            ULONGLONG condition = 0;
            version_info.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
            version_info.dwMajorVersion = o.second.major;
            version_info.dwMinorVersion = o.second.minor;
            VER_SET_CONDITION(condition, VER_MAJORVERSION, VER_EQUAL);
            VER_SET_CONDITION(condition, VER_MINORVERSION, VER_EQUAL);

            if (::VerifyVersionInfo(&version_info, VER_MAJORVERSION | VER_MINORVERSION, condition))
            {
                const char *os;
                if (version_info.dwPlatformId == VER_PLATFORM_WIN32_NT)
                {
                    os = "Windows NT %d.%d";
                }
                else
                {
                    os = "Windows CE %d.%d";
                }

                sprintf_s(name->release, sizeof(name->release), os,
                    static_cast<int>(version_info.dwMajorVersion),
                    static_cast<int>(version_info.dwMinorVersion));

                break;
            }


        }
    }


    // name.machine
    SYSTEM_INFO sys_info;
    GetSystemInfo(&sys_info);
    WORD arch = sys_info.wProcessorArchitecture;
    char cputype[COIL_UTSNAME_LENGTH/2];
    char subtype[COIL_UTSNAME_LENGTH/2];

    switch (arch)
      {
      case PROCESSOR_ARCHITECTURE_INTEL:
        strcpy_s(cputype, sizeof(cputype), "x86 Intel");
        if (sys_info.wProcessorLevel == 3)
          strcpy_s(subtype, sizeof(subtype), "80386");
        else if (sys_info.wProcessorLevel == 4)
          strcpy_s(subtype, sizeof(subtype), "80486");
        else if (sys_info.wProcessorLevel == 5)
          strcpy_s(subtype, sizeof(subtype), "Pentium");
        else if (sys_info.wProcessorLevel == 6)
          strcpy_s(subtype, sizeof(subtype), "Pentium Pro");
        else if (sys_info.wProcessorLevel == 7)
          strcpy_s(subtype, sizeof(subtype), "Pentium II");
        else
          strcpy_s(subtype, sizeof(subtype), "Pentium Family");
        break;
      case PROCESSOR_ARCHITECTURE_MIPS:
          strcpy_s(cputype, sizeof(cputype), "MIPS");
          strcpy_s(subtype, sizeof(subtype), "");
          break;
      case PROCESSOR_ARCHITECTURE_ALPHA:
          strcpy_s(cputype, sizeof(cputype), "ALPHA");
          strcpy_s(subtype, sizeof(subtype), "");
          break;
      case PROCESSOR_ARCHITECTURE_PPC:
          strcpy_s(cputype, sizeof(cputype), "PPC");
          strcpy_s(subtype, sizeof(subtype), "");
          break;
      case PROCESSOR_ARCHITECTURE_SHX:
          strcpy_s(cputype, sizeof(cputype), "SHX");
          strcpy_s(subtype, sizeof(subtype), "");
          break;
      case PROCESSOR_ARCHITECTURE_ARM:
          strcpy_s(cputype, sizeof(cputype), "ARM");
          strcpy_s(subtype, sizeof(subtype), "");
          break;
      case PROCESSOR_ARCHITECTURE_IA64:
          strcpy_s(cputype, sizeof(cputype), "IA64");
          strcpy_s(subtype, sizeof(subtype), "");
          break;
      case PROCESSOR_ARCHITECTURE_ALPHA64:
          strcpy_s(cputype, sizeof(cputype), "ALPHA64");
          strcpy_s(subtype, sizeof(subtype), "");
          break;
      case PROCESSOR_ARCHITECTURE_MSIL:
          strcpy_s(cputype, sizeof(cputype), "MSIL");
          strcpy_s(subtype, sizeof(subtype), "");
          break;
      case PROCESSOR_ARCHITECTURE_AMD64:
          if (sys_info.wProcessorLevel == 6)
          {
              strcpy_s(cputype, sizeof(cputype), "x64 Intel");
              strcpy_s(subtype, sizeof(subtype), "Pentium Pro");
          }
          else
          {
              strcpy_s(cputype, sizeof(cputype), "x64(AMD or Intel)");
              strcpy_s(subtype, sizeof(subtype), "");
          }
          break;
      case PROCESSOR_ARCHITECTURE_IA32_ON_WIN64:
          strcpy_s(cputype, sizeof(cputype), "IA32_ON_WIN64");
          strcpy_s(subtype, sizeof(subtype), "");
          break;
      case PROCESSOR_ARCHITECTURE_NEUTRAL:
          strcpy_s(cputype, sizeof(cputype), "NEUTRAL");
          strcpy_s(subtype, sizeof(subtype), "");
          break;
#ifdef PROCESSOR_ARCHITECTURE_ARM64
      case PROCESSOR_ARCHITECTURE_ARM64:
          strcpy_s(cputype, sizeof(cputype), "ARM64");
          strcpy_s(subtype, sizeof(subtype), "");
          break;
#endif
#ifdef PROCESSOR_ARCHITECTURE_ARM32_ON_WIN64
      case PROCESSOR_ARCHITECTURE_ARM32_ON_WIN64:
          strcpy_s(cputype, sizeof(cputype), "ARM32_ON_WIN64");
          strcpy_s(subtype, sizeof(subtype), "");
          break;
#endif
#ifdef PROCESSOR_ARCHITECTURE_IA32_ON_ARM64
      case PROCESSOR_ARCHITECTURE_IA32_ON_ARM64:
          strcpy_s(cputype, sizeof(cputype), "IA32_ON_ARM64");
          strcpy_s(subtype, sizeof(subtype), "");
          break;
#endif
      default:
        strcpy_s(cputype, sizeof(cputype), "Unknown");
        strcpy_s(subtype, sizeof(subtype), "Unknown");
      }
    sprintf_s(name->machine, sizeof(name->machine), "%s %s", cputype, subtype);

    // name.nodename
    DWORD len = COIL_UTSNAME_LENGTH;
    if (GetComputerNameExA(ComputerNameDnsHostname,
                            name->nodename,
                            &len) == false)
    ret = -1;
    return ret;
  }

  /*!
   * @if jp
   *
   * @brief 呼び出し元プロセスのプロセスID取得
   *
   * 呼び出し元プロセスのプロセスIDを返す。
   *
   * @return プロセスID
   *
   * @else
   *
   * @brief Get process ID of the caller process
   *
   * Return a process ID of the caller process.
   *
   * @return Process ID
   *
   * @endif
   */
  typedef int pid_t;
  inline pid_t getpid()
  {
    return ::_getpid();
  }

  /*!
   * @if jp
   *
   * @brief 呼び出し元プロセスの親プロセスのプロセスID取得
   *
   * 呼び出し元プロセスの親プロセスのプロセスIDを返す。
   *
   * @return 0
   *
   * @else
   *
   * @brief Get process ID of the parent process
   *
   * Return a process ID of the parent process.
   *
   * @return 0
   *
   * @endif
   */
  inline pid_t getppid()
  {
    return 0;
  }

  /*!
   * @if jp
   *
   * @brief 環境変数取得
   *
   * 環境変数を返す。
   *
   * @param name 環境変数名称
   *
   * @return 環境変数の値(NULL: 該当なし)
   *
   * @else
   *
   * @brief Get environment variable
   *
   * Return a environment variable.
   *
   * @param name Name of environment variable
   *
   * @return Value of environment variable(NULL: nonexistent)
   *
   * @endif
   */
  inline char* getenv(const char* name)
  {
    size_t return_size;
    ::getenv_s(&return_size, NULL, 0, name);
    if (return_size == 0)
    {
        return nullptr;
    }
    char *buff = new char[return_size * sizeof(char)];
    ::getenv_s(&return_size, buff, return_size, name);
    return buff;
  }

  static int    opterr = 1,     /* if error message should be printed */
                optind = 1,     /* index into parent argv vector */
                optopt,         /* character checked for validity */
                optreset;       /* reset getopt */
  static char   *optarg;        /* argument associated with option */

#define  BADCH  (int)'?'
#define  BADARG  (int)':'
#define  EMSG  ""

  /*!
   * @if jp
   *
   * @brief コマンドライン引数解析関数
   *
   * コマンドライン引数を解析する。
   *
   * @return 解析結果
   *
   * @else
   *
   * @brief Function of parses the command line arguments
   *
   * Parses the command line arguments.
   *
   * @return Result of parses.
   *
   * @endif
   */
  static int getopt(int nargc, char * const *nargv, const char *ostr)
  {
    static char *place = EMSG;        /* option letter processing */
    const char *oli;            /* option letter list index */
    char *p;

    if (optreset || !*place) {        /* update scanning pointer */
      optreset = 0;
      /* Check that the scanning reached the last element. */
      if (optind >= nargc)
      {
        place = EMSG;
        return(-1);
      }
      for (;;)
      {
        /* Check that the scanning reached the last element. */
        if (optind >= nargc)
        {
          place = EMSG;
          return(-1);
        }
        place = nargv[optind];
        /* Check that the first character of the element is '-'. */
        if (*place != '-')
        {
          /* When the head of the element is not '-',        */
          /* Scan the next element disregarding the element. */
          optind++;

        }
        else
        {
          break;
        }
      }
      if (place[1] && *++place == '-') {    /* found "--" */
        ++optind;
        place = EMSG;
        return(-1);
      }
    }                    /* option letter okay? */
    optopt = static_cast<int>(*place++);
    oli = strchr(ostr, optopt);
    if (optopt == static_cast<int>(':') ||
        oli == nullptr) {
      /*
       * if the user didn't specify '-' as an option,
       * assume it means -1 (EOF).
       */
      if (optopt == static_cast<int>('-'))
        return(-1);
      if (!*place)
        ++optind;
      if (opterr && *ostr != ':') {
        p = strrchr(*nargv, '/');
        if (p == nullptr)
          p = *nargv;
        else
          ++p;
        fprintf(stderr, "%s: illegal option -- %c\n",
                p, optopt);
      }
      return(BADCH);
    }
    if (*++oli != ':')             /* don't need argument */
      {
        optarg = NULL;
        if (!*place)
          ++optind;
      }
    else
      {                    /* need an argument */
        if (*place)            /* no white space */
          {
            optarg = place;
          }
        else if (nargc <= ++optind)
          {    /* no arg */
            place = EMSG;
            p = strrchr(*nargv, '/');
            if (p == nullptr)
              {
                p = *nargv;
              }
            else
              {
                ++p;
              }
            if (*ostr == ':')
              {
                return(BADARG);
              }
            if (opterr)
              {
                fprintf(stderr,
                      "%s: option requires an argument -- %c\n",
                      p, optopt);
                return(BADCH);
              }
          }
        else                /* white space */
          {
            optarg = nargv[optind];
          }
        place = EMSG;
        ++optind;
      }
    return(optopt);       /* dump back option letter */
  }

  /*!
   * @if jp
   *
   * @class GetOpt
   * @brief GetOpt クラス
   *
   * @else
   *
   * @class GetOpt
   * @brief GetOpt class
   *
   * @endif
   */
  class GetOpt
  {
  public:
    /*!
     * @if jp
     *
     * @brief コンストラクタ
     *
     * コンストラクタ。
     *
     * @param name オブジェクト名
     *
     * @else
     *
     * @brief Constructor
     *
     * Constructor
     *
     * @param name Object name
     *
     * @endif
     */
    GetOpt(int argc, char* const argv[], const char* opt, int flag)
      : optind(1), opterr(1), optopt(0),
        m_argc(argc), m_argv(argv), m_opt(opt), m_flag(flag)
    {
      this->optarg = coil::optarg;
      coil::optind = 1;
    }

    /*!
     * @if jp
     *
     * @brief デストラクタ
     *
     * デストラクタ。
     *
     * @else
     *
     * @brief Destructor
     *
     * Destructor
     *
     * @endif
     */
    ~GetOpt()
    {
      coil::optind = 1;
    }

    /*!
     * @if jp
     *
     * @brief コマンドライン引数解析
     *
     * コマンドライン引数を解析する。
     *
     * @return 解析結果
     *
     * @else
     *
     * @brief Parses the command line arguments
     *
     * Parses the command line arguments.
     *
     * @return Result of parses.
     *
     * @endif
     */
    int operator()()
    {
      coil::opterr = opterr;
      coil::optind = optind;

      int result = getopt(m_argc, m_argv, m_opt);

      optarg = coil::optarg;
      optind = coil::optind;
      optopt = coil::optopt;

      return result;
    }

    char* optarg;     //! オプション引数
    int optind;       //! 処理対象引数
    int opterr;       //! エラー表示 0:抑止、1:表示
    int optopt;       //! オプション文字が足りない時、多い時にセットされる


  private:
    int m_argc;
    char* const * m_argv;
    const char* m_opt;
    int m_flag;

  };

} // namespace coil

#endif  // COIL_OS_H
