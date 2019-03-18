// -*- C++ -*-
/*!
 * @file DynamicLib.cpp
 * @brief DynamicLib class
 * @date $Date$
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2008 Noriaki Ando
 *     Task-intelligence Research Group,
 *     Intelligent Systems Research Institute,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id$
 *
 */

#include <coil/DynamicLib.h>

#include <utility>

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
    : m_name(""), m_mode(0), m_closeflag(0), m_handle(nullptr)
  {
    if (!rhs.m_name.empty() &&
        open(rhs.m_name.c_str(), rhs.m_mode, rhs.m_closeflag) == 0)
      return;
//    throw std::bad_alloc();
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
    std::swap(this->m_handle, tmp.m_handle);
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
                   int  close_handle_on_destruction)
  {
    void* handle = ::dlopen(dll_name, open_mode);
    if (handle == nullptr)
      {
        return -1;
      }
    m_handle = handle;
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
  int DynamicLib::close()
  {
    if (m_handle == nullptr)
      return -1;
    if (m_name.empty())
      {
        return -1;
      }
    ::dlclose(m_handle);
    m_handle = nullptr;
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
    if (m_handle == nullptr) return nullptr;
    return ::dlsym(m_handle, symbol_name);
  }

  /*!
   * @if jp
   * @brief エラーについての説明メッセージを返す
   * @else
   * @brief Return the explanation message about the error
   * @endif
   */
  const char* DynamicLib::error() const
  {
    return ::dlerror();
  }
} // namespace coil

/*!
 * for unit test.
 */
extern "C"
{
  int ForExternTest(void) { return coil::DynamicLib::ForExternTest(); }
}
