// -*- C++ -*-

/*!
 * @file  OpenSpliceOutPort_impl.cpp
 * @brief OpenSpliceOutPort_impl class
 * @date  $Date: 2019-01-31 03:08:03 $
 * @author Nobuhiko Miyamoto <n-miyamoto@aist.go.jp>
 *
 * Copyright (C) 2019
 *     Nobuhiko Miyamoto
 *     Robot Innovation Research Center,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *
 *     All rights reserved.
 *
 *
 */



#include "OpenSpliceInPort_Impl.h"
#include "OpenSpliceManager.h"
#include <iostream>



namespace RTC
{
    /*!
     * @if jp
     * @class OpenSpliceInPort_impl
     * @brief OpenSpliceInPort_impl クラス
     *
     * OpenSpliceのSubscriberを操作するための関数を定義したクラス
     *
     *
     * @since 2.0.0
     *
     * @else
     * @class OpenSpliceInPort_impl
     * @brief OpenSpliceInPort_impl class
     *
     *
     * @since 2.0.0
     *
     * @endif
     */
    class OpenSpliceInPort_impl : public OpenSpliceInPortBase
    {
    public:
        /*!
         * @if jp
         * @brief コンストラクタ
         *
         * コンストラクタ
         *
         * @else
         * @brief Constructor
         *
         * Constructor
         *
         * @endif
         */
        OpenSpliceInPort_impl();
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
        ~OpenSpliceInPort_impl() override;
        /*!
         * @if jp
         * @brief 初期化
         * OpenSpliceのreaderを初期化する
         * Participant、DataTypeSupport、Subscriber、Topicが初期化されていない場合は初期化する
         *
         * @param inportlistener データ受信時のコールバック関数
         * @param datatype データ型名
         * @param idlpath データ型を定義したIDLファイルのパス
         * @param topic トピック名
         * @param prop 設定プロパティ
         * @param endian true：リトルエンディアン、false：ビッグエンディアン
         * @param corbamode
         * @return true：初期化成功、false：問題発生
         *
         * @else
         * @brief
         *
         * @param inportlistener
         * @param datatype
         * @param idlpath
         * @param topic
         * @param prop 
         * @param endian
         * @param corbamode
         * @return
         *
         * @endif
         */
        bool init(OpenSpliceInPortListenerBase* inportlistener, std::string& datatype, std::string& idlpath, std::string& topic, coil::Properties& prop, bool endian = true, bool corbamode = true) override;
        /*!
         * @if jp
         * @brief 終了処理
         * readerの削除を行う
         *
         *
         * @else
         * @brief
         *
         *
         * @endif
         */
        void finalize() override;
    private:
        std::string m_topic;
        std::string m_dataType;
        DDS::DataReader_var m_reader;
        bool m_endian;
        std::string m_idlPath;
        bool m_corbamode;
        DDS::DataReaderListener_var m_listener;
        coil::Properties m_prop;
    };

    /*!
     * @if jp
     * @class SubListener
     * @brief SubListener クラス
     *
     * Subscriberのリスナ
     *
     *
     * @since 2.0.0
     *
     * @else
     * @class OpenSpliceInPort
     * @brief OpenSpliceInPort class
     *
     *
     *
     * @since 2.0.0
     *
     * @endif
     */
    class SubListener : public DDS::DataReaderListener
    {
    public:
        /*!
         * @if jp
         * @brief コンストラクタ
         *
         * コンストラクタ
         *
         * @else
         * @brief Constructor
         *
         * Constructor
         *
         * @endif
         */
        SubListener(OpenSpliceInPortListenerBase* provider, bool corbamode);
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
        ~SubListener();
        void on_requested_deadline_missed(DDS::DataReader_ptr reader, const DDS::RequestedDeadlineMissedStatus& status) override;
        void on_requested_incompatible_qos(DDS::DataReader_ptr reader, const DDS::RequestedIncompatibleQosStatus& status) override;
        void on_sample_rejected(DDS::DataReader_ptr reader, const DDS::SampleRejectedStatus& status) override;
        void on_liveliness_changed(DDS::DataReader_ptr reader, const DDS::LivelinessChangedStatus& status) override;
        void on_data_available(DDS::DataReader_ptr reader) override;
        void on_subscription_matched(DDS::DataReader_ptr reader, const DDS::SubscriptionMatchedStatus& status) override;
        void on_sample_lost(DDS::DataReader_ptr reader, const DDS::SampleLostStatus& status) override;

    private:
        OpenSpliceInPortListenerBase* m_provider;
        bool m_corbamode;
    };

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
    OpenSpliceInPortBase::~OpenSpliceInPortBase()
    {

    }

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
    OpenSpliceInPortListenerBase::~OpenSpliceInPortListenerBase()
    {

    }

    /*!
     * @if jp
     * @brief コンストラクタ
     *
     * コンストラクタ
     *
     * @else
     * @brief Constructor
     *
     * Constructor
     *
     * @endif
     */
    OpenSpliceInPort_impl::OpenSpliceInPort_impl(): m_endian(true), m_corbamode(true)
    {
    }
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
    OpenSpliceInPort_impl::~OpenSpliceInPort_impl()
    {
        if (m_reader != nullptr)
        {
            OpenSpliceManager& topicmgr = OpenSpliceManager::instance();
            topicmgr.deleteReader(m_reader);
        }
    }
    /*!
     * @if jp
     * @brief 初期化
     * OpenSpliceのreaderを初期化する
     * Participant、DataTypeSupport、Subscriber、Topicが初期化されていない場合は初期化する
     *
     * @param inportlistener データ受信時のコールバック関数
     * @param datatype データ型名
     * @param idlpath データ型を定義したIDLファイルのパス
     * @param topic トピック名
     * @param prop 設定プロパティ
     * @param endian true：リトルエンディアン、false：ビッグエンディアン
     * @param corbamode
     * @return true：初期化成功、false：問題発生
     *
     * @else
     * @brief
     *
     * @param inportlistener
     * @param datatype
     * @param idlpath
     * @param topic
     * @param prop 
     * @param endian
     * @param corbamode
     * @return
     *
     * @endif
     */
    bool OpenSpliceInPort_impl::init(OpenSpliceInPortListenerBase* inportlistener, std::string& datatype, std::string& idlpath, std::string& topic, coil::Properties& prop, bool endian, bool corbamode)
    {

        m_topic = topic;
        m_dataType = datatype;
        m_endian = endian;
        m_idlPath = idlpath;
        m_corbamode = corbamode;
        m_prop = prop;

        
        OpenSpliceManager& topicmgr = OpenSpliceManager::instance();

        
        if (!topicmgr.registerType(m_dataType, m_idlPath))
        {
            return false;
        }
        
        
        if (!topicmgr.createTopic(topic, m_dataType, m_prop))
        {
            return false;
        }
        

        m_listener = new SubListener(inportlistener, m_corbamode);

        
        m_reader = topicmgr.createReader(m_topic, m_listener.in(), m_prop);
    
        if (!OpenSpliceManager::checkHandle(m_reader.in(), "createReader"))
        {
            return false;
        }
        return true;
    }

    /*!
     * @if jp
     * @brief 終了処理
     * readerの削除を行う
     *
     *
     * @else
     * @brief
     *
     *
     * @endif
     */
    void OpenSpliceInPort_impl::finalize()
    {
        if (m_reader.in() != nullptr)
        {
            OpenSpliceManager& topicmgr = OpenSpliceManager::instance();
            topicmgr.deleteReader(m_reader.in());
        }
    }



    /*!
     * @if jp
     * @brief コンストラクタ
     *
     * コンストラクタ
     *
     * @else
     * @brief Constructor
     *
     * Constructor
     *
     * @endif
     */
    SubListener::SubListener(OpenSpliceInPortListenerBase* provider, bool corbamode)
        : m_provider(provider), m_corbamode(corbamode)
    {
    }

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
    SubListener::~SubListener()
    {
    }

    void SubListener::on_requested_deadline_missed(DDS::DataReader_ptr /*reader*/, const DDS::RequestedDeadlineMissedStatus& /*status*/)
    {
    }
    void SubListener::on_requested_incompatible_qos(DDS::DataReader_ptr /*reader*/, const DDS::RequestedIncompatibleQosStatus& /*status*/)
    {
    }
    void SubListener::on_sample_rejected(DDS::DataReader_ptr /*reader*/, const DDS::SampleRejectedStatus& /*status*/)
    {
    }
    void SubListener::on_liveliness_changed(DDS::DataReader_ptr /*reader*/, const DDS::LivelinessChangedStatus& /*status*/)
    {
    }
    void SubListener::on_data_available(DDS::DataReader_ptr reader)
    {
        DDS::ReturnCode_t status;

        OpenRTM_OpenSplice::CORBACdrDataDataReader_var cdr_reader = OpenRTM_OpenSplice::CORBACdrDataDataReader::_narrow(reader);

        if (!OpenSpliceManager::checkHandle(cdr_reader.in(), "failed narrow CORBACdrDataDataReader"))
        {
            return;
        }

        DDS::CDRSample received_data;
        DDS::SampleInfo info;
        status = cdr_reader->read_cdr(received_data, info, DDS::ANY_SAMPLE_STATE, DDS::ANY_VIEW_STATE, DDS::ANY_INSTANCE_STATE);

        if (!OpenSpliceManager::checkStatus(status, "failed read cdr data"))
        {
            return;
        }

        RTC::ByteData data;

        data.writeData(received_data.blob.get_buffer(), received_data.blob.length());
        m_provider->put(data);
    }
    void SubListener::on_subscription_matched(DDS::DataReader_ptr /*reader*/, const DDS::SubscriptionMatchedStatus& /*status*/)
    {
    }
    void SubListener::on_sample_lost(DDS::DataReader_ptr /*reader*/, const DDS::SampleLostStatus& /*status*/)
    {
    }

    /*!
     * @if jp
     * @brief OpenSpliceのSubscriberを操作するクラスの初期化を行う関数
     * OpenSpliceとomniORBのヘッダーファイルを同時にインクルードできないため、
     * OpenSpliceの処理はOpenSpliceInPort_impl.cppに記述して以下の関数から抽象クラスを取得する。
     *
     * @param inportlistener データ受信時のコールバック関数
     * @param datatype データ型名
     * @param idlpath データ型を定義したIDLファイルのパス
     * @param topic トピック名
     * @param prop 設定プロパティ
     * @param endian true：リトルエンディアン、false：ビッグエンディアン
     * @param corbamode 
     * @return OpenSpliceInPort_implオブジェクト
     *
     * @else
     * @brief
     *
     * @param inportlistener
     * @param datatype
     * @param idlpath
     * @param topic
     * @param prop 
     * @param endian
     * @param corbamode
     * @return
     *
     * @endif
     */
    OpenSpliceInPortBase* createOpenSpliceInPort(OpenSpliceInPortListenerBase* inportlistener, std::string& datatype, std::string& idlpath, std::string& topic, coil::Properties& prop, bool endian, bool corbamode)
    {
        OpenSpliceInPort_impl* subscriber = new OpenSpliceInPort_impl();
        if (!subscriber->init(inportlistener, datatype, idlpath, topic, prop, endian, corbamode))
        {
          delete subscriber;
          return nullptr;
        }

        return subscriber;
    }
} // namespace RTC


