// -*- C++ -*-
/*!
 * @file DynamicLib.h
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

#ifndef COIL_DYNAMICLIB_H
#define COIL_DYNAMICLIB_H

#ifdef __RTP__
#include <dlfcn.h>
#define COIL_DEFAULT_DYNLIB_MODE RTLD_LAZY
#else
#include <moduleLib.h>
#include <loadLib.h>
#include <symLib.h>
#define COIL_DEFAULT_DYNLIB_MODE LOAD_GLOBAL_SYMBOLS
#endif
#include <string>
#include <coil/config_coil.h>




extern "C"
{
  int ForExternTest(void);
}

namespace coil
{
  /*!
   * @if jp
   *
   * @class DynamicLib
   * @brief DynamicLib クラス
   *
   * @else
   *
   * @class DynamicLib
   * @brief DynamicLib class
   *
   * @endif
   */
  class DynamicLib
  {
  public:

    /*!
     * @if jp
     *
     * @brief コンストラクタ
     *
     * コンストラクタ。
     *
     * @param close_handle_on_destruction クローズフラグ
     *
     * @else
     *
     * @brief Constructor
     *
     * Constructor
     *
     * @param close_handle_on_destruction Close flag.
     *
     * @endif
     */
    DynamicLib(int close_handle_on_destruction = 1);

    /*!
     * @if jp
     *
     * @brief コンストラクタ
     *
     * コンストラクタ。
     *
     * @param dynlib_name 動的リンクライブラリ名称
     * @param open_mode オープンモード
     * @param close_handle_on_destruction クローズフラグ
     *
     * @else
     *
     * @brief Constructor
     *
     * Constructor
     *
     * @param dynlib_name Dynamic link library name.
     * @param open_mode Open mode.
     * @param close_handle_on_destruction Close flag.
     *
     * @endif
     */
    DynamicLib(const char* dynlib_name,
               int open_mode = COIL_DEFAULT_DYNLIB_MODE,
               int close_handle_on_destruction = 1);

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
    virtual ~DynamicLib();

    /*!
     * @if jp
     *
     * @brief コピーコンストラクタ
     *
     * コピーコンストラクタ。
     *
     * @param rhs コピー元動的リンクライブラリオブジェクト
     *
     * @else
     *
     * @brief Copy Constructor
     *
     * Copy Constructor
     *
     * @param rhs Dynamic link library object of copy source.
     *
     * @endif
     */
    DynamicLib(const DynamicLib& rhs);

    /*!
     * @if jp
     *
     * @brief 代入演算子
     *
     * 動的リンクライブラリオブジェクトをコピーする。
     *
     * @param rhs 代入元動的リンクライブラリオブジェクト
     *
     * @return 代入結果
     *
     * @else
     *
     * @brief Assignment operator
     *
     * Copy a Dynamic link library object.
     *
     * @param rhs Dynamic link library object of assignment source.
     *
     * @return Assignment result.
     *
     * @endif
     */
    DynamicLib& operator=(const DynamicLib& rhs);

    /*!
     * @if jp
     *
     * @brief 動的リンクライブラリのロード
     *
     * 動的リンクライブラリをロードする。
     *
     * @param dll_name 動的リンクライブラリ名称
     * @param open_mode オープンモード
     * @param close_handle_on_destruction クローズフラグ
     *
     * @return 0: 成功, -1: 失敗
     *
     * @else
     *
     * @brief Load of the Dynamic link library 
     *
     * Load of the Dynamic link library.
     *
     * @param dll_name Dynamic link library name.
     * @param open_mode Open mode.
     * @param close_handle_on_destruction Close flag.
     *
     * @return 0: successful, -1: failed
     *
     * @endif
     */
    virtual int open(const char* dll_name,
                     int open_mode = COIL_DEFAULT_DYNLIB_MODE,
                     int close_handle_on_destruction = 1);

    /*!
     * @if jp
     *
     * @brief 動的リンクライブラリのアンロード
     *
     * 動的リンクライブラリをアンロードする。
     *
     * @return 0: 成功, -1: 失敗
     *
     * @else
     *
     * @brief Unload of the Dynamic link library 
     *
     * Unload of the Dynamic link library.
     *
     * @return 0: successful, -1: failed
     *
     * @endif
     */
    virtual int close(void);

    /*!
     * @if jp
     *
     * @brief シンボルがロードされたメモリアドレスを返す
     *
     * シンボルがロードされたメモリアドレスを返す。
     *
     * @param symbol_name シンボル名称
     *
     * @return メモリアドレス(NULL: 失敗)
     *
     * @else
     *
     * @brief Return an address of the memory where a symbol was loaded
     *
     * Return an address of the memory where a symbol was loaded.
     *
     * @param symbol_name Symbol name.
     *
     * @return Memory address.(NULL: failed)
     *
     * @endif
     */
    void *symbol (const char* symbol_name);

    /*!
     * @if jp
     *
     * @brief エラーについての説明メッセージを返す
     *
     * エラーについての説明メッセージを返す。
     *
     * @return エラーメッセージ(NULL: エラーなし)
     *
     * @else
     *
     * @brief Return the explanation message about the error
     *
     * Return the explanation message about the error.
     *
     * @return Error message.(NULL: not an error)
     *
     * @endif
     */
    const char* error(void) const;

    /*!
     * @if jp
     *
     * @brief ユニットテスト
     *
     * ユニットテストを行う。
     *
     * @return 0xdeadbeef
     *
     * @else
     *
     * @brief Unit Test
     *
     * Unit Test.
     *
     * @return 0xdeadbeef
     *
     * @endif
     */
    static int ForExternTest(void) { return 0xdeadbeef; }

  private:
    std::string m_name;
    int m_mode;
    int m_closeflag;
#ifdef __RTP__
    void* m_handle;
#else
    int m_fd;
    MODULE_ID m_id;
#endif
    int m_error;
  };
  
#ifndef __RTP__
  typedef struct
  {
    const char* name;
    int         group;
    void*       addr;
  }SymbolObj;
  
  extern "C" bool SymbolIterator(char* name, int val, SYM_TYPE type, int arg, UINT16 group);
#endif

};

#endif // DynamicLib_h
