// -*- C++ -*-
/*!
 * @file  InPortSHMConsumer.h
 * @brief InPortSHMConsumer class
 * @date  $Date: 2007-12-31 03:08:03 $
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * @date  $Date: 2017-10-30 07:49:59 $
 * @author Nobuhiko Miyamoto <n-miyamoto@aist.go.jp>
 *
 * Copyright (C) 2017 Nobuhiko Miyamoto
 *     Robot Innovation Research Center,
 *     Intelligent Systems Research Institute,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 */

#ifndef RTC_INPORTSHMCONSUMER_H
#define RTC_INPORTSHMCONSUMER_H



#include <rtm/SharedMemoryPort.h>
#include <rtm/CorbaConsumer.h>
#include <rtm/InPortConsumer.h>
#include <rtm/Manager.h>
#include <rtm/InPortCorbaCdrConsumer.h>

namespace RTC
{
  /*!
   * @if jp
   * @class InPortSHMConsumer
   * @brief InPortSHMConsumer クラス
   *
   * InPortConsumer 
   *
   * 通信手段に 共有メモリ を利用した入力ポートコンシューマの実装クラス。
   *
   * @since 1.2.0
   *
   * @else
   * @class InPortSHMConsumer
   * @brief InPortSHMConsumer class
   *
   * 
   *
   * @since 1.2.0
   *
   * @endif
   */
  class InPortSHMConsumer
       : public InPortConsumer,
         public CorbaConsumer< ::OpenRTM::PortSharedMemory >
  {
  public:
    /*!
     * @if jp
     * @brief コンストラクタ
     *
     * コンストラクタ
     *
     * @param buffer 当該コンシューマに割り当てるバッファオブジェクト
     *
     * @else
     * @brief Constructor
     *
     * Constructor
     *
     * @param buffer The buffer object that is attached to this Consumer
     *
     * @endif
     */
    InPortSHMConsumer();
    
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
    ~InPortSHMConsumer() override;

    /*!
     * @if jp
     * @brief 設定初期化
     *
     * 
     * 
     * @param prop 設定情報
     *
     * @else
     *
     * @brief Initializing configuration
     *
     *
     *
     * @param prop Configuration information
     *
     * @endif
     */
    void init(coil::Properties& prop) override;
    /*!
     * @if jp
     * @brief 接続先へのデータ送信
     *
     * 接続先のポートへデータを送信するための純粋仮想関数。
     * 
     * 
     *
     * @param data 送信するデータ
     * @return リターンコード
     *
     * @else
     * @brief Send data to the destination port
     *
     * 
     *
     * @endif
     */
    DataPortStatus put(ByteData& data) override;
    /*!
     * @if jp
     * @brief 
     * 
     * 
     *
     * @param obj 
     * @return 
     *
     * @else
     * @brief 
     *
     * 
     *
     * @endif
     */
    bool setObject(CORBA::Object_ptr obj) override;
    void publishInterfaceProfile(SDOPackage::NVList& properties) override;
    bool subscribeInterface(const SDOPackage::NVList& properties) override;
    void unsubscribeInterface(const SDOPackage::NVList& properties) override;
  
private:
    bool subscribeFromIor(const SDOPackage::NVList& properties);
    bool subscribeFromRef(const SDOPackage::NVList& properties);
    bool unsubscribeFromIor(const SDOPackage::NVList& properties);
    bool unsubscribeFromRef(const SDOPackage::NVList& properties);

protected:
    static DataPortStatus convertReturnCode(OpenRTM::PortStatus ret);

   coil::Properties m_properties;
   std::mutex m_mutex;
   std::string m_shm_address;
   SharedMemoryPort m_shmem;
   int m_memory_size{0};
   bool m_endian{true};
   mutable Logger rtclog{"InPortSHMConsumer"};
  };
} // namespace RTC

extern "C"
{
  /*!
   * @if jp
   * @brief モジュール初期化関数
   *
   * InPortSHMConsumer のファクトリを登録する初期化関数。
   *
   * @else
   * @brief Module initialization
   *
   * This initialization function registers InPortSHMConsumer's factory.
   *
   * @endif
   */
  void InPortSHMConsumerInit(void);
}

#endif // RTC_INPORTCORBACDRCONSUMER_H

