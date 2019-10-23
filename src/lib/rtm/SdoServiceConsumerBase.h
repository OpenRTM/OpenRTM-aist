// -*- C++ -*-
/*!
 * @file SdoServiceConsumerBase.h
 * @brief SDO service consumer base class and its factory
 * @date $Date$
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2011
 *     Noriaki Ando
 *     Intelligent Systems Research Institute,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id$
 *
 */


#ifndef RTC_SDOSERVICECONSUMERBASE_H
#define RTC_SDOSERVICECONSUMERBASE_H

#include <coil/Factory.h>
#include <coil/Timer.h>
#include <rtm/RTObject.h>
#include <rtm/idl/SDOPackageStub.h>

namespace RTC
{
  /*!
   * @if jp
   *
   * @brief SdoServiceConsumer　基底クラス
   *
   * SDOで定義されているSDOサービスのコンシューマを実装するための基底ク
   * ラス。SDOサービスには、外部から提供サービスをRTC(SDO)側で利用する
   * SDOサービスコンシューマと、RTC(SDO)自身がSDOサービスを提供するSDO
   * サービスプロバイダがある。すべてのSDOサービスコンシューマはこの基
   * 底クラスを継承して実装される。
   *
   * このオブジェクトのライフサイクルは以下の通り。
   *
   * -# オブジェクトは通常、共有オブジェクト (so, DLL) としてコンパイル・
   *    リンクされる。
   * -# マネージャに対してロードされるとモジュール初期化関数によりオブ
   *    ジェクトファクトリが、SdoServiceConsumerFactory に対して登録さ
   *    れる。登録のキーにはサービスインターフェースの IFR (interface
   *    repository) ID が利用され、これによりサービスが区別される。
   * -# 外部のツールなどからサービスプロバイダがアタッチされた場合、サー
   *    ビスインターフェースの IFR ID が同一である SDO コンシューマがイ
   *    ンスタンス化され、提供されたSDOサービスの ServiceProfile (この
   *    構造体はサービスのオブジェクトリファレンスを含む) がコンシュー
   *    マにアタッチされる。
   * -# このときのアタッチシーケンスは以下の通り。
   *   -# SDO::get_configuration() により Configuration オブジェクトを取得
   *   -# Configuration::add_service_profile() により外部側の
   *      SdoServiceProvider を ServiceProfile により RTC に与える。
   *   -# RTC側でサービスを呼び出す必要が有った場合、この
   *      SdoServiceConsumer が保持しているサービスオブジェクトプロキシ
   *      に対して呼び出しを行う
   * -# 最終的に SdoServiceConsumer が不要になった場合には、
   *     Configuration::remove_service_profile() が id とともに呼び出され
   *     SDOサービスコンシューマが RTC から削除される。
   *
   * <pre>
   *
   *   [RTC] [SDO consumer] [Configuration]  [SDO service]    [Other]
   *     |          :             |                 |            |
   *     |          :         get_configuration()   |            |
   *     |<---------:-------------------------------|------------|
   *     |          :             |                 |            |
   *     |          :             |   add_service_profile(prof)  |
   *     |          :  create()   |<----------------|------------|
   *     |          |<------------|                 |            |
   *     |          |         call_sdo_service()    |            |
   *     |          |-------------|---------------->|            |
   *     |          |         call_sdo_service2()   |            |
   *     |          |-------------|---------------->|            |
   *     |          |             |       :         |            |
   *     |          |             |                 |            |
   *     |          |             | remove_service_profile(id)   |
   *     |          |  delete()   |<----------------|------------|
   *     |          x<------------|                 |            |
   *     |                        |                 x            x
   *
   * </pre>
   *
   * このクラスの実装に当たっては、少なくとも以下の純粋仮想関数を実装す
   * る必要がある。
   *
   * - init(): 初期化関数。与えられた RTObject および ServiceProfile か
   *   ら、当該オブジェクトを初期化する。
   * - reinit(): 再初期化関数。ServiceProfile は設定情報更新のため同一
   *   IDで呼び出されることが有るが、その際にこの関数が新たな
   *   ServiceProfile とともに呼び出される。関数内では、設定の変更など
   *   再初期化処理を実装する。
   * - getProfile(): 設定されたプロファイルを返す関数。
   * - finalize(): 終了処理。コンシューマがデタッチされる際に呼び出され
   *   る関数。関数内では終了処理を実装する。
   *
   * SdoServiceConsumer は通常共有オブジェクトとしてコンパイル・リンク
   * される。共有オブジェクトのエントリポイントは通常コンパイルされたファ
   * イル名の basename + "Init" にしておく。以下に、クラス名、ファイル
   * 名、エントリポイント関数名の推奨例を示す。
   *
   * - 実装クラス名: MySdoServiceConusmer
   * - ファイル名: MySdoServiceConsumer.h. MySdoServiceConsumer.cpp
   * - 共有オブジェクト名: MySdoServiceConsumer.so (or DLL)
   * - エントリポイント関数名: MySdoServiceConsumerInit()
   *
   * エントリポイント関数は通常以下のように、SdoServiceConsumerFactory
   * に当該コンシューマのファクトリ (と解体ファンクタ) を登録する以下の
   * ような関数になる。
   *
   * <pre>
   * extern "C"
   * {
   *   void MySdoServiceConsumerInit()
   *   {
   *     RTC::SdoServiceConsumerFactory& factory
   *       = RTC::SdoServiceConsumerFactory::instance();
   *     factory.addFactory(CORBA_Util::toRepositoryId<OpenRTM::MySdoService>(),
   *                        ::coil::Creator< ::RTC::SdoServiceConsumerBase,
   *                        ::RTC::MySdoServiceConsumer>);
   *   }
   * };
   * </pre>
   *
   * @else
   *
   * @endif
   *
   */
  class SdoServiceConsumerBase
  {
  public:
    /*!
     * @if jp
     * @brief 仮想デストラクタ
     * @else
     * @brief virtual destructor
     * @endif
     */
    virtual ~SdoServiceConsumerBase() = default;

    /*!
     * @if jp
     * @brief コンシューマクラスの初期化関数
     *
     * このオブジェクトの初期化を行う。外部からSDOサービスが
     * ServiceProfile とともにアタッチされると、SDOコンシューマがインス
     * タンス化され、その直後に SDO サービスがアタッチされた RTC と与え
     * られた ServiceProfile を引数としてこの関数が呼ばれる。
     *
     * 関数内では、ServiceProfile 内の SDO サービスリファレンスを
     * CorbaConsumer クラス等を利用しオブジェクト内に保持するとともに、
     * properties から設定内容を読み込みサービス固有の設定等を行う。与
     * えられたサービスのオブジェクトリファレンスが不正、あるいは
     * properties の内容が不正、等の場合は戻り値に false を返す。
     *
     * @param rtobj このオブジェクトがインスタンス化された RTC
     * @param profile 外部から与えられた SDO ServiceProfile
     * @return 与えられた SDO Service や ServiceProfile が不正の場合 false
     *
     * @else
     * @brief Initialization function of the consumer class
     *
     * @endif
     */
    virtual bool init(RTObject_impl& rtobj,
                      const SDOPackage::ServiceProfile& profile) = 0;
    /*!
     * @if jp
     * @brief コンシューマクラスの再初期化関数
     *
     * このオブジェクトの再初期化を行う。ServiceProfile には id フィー
     * ルドにセッション固有の UUID がセットされているが、同一の id の場
     * 合、properties に設定された設定情報の変更や、service フィールド
     * のサービスの参照の変更が行われる。その際に呼ばれるのがこの
     * reinit() 関数である。実装では、service フィールドのオブジェクト
     * リファレンスの同一性を確認し、異なっている場合保持しているリファ
     * レンスを更新する必要がある。また properties には新たな設定が与え
     * られている可能性があるので、内容を読み込み設定を更新する。
     *
     * @param profile 新たに与えられた SDO ServiceProfile
     * @return 不正な ServiceProfile が与えられた場合は false
     *
     * @else
     * @brief Reinitialization function of the consumer class
     *
     * @endif
     */
    virtual bool reinit(const SDOPackage::ServiceProfile& profile) = 0;

    /*!
     * @if jp
     * @brief ServiceProfile を返す
     *
     * init()/reinit()で与えられた ServiceProfile は通常オブジェクト内
     * で保持される。SDO Service 管理フレームワークは管理上このオブジェ
     * クトに対応する ServiceProfile を必要とするので、この関数では保持
     * されている ServiceProfile を返す。
     *
     * @return このオブジェクトが保持している ServiceProfile
     *
     * @else
     * @brief Getting ServiceProfile
     * @endif
     */
    virtual const SDOPackage::ServiceProfile& getProfile() const = 0;

    /*!
     * @if jp
     * @brief 終了処理
     *
     * SDOサービスがでタッチされる際に呼び出される終了処理用関数。サー
     * ビスのでタッチに際して、当該オブジェクトが保持するリソースを解放
     * するなどの処理を行う。
     *
     * @else
     * @brief Finalization
     *
     * @endif
     */
    virtual void finalize() = 0;
  };

    /*!
     * @if jp
     * @brief SdoServiceConsumerFactory の typedef
     * @else
     * @brief typedef of sdoServiceConsumerFactory
     * @endif
     */
  using SdoServiceConsumerFactory = ::coil::GlobalFactory< ::RTC::SdoServiceConsumerBase>;
} // namespace RTC


#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    /*!
     * @if jp
     * @brief クラステンプレートの明示的インスタンス化
     * @else
     * @brief Explicit instantiation of class template
     * @endif
     */
EXTERN template class DLL_PLUGIN coil::GlobalFactory<RTC::SdoServiceConsumerBase >;
#elif defined(__GNUC__)
EXTERN template class coil::GlobalFactory<RTC::SdoServiceConsumerBase>;
#endif

#endif  // RTC_SDOSERVICECONSUMERBASE_H
