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
     m_endian(true)//,
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
	  catch (PortableServer::POA::ServantNotActive&)
	  {
#ifdef ORB_IS_ORBEXPRESS
		  oe_out << e << oe_endl << oe_flush;
#else
		  //RTC_ERROR(("%s", e._name()));
#endif
	  }
	  catch (PortableServer::POA::WrongPolicy&)
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
	  if (!size_str.empty())
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
		  
		  m_shmem.create(shm_address, memory_size);
		  try
		  {
			  m_smInterface->open_memory(memory_size, CORBA::string_dup(shm_address));
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
	  if (!m_shmem.created())
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
  void SharedMemoryPort::write(ByteData& data)
  {
      CORBA::ULongLong data_size = static_cast<CORBA::ULongLong>(data.getDataLength());
	  if (data_size + sizeof(CORBA::ULongLong) > m_shmem.get_size())
	  {
		  int memory_size = static_cast<int>(data_size) + static_cast<int>(sizeof(CORBA::ULongLong));
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
      CORBA_CdrMemoryStream data_size_cdr;

      data_size_cdr.setEndian(m_endian);
      data_size_cdr.serializeCDR(data_size);
      int ret = m_shmem.write((char*)data_size_cdr.getBuffer(), 0, sizeof(CORBA::ULongLong));

	  if (ret == 0)
	  {
          m_shmem.write(reinterpret_cast<char*>(data.getBuffer()), sizeof(CORBA::ULongLong), data.getDataLength());
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
	void SharedMemoryPort::read(ByteData& data)
  {
	  //CORBA::Octet data_size_str[sizeof(CORBA::ULongLong)];
	  //int ret = m_shmem.read((char*)data_size_str, 0, sizeof(CORBA::ULongLong));
	  //if (ret == 0)
	  if (m_shmem.created())
	  {
          CORBA_CdrMemoryStream data_size_cdr;
          CORBA::ULongLong data_size = 0;
          data.isLittleEndian(m_endian);
          data_size_cdr.setEndian(m_endian);
          data_size_cdr.writeCdrData(reinterpret_cast<unsigned char*>(&(m_shmem.get_data()[0])), sizeof(CORBA::ULongLong));
          data_size_cdr.deserializeCDR(data_size);

          //CORBA::Octet *shm_data = new CORBA::Octet[data_size];
          //ret = m_shmem.read((char*)shm_data, sizeof(CORBA::ULongLong), (int)data_size);
		 
          /*if (ret == 0)
          {
              	 
              data.put_octet_array(&(shm_data[0]), (int)data_size);
          }*/
          data.writeData((unsigned char*)(&m_shmem.get_data()[sizeof(CORBA::ULongLong)]), static_cast<unsigned long>(data_size));
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
} // namespace RTC
