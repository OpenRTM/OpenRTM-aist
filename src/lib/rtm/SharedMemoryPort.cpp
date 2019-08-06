// -*- C++ -*-
/*!
 * @file  SharedMemoryPort.cpp
 * @brief SharedMemoryPort class
 * @date $Date: 2017-10-29 07:57:18 $
 * @author Nobuhiko Miyamoto <n-miyamoto@aist.go.jp>
 *
 * Copyright (C) 2009-2010
 *     Noriaki Ando
 *     Robot Innovation Research Center,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 *
 */

#include <rtm/SharedMemoryPort.h>
#include <rtm/Manager.h>

#ifdef WIN32
#pragma warning( disable : 4290 )
#endif

namespace RTC
{
  /*!
   * @if jp
   * @brief コンストラクタ
   * @else
   * @brief Constructor
   * @endif
   */
	SharedMemoryPort::SharedMemoryPort()
   : m_smInterface(OpenRTM::PortSharedMemory::_nil()),
     m_endian(true), m_created_count(0)
     //rtclog("SharedMemoryPort")
  {

  }
  
  /*!
   * @if jp
   * @brief デストラクタ
   * @else
   * @brief Destructor
   * @endif
   */
	SharedMemoryPort::~SharedMemoryPort()
  {
	//RTC_PARANOID(("~SharedMemoryPort()"));
	  try
	  {
		  PortableServer::ObjectId_var oid;
#ifdef ORB_IS_OMNIORB
		  oid = ::RTC::Manager::instance().theShortCutPOA()->servant_to_id(this);
		  ::RTC::Manager::instance().theShortCutPOA()->deactivate_object(oid);
#else
		  oid = _default_POA()->servant_to_id(this);
		  _default_POA()->deactivate_object(oid);
#endif
	  }
	  catch (PortableServer::POA::ServantNotActive &e)
	  {
#ifdef ORB_IS_ORBEXPRESS
		  oe_out << e << oe_endl << oe_flush;
#else
		  //RTC_ERROR(("%s", e._name()));
#endif
	  }
	  catch (PortableServer::POA::WrongPolicy &e)
	  {
#ifdef ORB_IS_ORBEXPRESS
		  oe_out << e << oe_endl << oe_flush;
#else
		  //RTC_ERROR(("%s", e._name()));
#endif
	  }
	  catch (...)
	  {
		  // never throws exception
		  //RTC_ERROR(("Unknown exception caught."));
	  }
  }



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
  int SharedMemoryPort::string_to_MemorySize(std::string size_str)
  {
	  int memory_size = DEFAULT_MEMORY_SIZE;
	  std::string size_str_n = coil::normalize(size_str);
	  if (size_str.size() > 0)
	  {
		  std::string unit_str_M = "M";
		  unit_str_M = coil::normalize(unit_str_M);
		  std::string unit_str_k = "k";
		  unit_str_k = coil::normalize(unit_str_k);

		  std::string value_str = size_str_n.substr(0, size_str_n.size() - 1);
		  int value = 0;
		  if(coil::stringTo(value, value_str.c_str()))
		  {
			  if (size_str_n.substr(size_str_n.size() - 1, 1) == unit_str_M)
			  {
				  memory_size = 1048576 * value;
			  }
			  else if (size_str_n.substr(size_str_n.size() - 1, 1) == unit_str_k)
			  {
				  
				  memory_size = 1024 * value;
			  }
			  else
			  {
				  memory_size = value;
			  }
		  }
	  }

	  return memory_size;
  }
  /*!
  * @if jp
  * @brief 共有メモリの初期化
  * windowsではページングファイル上に領域を確保する
  * Linuxでは/dev/shm以下にファイルを作成する
  * 作成したファイルの内容を仮想アドレスにマッピングする
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
	void SharedMemoryPort::create_memory(::CORBA::ULongLong memory_size, const char *shm_address)
    throw (CORBA::SystemException)
  {
	  if (!m_shmem.created())
	  {
		  std::string address = shm_address + coil::otos(m_created_count);
		  m_created_count++;
		  m_shmem.create(address, memory_size);
		  try
		  {
			  m_smInterface->open_memory(memory_size, CORBA::string_dup(address.c_str()));
		  }
		  catch (...)
		  {
		  }
	  }
  }
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
	void SharedMemoryPort::open_memory(::CORBA::ULongLong memory_size, const char * shm_address)
    throw (CORBA::SystemException)
  {
	  
	  m_shmem.open(shm_address, memory_size);
  }
  /*!
  * @if jp
  * @brief マッピングした共有メモリをアンマップする
  *
  * @param unlink Linuxで/dev/shm以下に作成したファイルを削除する場合にTrueにする
  *
  *
  *
  * @else
  * @brief 
  *
  * @param unlink
  *
  * @endif
  */
	void SharedMemoryPort::close_memory(::CORBA::Boolean unlink)
    throw (CORBA::SystemException)
  {
	  if (m_shmem.created())
	  {
		  m_shmem.close();
		  if (unlink)
		  {
			  m_shmem.unlink();
		  }
		  try
		  {
			  m_smInterface->close_memory(false);
		  }
		  catch (...)
		  {
		  }
	  }
  }
  /*!
  * @if jp
  * @brief データを書き込む
  * 先頭8byteにデータサイズを書き込み、その後ろにデータを書き込む
  * 設定したデータサイズが共有メモリのサイズを上回った場合、共有メモリの初期化を行う
  *
  * @param data 
  *
  *
  *
  * @else
  * @brief 
  *
  * @param data 書き込むデータ
  *
  * @endif
  */
	void SharedMemoryPort::write(cdrMemoryStream& data)
  {
	  CORBA::ULongLong data_size = (CORBA::ULongLong)data.bufSize();
	  if (data_size + sizeof(CORBA::ULongLong) > m_shmem.get_size())
	  {
		  int memory_size = (int)data_size + (int)sizeof(CORBA::ULongLong);
		  if (!CORBA::is_nil(m_smInterface))
		  {
			  try
			  {
				  m_smInterface->close_memory(false);
			  }
			  catch (...)
			  {
			  }
		  }
		  close_memory(true);
		  create_memory(memory_size, m_shmem.get_addresss().c_str());
	  }
	  cdrMemoryStream data_size_cdr;

#ifdef ORB_IS_ORBEXPRESS
	  data_size_cdr.is_little_endian(m_endian);
	  data_size_cdr.cdr << data_size;
	  int ret = m_shmem.write(const_cast<char*>(data_size_cdr.cdr.get_buffer()), 0, sizeof(CORBA::ULongLong));
#elif defined(ORB_IS_TAO)
	  data_size_cdr.cdr << data_size;
	  int ret = m_shmem.write((char*)data_size_cdr.cdr.buffer(), 0, sizeof(CORBA::ULongLong));
#else
	  data_size_cdr.setByteSwapFlag(m_endian);
	  data_size >>= data_size_cdr;
	  int ret = m_shmem.write((char*)data_size_cdr.bufPtr(), 0, sizeof(CORBA::ULongLong));
#endif
	  if (ret == 0)
	  {
#ifdef ORB_IS_ORBEXPRESS
		  m_shmem.write(const_cast<char*>(data.get_buffer()), sizeof(CORBA::ULongLong), data.cdr.size_written());
#elif defined(ORB_IS_TAO)
		  m_shmem.write((char*)data.buffer(), sizeof(CORBA::ULongLong), data.cdr.length());
#else
		 
		  m_shmem.write((char*)data.bufPtr(), sizeof(CORBA::ULongLong), data.bufSize());
#endif
	  }

  }
  /*!
  * @if jp
  * @brief データを読み込む
  *
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
	void SharedMemoryPort::read(cdrMemoryStream& data)
  {
	  //CORBA::Octet data_size_str[sizeof(CORBA::ULongLong)];
	  //int ret = m_shmem.read((char*)data_size_str, 0, sizeof(CORBA::ULongLong));
	  //if (ret == 0)
	  if (m_shmem.created())
	  {
		  cdrMemoryStream data_size_cdr;
		  CORBA::ULongLong data_size;
#ifdef ORB_IS_ORBEXPRESS
		  data.is_little_endian(m_endian);
		  data_size_cdr.is_little_endian(m_endian);
		  data_size_cdr.write_array_1(data_size_str[0], sizeof(CORBA::ULongLong));
		  data_size_cdr.cdr >> data_size;
#elif defined(ORB_IS_TAO)
		  data_size_cdr.write_octet_array(data_size_str[0], sizeof(CORBA::ULongLong));
		  TAO_InputCDR(data_size_cdr.cdr) >> data_size;
#else
		  data.setByteSwapFlag(m_endian);
		  data_size_cdr.setByteSwapFlag(m_endian);
		  //data_size_cdr.put_octet_array(&(data_size_str[0]), sizeof(CORBA::ULongLong));
		  data_size_cdr.put_octet_array((CORBA::Octet *)&(m_shmem.get_data()[0]), sizeof(CORBA::ULongLong));
		  data_size <<= data_size_cdr;


#endif
		  //CORBA::Octet *shm_data = new CORBA::Octet[data_size];
		  //ret = m_shmem.read((char*)shm_data, sizeof(CORBA::ULongLong), (int)data_size);
		 
		  /*if (ret == 0)
		  {
			 
			  data.put_octet_array(&(shm_data[0]), (int)data_size);
		  }*/
		  data.put_octet_array((CORBA::Octet *)&(m_shmem.get_data()[sizeof(CORBA::ULongLong)]), (int)data_size);
		  //delete shm_data;
	  }

  }
  /*!
  * @if jp
  * @brief データを読み込む
  *
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
	void SharedMemoryPort::setInterface(::OpenRTM::PortSharedMemory_ptr sm)
    throw (CORBA::SystemException)
  {
	  m_smInterface = ::OpenRTM::PortSharedMemory::_narrow(sm);
  }
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
	void SharedMemoryPort::setEndian(::CORBA::Boolean endian)
    throw (CORBA::SystemException)
  {
	  m_endian = endian;
	  if (!CORBA::is_nil(m_smInterface))
	  {
		  try
		  {
			  m_smInterface->setEndian(endian);
		  }
		  catch (...)
		  {

		  }
	  }
  }
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
	::OpenRTM::PortStatus SharedMemoryPort::put()
    throw (CORBA::SystemException)
  {
	  return ::OpenRTM::PORT_OK;
  }
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
	::OpenRTM::PortStatus SharedMemoryPort::get()
    throw (CORBA::SystemException)
  {
	  return ::OpenRTM::PORT_OK;
  }

	::OpenRTM::PortSharedMemory_ptr SharedMemoryPort::getObjRef()
	{
		//m_objref = this->_this();
		//return ::OpenRTM::PortSharedMemory::_duplicate(m_objref);
		return this->_this();
	}
};
