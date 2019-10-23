// -*- C++ -*-
/*!
 * @file SdoServiceProviderBase.h
 * @brief SDO service provider base class and its factory
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


#ifndef RTC_SDOSERVICEPROVIDERBASE_H
#define RTC_SDOSERVICEPROVIDERBASE_H

#include <coil/Factory.h>
#include <coil/Timer.h>
#include <rtm/RTC.h>
#include <rtm/RTObject.h>
#include <rtm/idl/SDOPackageStub.h>

namespace RTC
{
  /*!
   * @if jp
   *
   * @brief SdoServiceProvider　基底クラス
   *
   * SDOで定義されているSDOサービスのプロバイダを実装するための基底クラ
   * ス。SDOサービスには、外部から提供サービスをRTC(SDO)側で利用する
   * SDOサービスコンシューマと、RTC(SDO)自身がSDOサービスを提供するSDO
   * サービスプロバイダがある。すべてのSDOサービスプロバイダはこの基底
   * クラスを継承して実装される。
   *
   * このオブジェクトのライフサイクルは以下の通り。
   *
   * -# オブジェクトは通常、共有オブジェクト (so, DLL) としてコンパイル・
   *    リンクされる。
   * -# マネージャに対してロードされるとモジュール初期化関数によりオブ
   *    ジェクトファクトリが、SdoServiceProviderFactory に対して登録さ
   *    れる。登録のキーにはサービスインターフェースの IFR (interface
   *    repository) ID が利用され、これによりサービスが区別される。
   * -# rtc.conf等のコンフィギュレーション指定により、有効化することが
   *    指定されているサービスインプロバイダは、RTCの起動と同時にインス
   *    タンス化される。
   * -# インスタンス化後、初期化関数 init() が呼ばれる。引数には当該サー
   *    ビスのためのコンフィギュレーションオプションが coil::Propertyに
   *    より渡される。
   * -# インスタンス化されたSDOサービスプロバイダは
   *    SDO::get_sdo_service() により外部からアクセスされる。このと
   *    き、サービスを指定するIDはIFR IDと同じである。このときのアタッ
   *    チシーケンスは以下の通り。
   * -# RTCがfinalizeされ解体されると同時にSDOサービスプロバイダも解体
   *    されるが、その際にはSdoServiceProviderBase::finalize()がコール
   *    されるので、ここでリソースの解放など終了処理を行う。
   *
   * <pre>
   *
   *   [RTC]      [SDO service]               [Other]
   *     |              :                        |
   *     | instantiate  :                        |
   *     |------------->:                        |
   *     |    init()    |                        |
   *     |------------->|                        |
   *     |              | get_service_profiles() |
   *     |<--------------------------------------|
   *     |              |    get_sdo_service()   |
   *     |<--------------------------------------|
   *     |              |        use service     |
   *     |              |<-----------------------|
   *     |              |                        |
   *     |  finalize()  |                        |
   *     |------------->x                        |
   *     x              x                        |
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
   * SdoServiceProvider は通常共有オブジェクトとしてコンパイル・リンク
   * される。共有オブジェクトのエントリポイントは通常コンパイルされたファ
   * イル名の basename + "Init" にしておく。以下に、クラス名、ファイル
   * 名、エントリポイント関数名の推奨例を示す。
   *
   * - 実装クラス名: MySdoServiceProvider 
   * - ファイル名: MySdoServiceProvider.h. MySdoServiceProvider.cpp
   * - 共有オブジェクト名: MySdoServiceProvider.so (or DLL)
   * - エントリポイント関数名: MySdoServiceProviderInit()
   *
   * エントリポイント関数は通常以下のように、SdoServiceProviderFactory
   * に当該コンシューマのファクトリ (と解体ファンクタ) を登録する以下の
   * ような関数になる。
   *
   * <pre>
   * extern "C"
   * {
   *   void MySdoServiceProviderInit()
   *   {
   *     RTC::SdoServiceProviderFactory& factory
   *       = RTC::SdoServiceProviderFactory::instance();
   *     factory.addFactory(CORBA_Util::toRepositoryId<OpenRTM::MySdoService>(),
   *                        ::coil::Creator< ::RTC::SdoServiceProviderBase,
   *                        ::RTC::MySdoServiceProvider>);
   *   }
   * };
   * </pre>
   *
   * @else
   *
   * @endif
   *
   */
  class SdoServiceProviderBase
    : public virtual POA_SDOPackage::SDOService,
      public virtual PortableServer::RefCountServantBase
  {
  public:
    /*!
     * @if jp
     * @brief 仮想デストラクタ
     * @else
     * @brief virtual destructor
     * @endif
     */
    ~SdoServiceProviderBase() override;

    /*!
     * @if jp
     * @brief コンシューマクラスの初期化関数
     *
     * 初期化関数。与えられた RTObject および ServiceProfile から、当該
     * オブジェクトを初期化します。このサービスが
     * ''sdo.service.provider.enabled_services'' で有効化されていれば、
     * この関数は対応するRTCがインスタンス化された直後に呼び出されます。
     *
     * ServiceProfile には以下の情報が入った状態で呼び出されます。
     *
     * - ServiceProfile.id: 当該サービスのIFR型
     * - ServiceProfile.interface_type: 当該サービスのIFR型
     * - ServiceProfile.service: 当該サービスのオブジェクト参照
     * - ServiceProfile.properties: rtc.conf や <component>.conf 等で与
     *                   えられたSDOサービス固有のオプションが渡される。
     *                   confファイル内で
     *                   は、''<pragma>.<module_name>.<interface_name>''
     *                   というプリフィックスをつけたオプションとして与
     *                   えることができ、properties 内には、このプリ
     *                   フィックスを除いたオプションがkey:value形式で
     *                   含まれている。
     *
     * 関数内では、主に properties から設定内容を読み込みサービス固有の
     * 設定等を行います。与えられた ServiceProfileの内容が不正、あるい
     * はその他の理由で当該サービスをインスタンス化しない場合は false
     * を返します。その場合、finalize() が呼び出されその後オブジェクト
     * は削除されます。それ以外の場合は true を返すと、サービスオブジェ
     * クトは RTC 内に保持されます。
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

  // No inline for gcc warning, too big
  SdoServiceProviderBase::~SdoServiceProviderBase() = default;

    /*!
     * @if jp
     * @brief SdoServiceProviderFactory の typedef
     * @else
     * @brief typedef of sdoServiceProviderFactory
     * @endif
     */
  using SdoServiceProviderFactory = ::coil::GlobalFactory< ::RTC::SdoServiceProviderBase>;
} // namespace RTC

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    /*!
     * @if jp
     * @brief クラステンプレートの明示的インスタンス化
     * @else
     * @brief Explicit instantiation of class template
     * @endif
     */
EXTERN template class DLL_PLUGIN coil::GlobalFactory<RTC::SdoServiceProviderBase >;
#elif defined(__GNUC__)
EXTERN template class coil::GlobalFactory<RTC::SdoServiceProviderBase>;
#endif

#endif  // RTC_SDOSERVICEPROVIDERBASE_H
