// -*- C++ -*-
/*!
 * @file  OpenSpliceOutPort.cpp
 * @brief OpenSpliceOutPort class
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



#include "OpenSpliceOutPort_Impl.h"
#include "OpenSpliceManager.h"
#include <iostream>





namespace RTC
{
    /*!
     * @if jp
     * @class OpenSpliceOutPortBase
     * @brief PubListener クラス
     *
     * Writerのリスナクラス
     *
     *
     * @since 2.0.0
     *
     * @else
     * @class PubListener
     * @brief PubListener class
     *
     *
     * @since 2.0.0
     *
     * @endif
     */
    class PubListener : public DDS::DataWriterListener
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
         * @brief Constructor
         *
         * Constructor
         *
         *
         * @endif
         */
        PubListener();

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
        ~PubListener();

        void on_offered_deadline_missed(DDS::DataWriter_ptr writer, const DDS::OfferedDeadlineMissedStatus& status) override;
        void on_offered_incompatible_qos(DDS::DataWriter_ptr writer, const DDS::OfferedIncompatibleQosStatus& status) override;
        void on_liveliness_lost(DDS::DataWriter_ptr writer, const DDS::LivelinessLostStatus& status) override;
        void on_publication_matched(DDS::DataWriter_ptr writer, const DDS::PublicationMatchedStatus& status) override;
    };

    /*!
     * @if jp
     * @class OpenSpliceOutPort_impl
     * @brief OpenSpliceOutPort_impl クラス
     *
     * OpenSpliceのPublisherを操作するための関数を定義したクラス
     *
     *
     * @since 2.0.0
     *
     * @else
     * @class OpenSpliceOutPort_impl
     * @brief OpenSpliceOutPort_impl class
     *
     *
     * @since 2.0.0
     *
     * @endif
     */
    class OpenSpliceOutPort_impl : public OpenSpliceOutPortBase
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
        OpenSpliceOutPort_impl();
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
        ~OpenSpliceOutPort_impl() override;
        /*!
         * @if jp
         * @brief 初期化
         * OpenSpliceのreaderを初期化する
         * Participant、DataTypeSupport、Publisher、Topicが初期化されていない場合は初期化する
         *
         * @param datatype データ型名
         * @param idlpath データ型を定義したIDLファイルのパス
         * @param topic トピック名
         * @param prop 設定プロパティ
         * @param endian true：リトルエンディアン、false：ビッグエンディアン
         * @param corbamode true：送信データの先頭に4bitのヘッダーを付加する
         * @return true：初期化成功、false：問題発生
         *
         * @else
         * @brief
         *
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
        bool init(std::string& datatype, std::string& idlpath, std::string& topic, coil::Properties& prop, bool endian = true, bool corbamode = true) override;
        /*!
         * @if jp
         * @brief データ送信
         *
         * @param data 送信データ
         * @return true：送信成功、false：問題発生
         * writerの未初期化、送信エラー等でfalseを返す
         *
         * @else
         * @brief
         *
         * @param data
         * @return
         *
         * @endif
         */
        bool write(RTC::ByteData& data) override;
        /*!
         * @if jp
         * @brief 終了処理
         * writerの削除を行う
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
        DDS::DataWriter_var m_writer;
        bool m_endian;
        std::string m_idlPath;
        bool m_corbamode;
        DDS::DataWriterListener_var m_listener;
        coil::Properties m_prop;
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
    OpenSpliceOutPortBase::~OpenSpliceOutPortBase()
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
    OpenSpliceOutPort_impl::OpenSpliceOutPort_impl() : m_endian(true), m_corbamode(true)
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
    OpenSpliceOutPort_impl::~OpenSpliceOutPort_impl()
    {
        if (m_writer != nullptr)
        {
            OpenSpliceManager& topicmgr = OpenSpliceManager::instance();
            topicmgr.deleteWriter(m_writer);
        }
    }
    /*!
     * @if jp
     * @brief 初期化
     * OpenSpliceのreaderを初期化する
     * Participant、DataTypeSupport、Publisher、Topicが初期化されていない場合は初期化する
     *
     * @param datatype データ型名
     * @param idlpath データ型を定義したIDLファイルのパス
     * @param topic トピック名
     * @param prop 設定プロパティ
     * @param endian true：リトルエンディアン、false：ビッグエンディアン
     * @param corbamode true：送信データの先頭に4bitのヘッダーを付加する
     * @return true：初期化成功、false：問題発生
     *
     * @else
     * @brief
     *
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
    bool OpenSpliceOutPort_impl::init(std::string& datatype, std::string& idlpath, std::string& topic, coil::Properties& prop, bool endian, bool corbamode)
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

        m_listener = new PubListener();
        
        DDS::DataWriter_var writer = topicmgr.createWriter(m_topic, m_listener.in(), m_prop);

        if (!OpenSpliceManager::checkHandle(writer.in(), "createWriter"))
        {
            return false;
        }

        m_writer = OpenRTM_OpenSplice::CORBACdrDataDataWriter::_narrow(writer.in());

        if (!OpenSpliceManager::checkHandle(m_writer, "CORBACdrDataDataWriter::_narrow(writer)"))
        {
            return false;
        }

        return true;
    }
    /*!
     * @if jp
     * @brief 終了処理
     * writerの削除を行う
     *
     *
     * @else
     * @brief
     *
     *
     * @endif
     */
    void OpenSpliceOutPort_impl::finalize()
    {
        if (m_writer.in() != nullptr)
        {
            OpenSpliceManager& topicmgr = OpenSpliceManager::instance();
            topicmgr.deleteWriter(m_writer.in());
        }
    }
    /*!
     * @if jp
     * @brief データ送信
     *
     * @param data 送信データ
     * @return true：送信成功、false：問題発生
     * writerの未初期化、送信エラー等でfalseを返す
     *
     * @else
     * @brief
     *
     * @param data
     * @return
     *
     * @endif
     */
    bool OpenSpliceOutPort_impl::write(RTC::ByteData& data)
    {

        if (!OpenSpliceManager::checkHandle(m_writer, "createWriter"))
        {
            return false;
        }

        ::DDS::CDRSample sample;

        if (m_corbamode)
        {
            sample.blob.length(data.getDataLength() + 4);

            DDS::Octet_ptr b = sample.blob.get_buffer();
            b[0] = 0;
            if (m_endian)
            {
                b[1] = 1;
            }
            else
            {
                b[1] = 0;
            }
            b[2] = 0;
            b[3] = 0;

            memcpy(b + 4, data.getBuffer(), data.getDataLength());
        }
        else
        {
            sample.blob.length(data.getDataLength());
            DDS::Octet_ptr b = sample.blob.get_buffer();
            memcpy(b, data.getBuffer(), data.getDataLength());
        }

        OpenRTM_OpenSplice::CORBACdrDataDataWriter_var writer = OpenRTM_OpenSplice::CORBACdrDataDataWriter::_narrow(m_writer);
        if (!OpenSpliceManager::checkHandle(writer, "writer narrow"))
        {
            return false;
        }

        DDS::ReturnCode_t result = writer->write_cdr(sample, DDS::HANDLE_NIL);

        if (!OpenSpliceManager::checkStatus(result, "write_cdr() failed"))
        {
            return false;
        }
        else
        {
            return true;
        }
    }

    /*!
     * @if jp
     * @brief コンストラクタ
     *
     * コンストラクタ
     *
     *
     * @else
     * @brief Constructor
     *
     * Constructor
     *
     *
     * @endif
     */
    PubListener::PubListener()
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
    PubListener::~PubListener()
    {
    }

    void PubListener::on_offered_deadline_missed(DDS::DataWriter_ptr /*writer*/, const DDS::OfferedDeadlineMissedStatus& /*status*/)
    {
    }
    void PubListener::on_offered_incompatible_qos(DDS::DataWriter_ptr /*writer*/, const DDS::OfferedIncompatibleQosStatus& /*status*/)
    {
    }
    void PubListener::on_liveliness_lost(DDS::DataWriter_ptr /*writer*/, const DDS::LivelinessLostStatus& /*status*/)
    {
    }
    void PubListener::on_publication_matched(DDS::DataWriter_ptr /*writer*/, const DDS::PublicationMatchedStatus& /*status*/)
    {
    }

    /*!
     * @if jp
     * @brief OpenSpliceのPublisherを操作するクラスの初期化を行う関数
     * OpenSpliceとomniORBのヘッダーファイルを同時にインクルードできないため、
     * OpenSpliceの処理はOpenSpliceOutPort_impl.cppに記述して以下の関数から抽象クラスを取得する。
     *
     * @param datatype データ型名
     * @param idlpath データ型を定義したIDLファイルのパス
     * @param topic トピック名
     * @param prop 設定プロパティ
     * @param endian true：リトルエンディアン、false：ビッグエンディアン
     * @param corbamode true：送信データの先頭に4bitのヘッダーを付加する
     * @return OpenSpliceOutPort_implオブジェクト
     *
     * @else
     * @brief
     *
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
    OpenSpliceOutPortBase* createOpenSpliceOutPort(std::string& datatype, std::string& idlpath, std::string& topic, coil::Properties& prop, bool endian, bool corbamode)
    {
        OpenSpliceOutPort_impl* publisher = new OpenSpliceOutPort_impl();
        if (!publisher->init(datatype, idlpath, topic, prop, endian, corbamode))
        {
          delete publisher;
          return nullptr;
        }

        return publisher;
    }

} // namespace RTC