// -*- C++ -*-
/*!
 * @file DynamicLib.cpp
 * @brief DynamicLib class
 * @date $Date$
 * @author Nobuhiko Miyamoto <n-miyamoto@aist.go.jp>
 *
 * Copyright (C) 2017
 *     Nobuhiko Miyamoto
 *     Robot Innovation Research Center
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id$
 *
 */

#include <coil/DynamicLib.h>

#ifndef __RTP__
#include <unldLib.h>
#include <ioLib.h>
#include <sysSymTbl.h>
#endif



namespace coil
{
  /*!
   * @if jp
   * @brief コンストラクタ
   * @else
   * @brief Constructor
   * @endif
   */
  DynamicLib::DynamicLib(int close_handle_on_destruction)
    : m_closeflag(close_handle_on_destruction)
  {
  }

  /*!
   * @if jp
   * @brief コンストラクタ
   * @else
   * @brief Constructor
   * @endif
   */
  DynamicLib::DynamicLib(const char* dynlib_name,
                         int open_mode,
                         int close_handle_on_destruction)
    : m_name(dynlib_name), m_mode(open_mode),
      m_closeflag(close_handle_on_destruction)
  {
    if (open(m_name.c_str(), m_mode, m_closeflag) != 0)
      {
        throw std::bad_alloc();
      }
  }

  /*!
   * @if jp
   * @brief デストラクタ
   * @else
   * @brief Destructor
   * @endif
   */
  DynamicLib::~DynamicLib()
  {
    close();
  }

  /*!
   * @if jp
   * @brief コピーコンストラクタ
   * @else
   * @brief Copy Constructor
   * @endif
   */
  DynamicLib::DynamicLib(const DynamicLib& rhs)
#ifdef __RTP__
    : m_name(""), m_mode(0), m_closeflag(0), m_handle(0)
#else
  : m_name(""), m_mode(0), m_fd(-1), m_id(0)
#endif
  {
    if (!rhs.m_name.empty() &&
        open(rhs.m_name.c_str(), rhs.m_mode, rhs.m_closeflag) == 0)
      return;
  }

  /*!
   * @if jp
   * @brief 代入演算子
   * @else
   * @brief Assignment operator
   * @endif
   */
  DynamicLib& DynamicLib::operator=(const DynamicLib& rhs)
  {
    DynamicLib tmp(rhs);
    std::swap(this->m_name, tmp.m_name);
    std::swap(this->m_mode, tmp.m_mode);
    std::swap(this->m_closeflag, tmp.m_closeflag);
#ifdef __RTP__
    std::swap(this->m_handle, tmp.m_handle);
#else
    std::swap(this->m_fd, tmp.m_fd);
    std::swap(this->m_id, tmp.m_id);
#endif
    return *this;
  }

  /*!
   * @if jp
   * @brief 動的リンクライブラリのロード
   * @else
   * @brief Load of the Dynamic link library 
   * @endif
   */
  int DynamicLib::open(const char* dll_name,
                   int open_mode,
                   int close_handle_on_destruction)
  {
#ifdef __RTP__
    void* handle = ::dlopen(dll_name, open_mode);
    if (handle == NULL)
      {
        return -1;
      }
    m_handle = handle;
    
#else
    m_fd = ::open(const_cast<char*>(dll_name), O_RDONLY, 0);
    if (!m_fd)
    {
      return -1;
    }
    m_id = loadModule(m_fd, open_mode);
    if (!m_id)
    {
      ::close(m_fd);
      return -1;
    }
#endif
    m_name = dll_name;
    m_closeflag = close_handle_on_destruction;
    return 0;
  }

  /*!
   * @if jp
   * @brief 動的リンクライブラリのアンロード
   * @else
   * @brief Unload of the Dynamic link library 
   * @endif
   */
  int DynamicLib::close(void)
  {
#ifdef __RTP__
    if (m_handle == NULL)
      return -1;
    if (m_name.empty())
      {
        return -1;
      }
    ::dlclose(m_handle);
    m_handle = NULL;
#else
    if (!m_id)
          return -1;
    if (m_name.empty())
          {
            return -1;
          }
    unldByModuleId(m_id, 0);
    m_id = 0;
#endif
    m_name = "";
    return 0;
  }

  /*!
   * @if jp
   * @brief シンボルがロードされたメモリアドレスを返す
   * @else
   * @brief Return an address of the memory where a symbol was loaded
   * @endif
   */
  void* DynamicLib::symbol(const char* symbol_name)
  {
#ifdef __RTP__
    if (m_handle == NULL) return NULL;
    return ::dlsym(m_handle, symbol_name);
#else
    if (!m_id) return NULL;
    MODULE_INFO mi;
    if (moduleInfoGet(m_id, &mi) != OK) return 0;
    SymbolObj symbolObj;
    symbolObj.name  = const_cast<char*>(symbol_name);
    symbolObj.group = mi.group;
    symbolObj.addr  = 0;
    symEach(sysSymTbl, reinterpret_cast<FUNCPTR>(SymbolIterator), reinterpret_cast<int>(&symbolObj));

    return symbolObj.addr;
#endif
  }

  /*!
   * @if jp
   * @brief エラーについての説明メッセージを返す
   * @else
   * @brief Return the explanation message about the error
   * @endif
   */
  const char* DynamicLib::error(void) const
  {
#ifdef __RTP__
    return ::dlerror();
#else
    return "";
#endif
  }
  
#ifndef __RTP__
  extern "C" bool SymbolIterator(char* name, int val, SYM_TYPE type, int arg, UINT16 group)
   {
     SymbolObj* symbolObj = reinterpret_cast<SymbolObj*>(arg);
     if (group == symbolObj->group && std::strcmp(name, symbolObj->name) == 0)
     {
       symbolObj->addr = reinterpret_cast<void*>(val);
       return false;
     }
     else return true;
   };
#endif
};

/*!
 * for unit test.
 */
extern "C"
{
  int ForExternTest(void) { return coil::DynamicLib::ForExternTest(); }
}
