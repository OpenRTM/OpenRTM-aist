// -*- C++ -*-
/*!
 * @file SharedMemory_posix.cpp
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


#include <coil/SharedMemory.h>
//#include "SharedMemory.h"
#include <cstring>
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
  SharedMemory::SharedMemory()
    : m_memory_size(0),
      m_shm(nullptr),
      m_file_create(false),
      m_fd(-1)
  {
  }


  /*!
   * @if jp
   * @brief デストラクタ
   * @else
   * @brief Destructor
   * @endif
   */
  SharedMemory::~SharedMemory()
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
  SharedMemory::SharedMemory(const SharedMemory& rhs)
  {
    m_memory_size = rhs.m_memory_size;
    m_shm_address = rhs.m_shm_address;
    m_shm = rhs.m_shm;
    m_fd = rhs.m_fd;
 

  }

  /*!
   * @if jp
   * @brief 代入演算子
   * @else
   * @brief Assignment operator
   * @endif
   */
  SharedMemory& SharedMemory::operator=(const SharedMemory& rhs)
  {
    SharedMemory tmp(rhs);
    std::swap(this->m_memory_size, tmp.m_memory_size);
    std::swap(this->m_shm_address, tmp.m_shm_address);
    std::swap(this->m_shm, tmp.m_shm);
    std::swap(this->m_fd, tmp.m_fd);
    std::swap(this->m_file_create, tmp.m_file_create);

    return *this;
  }

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
  int SharedMemory::create(std::string shm_address,
                     unsigned long long memory_size)
  {

    m_shm_address = std::move(shm_address);
    m_memory_size = memory_size;
 

    m_fd = shm_open(m_shm_address.c_str(), O_RDWR|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH);
    if(m_fd < 0)
    {
        return -1;
    }
    ftruncate(m_fd, m_memory_size);
    m_shm = (char*)mmap(nullptr,
            m_memory_size,
            PROT_READ|PROT_WRITE,
            MAP_SHARED,
            m_fd,
            0);

    m_file_create = true;
    return 0;
  }

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
  int SharedMemory::open(std::string shm_address, unsigned long long memory_size)
  {
    m_shm_address = std::move(shm_address);
	m_memory_size = memory_size;


    m_fd = shm_open(m_shm_address.c_str(), O_RDWR|O_CREAT, 0);
    if(m_fd < 0)
    {
        return -1;
    }
    ftruncate(m_fd, m_memory_size);
    m_shm = (char*)mmap(nullptr,
            m_memory_size,
            PROT_READ|PROT_WRITE,
            MAP_SHARED,
            m_fd,
            0);
 
    return 0;
  }

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
  int SharedMemory::write(const char *data, const unsigned long long pos, const unsigned long long size)
  {
	  if (!created())
	  {
		  return -1;
	  }

	  memcpy(&m_shm[pos],&data[0],size);
    
	  return 0;
  }

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
  int SharedMemory::read(char* data, const unsigned long long pos, const unsigned long long size)
  {
	  if (!created())
	  {
		  return -1;
	  }

	  memcpy(&data[0],&m_shm[pos],size);

	  return 0;
  }

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
  int SharedMemory::close()
  {
    
    if (created())
    {
	::close(m_fd);
    }
    else
    {
	return -1;
    }
    if(m_file_create)
    {
	//shm_unlink(m_shm_address);
    }
    return 0;

  }
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
  unsigned long long SharedMemory::get_size()
  {
	return m_memory_size;
  }
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
  std::string SharedMemory::get_addresss()
  {
	return m_shm_address;
  };
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
  char *SharedMemory::get_data()
  {
	return m_shm;
  }



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
  int SharedMemory::unlink()
  {
	shm_unlink(m_shm_address.c_str());
	return 0;
  }


  /*!
  * @if jp
  *
  * @brief 共有メモリに生成済みかの確認
  *
  *
  *
  * @return true: 作成済み, false: 未作成
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
  bool SharedMemory::created()
  {
	if(m_fd < 0)
	{
		return false;
	}
	else
	{
		return true;
	}
  }




}  // namespace coil
