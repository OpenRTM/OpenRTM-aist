// -*- C++ -*-
/*!
 * @file SharedMemoryPort.h
 * @brief Shared Memory Port class
 * @date $Date: 2017-10-29 07:57:18 $
 * @author Nobuhiko Miyamoto <n-miyamoto@aist.go.jp>
 *
 * Copyright (C) 2006-2017
 *     Robot Innovation Research Center,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 *
 */

#ifndef SHAREDMEMORYPORT_H
#define SHAREDMEMORYPORT_H


#include <rtm/Manager.h>
#include <rtm/idl/SharedMemorySkel.h>
#include <coil/SharedMemory.h>

#define DEFAULT_DATA_SIZE 8
#define DEFAULT_SHARED_MEMORY_SIZE 2097152

namespace RTC
{
  /*!
   * @if jp
   * @class SharedMemoryPort
   * @brief SharedMemory クラス
   *
   * SharedMemoryPort 
   *
   * 共有メモリ操作クラス
   * CORBAによる通信により、mmapの初期化、終了などがリモートに操作できる
   *
   * @since 1.2.0
   *
   * @else
   * @class 
   * @brief 
   *
   * 
   *
   * @since 1.2.0
   *
   * @endif
   */
  class SharedMemoryPort
      : public virtual POA_OpenRTM::PortSharedMemory,
      public virtual PortableServer::RefCountServantBase
  {
  public:
    /*!
     * @if jp
     * @brief コンストラクタ
     *
     * コンストラクタ
     *
     *
     * @else
     *
     * @brief Constructor
     *
     * Constructor
     *
     * @param manager Manager object
     *
     * @endif
     */
	  SharedMemoryPort();
    /*!
     * @if jp
     * @brief デストラクタ
     *
     * デストラクタ
     *
     * @else
     * @brief Destructor
     *
     * Destructor
     *
     * @endif
     */
	  virtual ~SharedMemoryPort();
    /*!
     * @if jp
     * @brief 文字列で指定したデータサイズを数値に変換する
	 * 1M → 1048576
	 * 1k → 1024
	 * 100 → 100
     *
     * @param size_str データサイズ(文字列)
     * @return データサイズ(数値)
     *
     *
     *
     * @else
     * @brief 
     *
     * @param 
     * @return 
     *
     * @endif
     */
    virtual int string_to_MemorySize(std::string size_str);
     /*!
     * @if jp
     * @brief 共有メモリの初期化
	 * windowsではページングファイル上に領域を確保する
	 * Linuxでは/dev/shm以下にファイルを作成する
	 * 作成したファイルの内容を仮想アドレスにマッピングする
     *
     *
     *
     * @else
     * @brief 
     *
     * @param memory_size 
     * @param shm_address 
     *
     * @endif
     */
	virtual void create_memory(::CORBA::ULongLong memory_size, const char *shm_address)
    	throw (CORBA::SystemException);
     /*!
     * @if jp
     * @brief 共有メモリのマッピングを行う
     *
     * @param memory_size 共有メモリのサイズ
     * @param shm_address 空間名
     *
     *
     *
     * @else
     * @brief 
     *
     * @param memory_size 
     * @param shm_address 
     *
     * @endif
     */
	virtual void open_memory(::CORBA::ULongLong memory_size, const char *shm_address)
    	throw (CORBA::SystemException);
     /*!
     * @if jp
     * @brief マッピングした共有メモリをアンマップする
     * @param unlink Linuxで/dev/shm以下に作成したファイルを削除する場合にTrueにする
     *
     *
     * @else
     * @brief 
     *
     * @param unlink
     *
     * @endif
     */
	virtual void close_memory(::CORBA::Boolean unlink = false)
    	throw (CORBA::SystemException);
     /*!
     * @if jp
     * @brief データを書き込む
	 * 先頭8byteにデータサイズを書き込み、その後ろにデータを書き込む
	 * 設定したデータサイズが共有メモリのサイズを上回った場合、共有メモリの初期化を行う
	 *
     * @param data 書き込むデータ
     *
     *
     *
     * @else
     * @brief 
     *
     * @param data 
     *
     * @endif
     */
    virtual void write(cdrMemoryStream& data);
     /*!
     * @if jp
     * @brief データを読み込む
     * @return データ
     *
     *
     *
     * @else
     * @brief 
     *
     * @return 
     *
     * @endif
     */
    virtual void read(cdrMemoryStream& data);
     /*!
     * @if jp
     * @brief 通信先のCORBAインターフェースを登録する
	 * 登録する事により共有メモリの初期化したときに、通信先でもマッピングをやり直すことができる
	 *
     * @param sm SharedMemoryのオブジェクトリファレンス
     *
     *
     *
     * @else
     * @brief 
     *
     * @return 
     *
     * @endif
     */
	virtual void setInterface(::OpenRTM::PortSharedMemory_ptr sm)
    	throw (CORBA::SystemException);
     /*!
     * @if jp
     * @brief エンディアンを設定する
	 *
     * @param endian エンディアン
	 *
     *
     *
     * @else
     * @brief 
     *
     * @param 
     *
     * @endif
     */
	virtual void setEndian(::CORBA::Boolean endian)
    	throw (CORBA::SystemException);
     /*!
     * @if jp
     * @brief データの送信を知らせる
	 *
     * @return 
     *
     *
     *
     * @else
     * @brief 
     *
     * @return 
     *
     * @endif
     */
    virtual ::OpenRTM::PortStatus put()
      throw (CORBA::SystemException);
     /*!
     * @if jp
     * @brief データの送信を要求する
	 *
     * @return 
     *
     *
     *
     * @else
     * @brief 
     *
     * @return 
     *
     * @endif
     */
    virtual ::OpenRTM::PortStatus get()
      throw (CORBA::SystemException);

	virtual ::OpenRTM::PortSharedMemory_ptr getObjRef();

 protected:
    //mutable Logger rtclog;
    ::OpenRTM::PortSharedMemory_var m_smInterface;
    bool m_endian;
    coil::SharedMemory m_shmem;
	//::OpenRTM::PortSharedMemory_var m_objref;
    unsigned int m_created_count;

    
  };  // class SharedMemoryPort
};  // namespace RTC

#ifdef WIN32
#pragma warning( default : 4290 )
#endif

#endif // RTC_RTOBJECT
