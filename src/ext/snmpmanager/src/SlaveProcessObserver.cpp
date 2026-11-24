// -*- C++ -*-
/*!
 * @file SlaveProcessObserver.cpp
 * @brief Detection of slave manager exits (replacement of coil::launch_shell). See SlaveProcessObserver.hpp
 *
 * @~japanese
 * slave マネージャの終了の検知 (coil::launch_shell の置き換え)。説明は SlaveProcessObserver.hpp
 * @~
 *
 * @date $Date: 2026-11-13 16:24:29 $
 * @author Nobuhiko Miyamoto <n-miyamoto@aist.go.jp>
 *
 * Copyright (C) 2026
 *     Nobuhiko Miyamoto
 *
 *     Intelligent Systems Research Institute,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id$
 *
 */

#include "SlaveProcessObserver.hpp"
#include "SNMPManagerUtil.hpp"

#include <cerrno>
#include <mutex>
#include <regex>
#include <set>
#include <thread>

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#else
#include <dlfcn.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#endif

namespace snmpmgr
{
  namespace process
  {
    namespace
    {
      std::mutex g_mutex;
      Callback g_callback;

      /*!
       * @~japanese
       * @brief 設定されたコールバックを返す
       *
       * @return コールバック
       * @~english
       * @brief Returns the configured callback
       *
       * @return callback
       * @~
       */
      Callback current()
      {
        std::lock_guard<std::mutex> g(g_mutex);
        return g_callback;
      }

      /*!
       * @~japanese
       * @brief コールバックを呼ぶ (例外は捨てる)
       *
       * @param cb コールバック
       * @param code 終了コード
       * @param info slave の情報
       * @~english
       * @brief Calls the callback (exceptions are discarded)
       *
       * @param cb callback
       * @param code exit code
       * @param info information of the slave
       * @~
       */
      void notify(const Callback& cb, int code, const ExitInfo& info)
      {
        try { cb(code, info); }
        catch (std::exception& e) { Log::info(std::string("process callback failed: ") + e.what()); }
      }
#ifndef _WIN32
      using RealLaunch = int (*)(const std::string&);
      /*!
       * @~japanese
       * @brief 元の coil::launch_shell (libRTC2 の関数) を返す
       *
       * @return 関数のポインタ
       * @~english
       * @brief Returns the original coil::launch_shell (function in libRTC2)
       *
       * @return function pointer
       * @~
       */
      RealLaunch real_launch()
      {
        // libRTC2 の coil::launch_shell (自分の定義の次に見つかるもの)
        static RealLaunch r = reinterpret_cast<RealLaunch>(
            dlsym(RTLD_NEXT, "_ZN4coil12launch_shellERKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE"));
        return r;
      }

      /*!
       * @~japanese
       * @brief slave を自分の子として起動し、終わるのを待つスレッドを始める
       *
       * @param command 起動のコマンド
       * @param cb コールバック
       * @return 0 (失敗なら -1)
       * @~english
       * @brief Starts a slave as a child of this process and a thread that waits for its exit
       *
       * @param command launch command
       * @param cb callback
       * @return 0 (-1 on failure)
       * @~
       */
      int launch_observed(const std::string& command, const Callback& cb)
      {
        // fork の後 (子) ではメモリを確保しない
        std::string shell_cmd = "exec " + command;
        signal(SIGCHLD, SIG_DFL);   // 元の launch_shell が SIG_IGN にしているので戻す (終了コードを受け取る)
        pid_t pid = fork();
        if (pid < 0) { return -1; }
        if (pid == 0)
          {
            setsid();   // 元の launch_shell と同じく別のセッションにする
            execl("/bin/sh", "sh", "-c", shell_cmd.c_str(), static_cast<char*>(nullptr));
            _exit(127);
          }
        ExitInfo info{pid, manager_name_of(command, pid), command};
        Log::info("slave " + info.manager_name + " started (pid " + std::to_string(pid) + ", observed)");
        std::thread([cb, info] {
          int status = 0;
          pid_t r;
          do { r = waitpid(info.pid, &status, 0); } while (r < 0 && errno == EINTR);
          int code = kUnknownExit;
          if (r == info.pid)
            {
              if (WIFEXITED(status)) { code = WEXITSTATUS(status); }
              else if (WIFSIGNALED(status)) { code = -WTERMSIG(status); }
            }
          notify(cb, code, info);
        }).detach();
        return 0;
      }
#endif
    } // namespace

    bool replaces_launch()
    {
#ifdef _WIN32
      return false;
#else
      return true;
#endif
    }

    void watch(int pid, const std::string& manager_name)
    {
#ifdef _WIN32
      static std::mutex m;
      static std::set<int> watched;
      if (pid <= 0 || !current()) { return; }
      {
        std::lock_guard<std::mutex> g(m);
        if (!watched.insert(pid).second) { return; }
      }
      HANDLE h = OpenProcess(SYNCHRONIZE | PROCESS_QUERY_LIMITED_INFORMATION, FALSE, static_cast<DWORD>(pid));
      if (!h)
        {
          Log::info("process observer: cannot open slave " + manager_name + " (pid " + std::to_string(pid) + ", error " +
                    std::to_string(GetLastError()) + ")");
          return;
        }
      ExitInfo info{pid, manager_name, ""};
      Log::info("slave " + manager_name + " is observed (pid " + std::to_string(pid) + ")");
      std::thread([h, info] {
        WaitForSingleObject(h, INFINITE);
        DWORD code = 0;
        int c = GetExitCodeProcess(h, &code) ? static_cast<int>(code) : kUnknownExit;
        CloseHandle(h);
        {
          std::lock_guard<std::mutex> g(m);
          watched.erase(info.pid);
        }
        auto cb = current();
        if (cb) { notify(cb, c, info); }
      }).detach();
#else
      (void)pid;
      (void)manager_name;
#endif
    }

    void set_callback(Callback cb)
    {
      std::lock_guard<std::mutex> g(g_mutex);
      g_callback = std::move(cb);
    }

    std::string manager_name_of(const std::string& command, int pid)
    {
      static const std::regex re(R"(manager\.instance_name:([^\s"']+))");
      std::smatch m;
      if (!std::regex_search(command, m, re)) { return ""; }
      std::string name = m[1];
      for (auto p = name.find("%p"); p != std::string::npos; p = name.find("%p"))
        {
          name.replace(p, 2, std::to_string(pid));
        }
      return name;
    }
  } // namespace process
} // namespace snmpmgr

#ifndef _WIN32
// OpenRTM (libRTC2) の coil::launch_shell を置き換える
namespace coil
{
  /*!
   * @~japanese
   * @brief coil::launch_shell の置き換え (コールバックがあれば観測して起動し、なければ元の関数を呼ぶ)
   *
   * @param command 起動のコマンド
   * @return 0 (失敗なら -1)
   * @~english
   * @brief Replacement of coil::launch_shell (launches with observation if a callback is set, otherwise calls the original)
   *
   * @param command launch command
   * @return 0 (-1 on failure)
   * @~
   */
  int launch_shell(const std::string& command)
  {
    auto cb = snmpmgr::process::current();
    if (cb) { return snmpmgr::process::launch_observed(command, cb); }
    auto real = snmpmgr::process::real_launch();
    return real ? real(command) : -1;
  }
} // namespace coil
#endif
