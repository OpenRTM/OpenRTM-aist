// -*- C++ -*-
/*!
 * @file SharedMemory_win32.cpp
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
#include <string.h>


namespace coil
{
  /*!
   * @if jp
   * @brief �R���X�g���N�^
   * @else
   * @brief Constructor
   * @endif
   */
  SharedMemory::SharedMemory()
    : m_memory_size(0),
      m_shm(NULL),
      m_file_create(false),
      m_handle(NULL)
  {
  }


  /*!
   * @if jp
   * @brief �f�X�g���N�^
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
   * @brief �R�s�[�R���X�g���N�^
   * @else
   * @brief Copy Constructor
   * @endif
   */
  SharedMemory::SharedMemory(const SharedMemory& rhs)
  {
    m_memory_size = rhs.m_memory_size;
    m_shm_address = rhs.m_shm_address;
    m_shm = rhs.m_shm;
    m_handle = rhs.m_handle;
    m_file_create = rhs.m_file_create;
  }

  /*!
   * @if jp
   * @brief ������Z�q
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
    std::swap(this->m_handle, tmp.m_handle);
    return *this;
  }

  /*!
   * @if jp
   *
   * @brief ���L�������̐���
   *�B
   *
   * @param shm_address ���L�������̎��ʎq
   * @param memory_size ���L�������̃T�C�Y
   *
   * @return 0: ����, -1: ���s
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
                     int memory_size)
  {

    m_shm_address = shm_address;
    m_memory_size = memory_size;
    m_handle = CreateFileMapping(
		(HANDLE)INVALID_HANDLE_VALUE,
		NULL,
		PAGE_READWRITE | SEC_COMMIT,
		0, m_memory_size, 
		shm_address.c_str());
    /*
    if(GetLastError() == ERROR_ALREADY_EXISTS)
    {
    }
    else
    {
    	
    }
    */

    m_shm = (char *)MapViewOfFile(m_handle, FILE_MAP_ALL_ACCESS, 0, 0, 0);
    m_file_create = true;
    return 0;
  }

  /*!
   * @if jp
   *
   * @brief ���L�������ւ̃A�N�Z�X
   *�B
   *
   * @param shm_address ���L�������̎��ʎq
   *
   * @return 0: ����, -1: ���s
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
  int SharedMemory::open(std::string shm_address, int memory_size)
  {
    m_shm_address = shm_address;
	m_memory_size = memory_size;
    m_handle = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, m_shm_address.c_str());
    m_shm = (char *)MapViewOfFile(m_handle, FILE_MAP_ALL_ACCESS, 0, 0, 0);
    return 0;
  }

  /*!
   * @if jp
   *
   * @brief ���L�������ւ̏�������
   *�B
   *
   * @param data �������ރf�[�^
   *
   * @return 0: ����, -1: ���s
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
  int SharedMemory::write(const char *data, const int pos, const int size)
  {
      if (!created())
        {
          return -1;
        }

      if (m_memory_size < size + pos)
        {
          return -1;
        }

      memcpy(&m_shm[pos],&data[0],size);
    
      return 0;
  }

  /*!
   * @if jp
   *
   * @brief ���L����������̓ǂݍ���
   *�B
   *
   * @param data �ǂݍ��ރf�[�^
   *
   * @return 0: ����, -1: ���s
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
  int SharedMemory::read(char* data, const int pos, const int size)
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
   * @brief ���L�������̍폜
   *�B
   *
   *
   * @return 0: ����, -1: ���s
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
        UnmapViewOfFile(m_shm);
    }
	else
	{
		return -1;
	}
    if(m_file_create)
    {
    	if (CloseHandle(m_handle) == 0)
    	{
    		return -1;
    	}
	else
	{
             m_handle = nullptr;
             m_file_create = false;
             return 0;
	}
    }
    return 0;

  }
  /*!
   * @if jp
   *
   * @brief �������̃T�C�Y�擾
   *�B
   *
   *
   * @return �T�C�Y
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
  unsigned int SharedMemory::get_size()
  {
	return m_memory_size;
  }
  /*!
   * @if jp
   *
   * @brief �������̃A�h���X�擾
   *�B
   *
   *
   * @return �A�h���X
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
   * @brief �������̎擾
   *
   *
   *
   * @return ������
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
   * @brief �t�@�C���폜(Windows�̏ꍇ�͉������Ȃ�)
   *
   *
   *
   * @return 0: ����, -1: ���s
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
	return -1;
  }


  /*!
  * @if jp
  *
  * @brief ���L�������ɐ����ς݂��̊m�F
  *
  *
  *
  * @return true: �쐬�ς�, false: ���쐬
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
	if (m_handle == NULL)
	{
		return false;
	}
	else
	{
		return true;
	}
  }




}