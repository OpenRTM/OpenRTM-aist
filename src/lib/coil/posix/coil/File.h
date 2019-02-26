// -*- C++ -*-
/*!
 * @file File_posix.h
 * @brief File functions
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

#ifndef COIL_FILE_H
#define COIL_FILE_H

#include <coil/config_coil.h>
#include <coil/stringutil.h>

#include <dirent.h>
#include <libgen.h>
#include <cstdio>
#include <sys/types.h>
#include <string>
#include <cstring>

#include <cstring>
#include <sys/stat.h>


#include <coil/config_coil.h>
#include <coil/stringutil.h>

#ifdef __QNX__
using std::strlen;
using std::strcpy;
#endif

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
  inline std::string dirname(char* path)
  {
    std::string path_name(path);
    std::string dir_name = ::dirname(&path_name[0]);
    return dir_name;
  }

  /*!
   * @if jp
   *
   * @brief ファイルパスよりファイル名部分を取得する
   *
   * ファイルパスよりファイル名部分を取得する。
   *
   * @param path ファイルパス
   *
   * @return ファイル名称
   *
   * @else
   *
   * @brief Get a file name part than a file pass
   *
   * Get a directory part than a file pass.
   *
   * @param path File path
   *
   * @return File name
   *
   * @endif
   */
  inline std::string basename(const char* path)
  {
    std::string path_name(path);
    std::string base_name = ::basename(&path_name[0]);
    return base_name;
  }

  /*!
   * @if jp
   *
   * @brief ファイルリストを取得する
   *
   * ディレクトリパスの中で指定ファイルにマッチするリストを取得する。
   *
   * @param path ディレクトリパス
   * @param glob_str ファイル名
   *
   * @return ファイルリスト
   *
   * @else
   *
   * @brief Get file list
   *
   * Get a list matching a file designated than a directory path.
   *
   * @param path Directory path
   * @param glob_str File name
   *
   * @return File list
   *
   * @endif
   */
  inline coil::vstring filelist(const char* path, const char* glob_str = "")
  {
    struct dirent* ent;
    coil::vstring flist;
    bool has_glob(false);

    if (path == nullptr) { return flist; }
    if (glob_str[0] != '\0') { has_glob = true; }

    DIR* dir_ptr(::opendir(path));
    if (dir_ptr == nullptr) { return flist; }

    while ((ent = ::readdir(dir_ptr)) != nullptr)
      {
        bool match(true);
        if (has_glob)
          {
            const char* globc(glob_str);
            std::string fname(ent->d_name);
            for (size_t i(0); i < fname.size() && *globc != '\0'; ++i, ++globc)
              {
                if (*globc == '*')
                  {
                    // the last '*' matches every thing
                    if (globc[1] == '\0') { break; }
                    // consecutive * or + are skiped, but fname keeps pointer
                    if (globc[1] == '*' || globc[1] == '+') { --i; continue; }

                    // advance pointer and find normal characters
                    ++globc;
                    size_t pos(fname.find(*globc, i));
                    if (pos == std::string::npos) { match = false; break; }
                    // matched, and advance i to pos
                    i = pos;
                  }
                else if (*globc == '+')
                  {
                    // the last '+' matches last one or more characters
                    if (globc[1] == '\0' && !(i + 1 < fname.size())) { break; }
                    // consecutive * or + are skiped, but fname keeps pointer
                    if (globc[1] == '*' || globc[1] == '+') { --i; continue; }

                    // advance pointer and find normal characters
                    ++globc;
                    size_t pos(fname.find(*globc, i + 1));
                    if (pos == std::string::npos) { match = false; break; }
                    // matched, and advance i to pos
                    i = pos;
                  }
                else
                  {
                    if (fname[i] != *globc) { match = false; }
                  }

                // in the last fname character, if glob is not end,
                // or *, fname is not matched.
                if (i + 1 == fname.size() &&
                    globc[1] != '\0' && globc[1] != '*') { match = false; }
              }
          }
        if (match) { flist.push_back(ent->d_name); }
      }
    ::closedir(dir_ptr);

    return flist;
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
  inline void findFile(const std::string& dir, const std::string& filename, coil::vstring &filelist)
  {
	struct dirent **namelist=nullptr;
#ifndef COIL_OS_QNX
	int files = scandir(dir.c_str(), &namelist, nullptr, nullptr);
#else
	int files = scandir(const_cast<char*>(dir.c_str()), &namelist, NULL, NULL);
#endif

	for (int i=0; i<files; i++) {
		std::string dname = namelist[i]->d_name;
		if( dname != "." && dname != ".." ){

			std::string fullpath = dir + "/" + dname;

			struct stat stat_buf;
			if(stat(fullpath.c_str(), &stat_buf) == 0){
				if ((stat_buf.st_mode & S_IFMT) == S_IFDIR){
					findFile(fullpath, filename, filelist);
				}
				else
				{
					if(dname == filename)
					{
						filelist.push_back(fullpath);
					}
				}
			}
		
			
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
  inline void getFileList(const std::string& dir, const std::string& ext, coil::vstring &filelist)
  {
	struct dirent **namelist=nullptr;
#ifndef COIL_OS_QNX
	int files = scandir(dir.c_str(), &namelist, nullptr, nullptr);
#else
	int files = scandir(const_cast<char*>(dir.c_str()), &namelist, NULL, NULL);
#endif

	for (int i=0; i<files; i++) {
		std::string dname = namelist[i]->d_name;
		if( dname != "." && dname != ".." ){

			std::string fullpath = dir + "/" + dname;

			struct stat stat_buf;
			if(stat(fullpath.c_str(), &stat_buf) == 0){
				if ((stat_buf.st_mode & S_IFMT) == S_IFDIR){
					getFileList(fullpath, ext, filelist);
				}
				else
				{
					coil::vstring filesp = coil::split(dname, ".");
					if(filesp.back() == ext)
					{
						filelist.push_back(fullpath);
					}
				}
			}
		
			
		}
	}
	
  }
}  // namespace coil

#endif // COIL_FILE_H
