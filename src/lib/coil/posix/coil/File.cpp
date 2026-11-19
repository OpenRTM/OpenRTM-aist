// -*- C++ -*-
/*!
 * @file File_posix.cpp
 * @brief File functions
 * @date $Date$
 * @author Nobuhiko Miyamoto <n-miyamoto@aist.go.jp>
 *
 * Copyright (C) 2019
 *     Robot Innovation Research Center,
 *     Intelligent Systems Research Institute,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id$
 *
 */

#include <iterator>
#include <coil/File.h>

namespace coil
{
  /*!
   * @if jp
   *
   * @brief ファイルパスよりディレクトリ部分を取得する
   *
   * ファイルパスよりディレクトリ部分を取得する。
   *
   * @param path ファイルパス
   *
   * @return ディレクトリ名称
   *
   * @else
   *
   * @brief Get a directory part than a file pass
   *
   * Get a directory part than a file pass.
   *
   * @param path File path
   *
   * @return Directory name
   *
   * @endif
   */
  std::string dirname(const char* path)
  {
    std::string path_name(path);
    std::string dir_name = ::dirname(&path_name[0]);
    return dir_name;
  }

  /*!
  * @if jp
  *
  * @brief 指定ファイル名を指定ディレクトリから探査する
  *
  * @param dir ディレクトリパス
  * @param filename ディレクトリパス
  * @param filelist ファイル一覧
  *
  *
  * @else
  *
  * @brief
  *
  * @param dir
  * @param filename
  * @param filelist
  *
  * @endif
  */
  void findFile(const std::string& dir, const std::string& filename, coil::vstring &filelist)
  {
    struct dirent** namelist = nullptr;
#ifndef COIL_OS_QNX
    int files = scandir(dir.c_str(), &namelist, nullptr, nullptr);
#else
    int files = scandir(const_cast<char*>(dir.c_str()), &namelist, NULL, NULL);
#endif

    for (int i = 0; i < files; ++i)
    {
      std::string dname = (*std::next(namelist, i))->d_name;
      if(dname == "." || dname == "..") { continue; }
      std::string fullpath = dir + "/" + dname;

      struct stat stat_buf;
      if (stat(fullpath.c_str(), &stat_buf) != 0) { continue; }
      if ((stat_buf.st_mode & S_IFMT) == S_IFDIR)
        {
          findFile(fullpath, filename, filelist);
          continue;
        }
      if(dname == filename)
        {
          filelist.emplace_back(std::move(fullpath));
        }
     }
  }

  /*!
  * @if jp
  *
  * @brief ファイル一覧を指定ディレクトリから探査する
  *
  * @param dir ディレクトリパス
  * @param ext 拡張子
  * @param filelist ファイル一覧
  *
  *
  * @else
  *
  * @brief
  *
  * @param dir
  * @param ext
  * @param filelist
  *
  * @endif
  */
  void getFileList(const std::string& dir, const std::string& ext, coil::vstring &filelist)
  {
    struct dirent **namelist=nullptr;
#ifndef COIL_OS_QNX
    int files = scandir(dir.c_str(), &namelist, nullptr, nullptr);
#else
    int files = scandir(const_cast<char*>(dir.c_str()), &namelist, NULL, NULL);
#endif

    for (int i = 0; i < files; ++i)
      {
        std::string dname = (*std::next(namelist, i))->d_name;
        if (dname == "." || dname == "..") { continue; }

        std::string fullpath = dir + "/" + dname;
        struct stat stat_buf;
        if(stat(fullpath.c_str(), &stat_buf) != 0) { continue; }
        if ((stat_buf.st_mode & S_IFMT) == S_IFDIR)
          {
            getFileList(fullpath, ext, filelist); // recursive call
            continue;
          }
        coil::vstring filesp = coil::split(dname, ".");
        if(filesp.back() == ext)
          {
            filelist.emplace_back(std::move(fullpath));
          }
      }
  }
} //namespace coil
