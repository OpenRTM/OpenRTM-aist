// -*- C++ -*-
/*!
 * @file  Process.h
 * @brief Process handling functions
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
#include <coil/File.h>
#include <coil/stringutil.h>

#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <libgen.h>
#include <csignal>
#include <sys/types.h>
#include <sys/stat.h>
#include <string>
#include <regex>


namespace coil
{

  /*!
   * @if jp
   * @brief ¥×¥í¥»¥¹¤òµ¯Æ°¤¹¤ë
   * @else
   * @brief Launching a process
   * @endif
   */
  int launch_shell(const std::string& command)
  {
    signal(SIGCHLD, SIG_IGN);

    pid_t pid;
    if ((pid = fork()) < 0 )
      {  // fork failed
        return -1;
      }

    if (pid == 0)  // I'm child process
      {
        setsid();

        const std::regex base_regex("((?:[^\\s\"\\\\]|\\\\.|\"(?:\\\\.|[^\\\\\"])*(?:\"|$))+)");
        std::sregex_iterator words_begin = std::sregex_iterator(command.begin(), command.end(), base_regex);
        std::sregex_iterator words_end = std::sregex_iterator();

        coil::vstring vstr;

        for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
            std::smatch match = *i;
            std::string match_str = replaceString(match.str(), "\"", "");
            vstr.push_back(match_str);
        }
        
        Argv argv(vstr);

        execvp(vstr.front().c_str(), argv.get());

        return -1;
      }
    return 0;
  }

  int daemon(int nochdir, int noclose)
  {
    return ::daemon(nochdir, noclose);
  }

  int create_process(const std::string& command, std::vector<std::string> &out)
  {
    FILE* fd;
    out.clear();
    if ((fd = popen(command.c_str(), "r")) == nullptr)
      {
        return -1;
      }
    do
      {
        char str[512];
        if(fgets(str, 512, fd) != nullptr)
          {
            std::string line(str);
            if (!line.empty())
              {
                line.erase(line.size() - 1);
              }
            out.emplace_back(line);
          }
      } while (feof(fd) == 0);
    
    (void) pclose(fd);
    return 0;
  }

} // namespace coil
