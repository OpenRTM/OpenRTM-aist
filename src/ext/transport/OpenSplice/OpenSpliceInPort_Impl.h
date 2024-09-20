// -*- C++ -*-
/*!
 * @file  OpenSpliceInPort_impl.h
 * @brief OpenSpliceInPort_impl class
 * @date  $Date: 2019-02-21 03:08:03 $
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

#ifndef RTC_OPENSPLICEINPORT_IMPL_H
#define RTC_OPENSPLICEINPORT_IMPL_H

#define LIBRARY_EXPORTS
#include <rtm/ByteData.h>
#undef LIBRARY_EXPORTS
#include <coil/Properties.h>
#include <string>


namespace RTC
{
    /*!
     * @if jp
     * @class OpenSpliceInPortListenerBase
     * @brief OpenSpliceInPortListenerBase クラス
     *
     * OpenSpliceのSubscriberでデータ受信時に呼び出すコールバック関数を定義したベースクラス
     * InPortCunsumer側でコネクタにデータを書き込むための関数を定義したクラスを定義する必要がある
     *
     *
     * @since 2.0.0
     *
     * @else
     * @class OpenSpliceInPortListenerBase
     * @brief OpenSpliceInPortListenerBase class
     *
     *
     * @since 2.0.0
     *
     * @endif
     */
    class OpenSpliceInPortListenerBase
    {
    public:
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
        virtual ~OpenSpliceInPortListenerBase();

        /*!
         * @if jp
         * @brief コネクタにデータを書き込む
         *
         * @param data 受信データ
         *
         * @else
         * @brief 
         *
         * @param data
         *
         * @endif
         */
        virtual void put(RTC::ByteData& data) = 0;
    };

    /*!
     * @if jp
     * @class OpenSpliceInPortBase
     * @brief OpenSpliceInPortBase クラス
     *
     * OpenSpliceのSubscriberを操作するための関数を定義したクラスの抽象クラス
     *
     *
     * @since 2.0.0
     *
     * @else
     * @class OpenSpliceInPortBase
     * @brief OpenSpliceInPortBase class
     *
     *
     * @since 2.0.0
     *
     * @endif
     */
    class OpenSpliceInPortBase
    {
    public:
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
        virtual ~OpenSpliceInPortBase() = 0;
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
        virtual bool init(OpenSpliceInPortListenerBase* inportlistener, std::string& datatype, std::string& idlpath, std::string& topic, coil::Properties& prop, bool endian=true, bool corbamode=true) = 0;
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
        virtual void finalize() = 0;
    private:
    };

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
    OpenSpliceInPortBase* createOpenSpliceInPort(OpenSpliceInPortListenerBase* inportlistener, std::string& datatype, std::string& idlpath, std::string& topic, coil::Properties& prop, bool endian = true, bool corbamode = true);
}


#endif // RTC_OPENSPLICEINPORT_H

