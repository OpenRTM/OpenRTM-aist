// -*- C++ -*-
/*!
 * @file  OpenSpliceOutPort_impl.h
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

#ifndef RTC_OPENSPLICEOUTPORT_IMPL_H
#define RTC_OPENSPLICEOUTPORT_IMPL_H

#define LIBRARY_EXPORTS
#include <rtm/ByteData.h>
#undef LIBRARY_EXPORTS
#include <coil/Properties.h>
#include <string>



namespace RTC
{
    /*!
     * @if jp
     * @class OpenSpliceOutPortBase
     * @brief OpenSpliceOutPortBase クラス
     *
     * OpenSpliceのPublisherを操作するための関数を定義したクラスの抽象クラス
     *
     *
     * @since 2.0.0
     *
     * @else
     * @class OpenSpliceOutPortBase
     * @brief OpenSpliceOutPortBase class
     *
     *
     * @since 2.0.0
     *
     * @endif
     */
    class OpenSpliceOutPortBase
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
        virtual ~OpenSpliceOutPortBase() = 0;
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
        virtual bool init(std::string& datatype, std::string& idlpath, std::string& topic, coil::Properties& prop, bool endian = true, bool corbamode = true) = 0;
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
        virtual bool write(RTC::ByteData& data) = 0;
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
        virtual void finalize() = 0;
    };

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
    OpenSpliceOutPortBase* createOpenSpliceOutPort(std::string& datatype, std::string& idlpath, std::string& topic, coil::Properties &prop, bool endian = true, bool corbamode = true);
}



#endif // RTC_OPENSPLICEOUTPORT_H

