// -*- C++ -*-
/*!
 * @file  Process.cpp
 * @brief coil process management functions
 * @date  $Date$
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2010
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

#include <coil/Process.h>

namespace coil
{

  /*!
   * @if jp
   * @brief プロセスを起動する
   * @else
   * @brief Launching a process
   * @endif
   */
  int launch_shell(std::string command)
  {
    std::string cmd = "cmd.exe /c ";
    command = cmd + command;
#ifdef UNICODE
    // std::string -> LPTSTR
    std::wstring wcommand = string2wstring(command);
    LPTSTR lpcommand = new TCHAR[wcommand.size() + 1];
    _tcscpy(lpcommand, wcommand.c_str());
#else
    // std::string -> LPTSTR
    LPTSTR lpcommand = new TCHAR[command.size() + 1];
    _tcscpy_s(lpcommand, command.size() + 1, command.c_str());
#endif  // UNICODE

    STARTUPINFO si;
    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);

    PROCESS_INFORMATION pi;
    ZeroMemory( &pi, sizeof(pi) );

    if (!CreateProcess(NULL, lpcommand, NULL, NULL, FALSE, 0,
                      NULL, NULL, &si, &pi) )
      {
        delete lpcommand;
        return -1;
      }
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    delete lpcommand;
    return 0;
  }

  int daemon(int /*nochdir*/, int /*noclose*/)
  {
    // not implemented
    return 0;
  }


  /*!
  * @if jp
  * @brief プロセスを起動して出力を取得する
  *
  * @else
  *
  * @endif
  */
  int create_process(std::string command, std::vector<std::string> &out)
  {
      HANDLE rPipe, wPipe;
      SECURITY_ATTRIBUTES sa;
      sa.nLength = sizeof(sa);
      sa.bInheritHandle = TRUE;
      sa.lpSecurityDescriptor = NULL;
      if (!CreatePipe(&rPipe, &wPipe, &sa, 0))
      {
          return -1;
      }


      STARTUPINFO si = { 0 };
      si.cb = sizeof(si);
      si.dwFlags = STARTF_USESTDHANDLES;
      si.hStdInput = stdin;
      si.hStdOutput = wPipe;
      si.hStdError = wPipe;


#ifdef UNICODE
      // std::string -> LPTSTR
      std::wstring wcommand = string2wstring(command);
      LPTSTR lpcommand = new TCHAR[wcommand.size() + 1];
      _tcscpy(lpcommand, wcommand.c_str());
#else
      // std::string -> LPTSTR
      LPTSTR lpcommand = new TCHAR[command.size() + 1];
      _tcscpy_s(lpcommand, command.size() + 1, command.c_str());
#endif // UNICODE

      PROCESS_INFORMATION pi = { 0 };
      if (!CreateProcess(NULL, lpcommand, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi))
      {
          delete lpcommand;
          return -1;
      }


      WaitForSingleObject(pi.hProcess, INFINITE);
      CloseHandle(pi.hProcess);
      CloseHandle(pi.hThread);


      char Buf[1025] = { 0 };
      DWORD len;
      ReadFile(rPipe, Buf, sizeof(Buf) - 1, &len, NULL);


      out = coil::split(std::string(Buf), "\n");

      for(auto & o : out)
      {
          if (0 < o.size())
          {
              o.erase(o.size() - 1);
          }
          coil::eraseBothEndsBlank(o);
      }

      delete lpcommand;
      return 0;

  }
} // namespace coil

