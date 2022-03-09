// -*- C++ -*-
/*!
 * @file SharedMemory_vxworks.h
 * @brief SharedMemory class
 * @date $Date$
 * @author Nobuhiko Miyamoto <n-miyamoto@aist.go.jp>
 *
 * Copyright (C) 2017 Nobuhiko Miyamoto
 *     Robot Innovation Research Center,
 *     Intelligent Systems Research Institute,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id$
 *
 */

#ifndef COIL_SHAREDMEMORY_H
#define COIL_SHAREDMEMORY_H


#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstdio>
#include <unistd.h>

#include <string>

#include <coil/config_coil.h>

#define DEFAULT_MEMORY_SIZE 2097152



namespace coil
{
  /*!
   * @if jp
   *
   * @class SharedMemory
   * @brief SharedMemory クラス
   *
   * @else
   *
   * @class SharedMemory
   * @brief SharedMemory class
   *
   * @endif
   */
  class SharedMemory
  {
  public:

    /*!
     * @if jp
     *
     * @brief コンストラクタ
     *
     * コンストラクタ。
     *
     *
     * @else
     *
     * @brief Constructor
     *
     * Constructor
     *
     *
     * @endif
     */
    SharedMemory();



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
    virtual ~SharedMemory();

    /*!
     * @if jp
     *
     * @brief コピーコンストラクタ
     *
     * コピーコンストラクタ。
     *
     * @param rhs コピー元共有メモリオブジェクト
     *
     * @else
     *
     * @brief Copy Constructor
     *
     * Copy Constructor
     *
     * @param rhs shared memory object of copy source.
     *
     * @endif
     */
    SharedMemory(const SharedMemory& rhs);

    /*!
     * @if jp
     *
     * @brief 代入演算子
     *
     * 共有メモリオブジェクトをコピーする。
     *
     * @param rhs 代入元動的共有メモリオブジェクト
     *
     * @return 代入結果
     *
     * @else
     *
     * @brief Assignment operator
     *
     * Copy a shared memory object.
     *
     * @param rhs shared memory object of assignment source.
     *
     * @return Assignment result.
     *
     * @endif
     */
    SharedMemory& operator=(const SharedMemory& rhs);


    /*!
     * @if jp
     *
     * @brief 共有メモリの生成
     *。
     *
     * @param shm_address 共有メモリの識別子
     * @param memory_size 共有メモリのサイズ
     *
     * @return 0: 成功, -1: 失敗
     *
     * @else
     *
     * @brief Create Shared Memory 
     *
     *
     * @param shm_address 
     * @param memory_size 
     *
     * @return 0: successful, -1: failed
     *
     * @endif
     */
    virtual int create(std::string shm_address,
                     unsigned long long memory_size = DEFAULT_MEMORY_SIZE);


    /*!
     * @if jp
     *
     * @brief 共有メモリへのアクセス
     *。
     *
     * @param shm_address 共有メモリの識別子
     *
     * @return 0: 成功, -1: 失敗
     *
     * @else
     *
     * @brief Open Shared Memory 
     *
     *
     * @param shm_address 
     * @param memory_size 
     *
     * @return 0: successful, -1: failed
     *
     * @endif
     */
    virtual int open(std::string shm_address,
                     unsigned long long memory_size = DEFAULT_MEMORY_SIZE);


    /*!
     * @if jp
     *
     * @brief 共有メモリへの書き込み
     *。
     *
     * @param data 書き込むデータ
     *
     * @return 0: 成功, -1: 失敗
     *
     * @else
     *
     * @brief Write Shared Memory 
     *
     *
     * @param data 
     *
     * @return 0: successful, -1: failed
     *
     * @endif
     */
    virtual int write(const char *data, unsigned long long pos, unsigned long long size);


    /*!
     * @if jp
     *
     * @brief 共有メモリからの読み込み
     *。
     *
     * @param data 読み込むデータ
     *
     * @return 0: 成功, -1: 失敗
     *
     * @else
     *
     * @brief Read Shared Memory 
     *
     *
     * @param data 
     *
     * @return 0: successful, -1: failed
     *
     * @endif
     */
    virtual int read(char* data, unsigned long long pos, unsigned long long size);

    /*!
     * @if jp
     *
     * @brief 共有メモリの削除
     *。
     *
     *
     * @return 0: 成功, -1: 失敗
     *
     * @else
     *
     * @brief Close Shared Memory 
     *
     *
     *
     * @return 0: successful, -1: failed
     *
     * @endif
     */
    virtual int close();
    /*!
     * @if jp
     *
     * @brief メモリのサイズ取得
     *。
     *
     *
     * @return サイズ
     *
     * @else
     *
     * @brief 
     *
     *
     *
     * @return size
     *
     * @endif
     */
    virtual unsigned long long get_size();
    /*!
     * @if jp
     *
     * @brief メモリのアドレス取得
     *。
     *
     *
     * @return アドレス
     *
     * @else
     *
     * @brief 
     *
     *
     *
     * @return address
     *
     * @endif
     */
    virtual std::string get_addresss();
    /*!
     * @if jp
     *
     * @brief メモリの取得
     *
     *
     *
     * @return メモリ
     *
     * @else
     *
     * @brief 
     *
     *
     *
     * @return memory
     *
     * @endif
     */
    virtual char *get_data();
    /*!
     * @if jp
     *
     * @brief ファイル削除(Windowsの場合は何もしない)
     *
     *
     *
     * @return 0: 成功, -1: 失敗
     *
     * @else
     *
     * @brief 
     *
     *
     *
     * @return 0: successful, -1: failed
     *
     * @endif
     */
    virtual int unlink();
     /*!
     * @if jp
     *
     * @brief 共有メモリに生成済みかの確認
     *
     *
     *
     * @return true: 作成済み,false: 未作成
     *
     * @else
     *
     * @brief 
     *
     *
     *
     * @return
     *
     * @endif
     */
    virtual bool created();

  private:
    unsigned long long m_memory_size{0};
    std::string m_shm_address;
    char *m_shm{nullptr};
    bool m_file_create{false};
    int m_fd{-1};
  };  // class SharedMemory

} // namespace coil

#endif // SharedMemory_h
