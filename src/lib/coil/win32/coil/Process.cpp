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
#include <memory>

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
    // std::wstring -> LPTSTR
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

    if (!CreateProcess(nullptr, lpcommand, nullptr, nullptr, FALSE, 0,
                      nullptr, nullptr, &si, &pi) )
      {
        delete[] lpcommand;
        return -1;
      }
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    delete[] lpcommand;
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
  int create_process(const std::string& command, std::vector<std::string> &out)
  {
      HANDLE rPipe, wPipe;
      SECURITY_ATTRIBUTES sa;
      sa.nLength = sizeof(sa);
      sa.bInheritHandle = TRUE;
      sa.lpSecurityDescriptor = nullptr;
      if (CreatePipe(&rPipe, &wPipe, &sa, 65535) == 0)
      {
          return -1;
      }
      if (!SetHandleInformation(rPipe, HANDLE_FLAG_INHERIT, 0))
      {
          CloseHandle(rPipe);
          CloseHandle(wPipe);
          return -1;
      }


      STARTUPINFO si{};
      si.cb = sizeof(si);
      si.dwFlags = STARTF_USESTDHANDLES;
      si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
      si.hStdOutput = wPipe;
      si.hStdError = wPipe;


#ifdef UNICODE
      // std::wstring -> LPTSTR
      std::wstring wcommand = string2wstring(command);
      LPTSTR lpcommand = new TCHAR[wcommand.size() + 1];
      _tcscpy_s(lpcommand, wcommand.size() + 1, wcommand.c_str());
#else
      // std::string -> LPTSTR
      LPTSTR lpcommand = new TCHAR[command.size() + 1];
      _tcscpy_s(lpcommand, command.size() + 1, command.c_str());
#endif // UNICODE

      PROCESS_INFORMATION pi{};
      BOOL retcp = CreateProcess(nullptr, lpcommand, nullptr, nullptr, TRUE, CREATE_NO_WINDOW, nullptr, nullptr, &si, &pi);
      delete[] lpcommand;
      if (!retcp)
      {
          std::string commandbatch = std::string("cmd.exe /c ") + command;
#ifdef UNICODE
          std::wstring wcommandbatch = string2wstring(commandbatch);
          LPTSTR lpcommandbatch = new TCHAR[wcommandbatch.size() + 1];
          _tcscpy_s(lpcommandbatch, wcommandbatch.size() + 1, wcommandbatch.c_str());
#else
          // std::string -> LPTSTR
          LPTSTR lpcommandbatch = new TCHAR[commandbatch.size() + 1];
          _tcscpy_s(lpcommandbatch, commandbatch.size() + 1, commandbatch.c_str());
#endif // UNICODE
          BOOL retcpbatch = CreateProcess(nullptr, lpcommandbatch, nullptr, nullptr, TRUE, CREATE_NO_WINDOW, nullptr, nullptr, &si, &pi);
          delete[] lpcommandbatch;
          if (!retcpbatch)
          {
              CloseHandle(rPipe);
              CloseHandle(wPipe);
              return -1;
          }
      }

      CloseHandle(wPipe);
      std::string output;
      char buffer[4096];
      DWORD bytesRead = 0;
      BOOL readOk;
      while ((readOk = ReadFile(rPipe, buffer, sizeof(buffer), &bytesRead, nullptr)) && bytesRead != 0)
      {
          output.append(buffer, bytesRead);
      }
      const DWORD readError = readOk ? ERROR_SUCCESS : GetLastError();
      CloseHandle(rPipe);
      WaitForSingleObject(pi.hProcess, INFINITE);
      CloseHandle(pi.hProcess);
      CloseHandle(pi.hThread);
      if (readError != ERROR_SUCCESS && readError != ERROR_BROKEN_PIPE && readError != ERROR_HANDLE_EOF)
      {
          return -1;
      }

      for(auto&& o : coil::split(output, "\n"))
      {
          if (!o.empty() && o.back() == '\r')
          {
              o.erase(o.size() - 1);
          }
          out.emplace_back(coil::eraseBothEndsBlank(std::move(o)));
      }

      return 0;

  }

  Popen::Popen(const std::string& cmd, const std::string& mode)
    {
      m_fd = _popen(cmd.c_str(), mode.c_str());
    }

  Popen::~Popen()
    {
      if (m_fd != nullptr)
        {
          _pclose(m_fd);
        }
    }

  bool Popen::isEof()
    {
      return feof(m_fd) != 0;
    }

  std::string Popen::getline()
    {
      if (m_fd == nullptr) { return ""; }
      if (feof(m_fd) != 0) { return ""; }
      char str[512];
      fgets(str, 512, m_fd);
      std::string line(str);
      return line;
    }
} // namespace coil

