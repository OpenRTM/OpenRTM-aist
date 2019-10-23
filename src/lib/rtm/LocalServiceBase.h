// -*- C++ -*-
/*!
 * @file LocalServiceBase.h
 * @brief Local service provider base class and its factory
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

#ifndef RTM_LOCALSERVICEBASE_H
#define RTM_LOCALSERVICEBASE_H

#include <coil/Factory.h>
#include <coil/Timer.h>
#include <coil/Properties.h>

#include <string>
#include <vector>

namespace RTM
{

  // forward decl
  class LocalServiceBase;

  /*!
   * @if jp
   * @brief LocalServiceのプロファイルデータ
   * @else
   * @brief Profile data structure of LocalService
   * @endif
   */
  class LocalServiceProfile
  {
  public:
    /*!
     * @if jp
     * @brief デストラクタ
     * @else
     * @brief destructor
     * @endif
     */
    ~LocalServiceProfile();
    /*!
     * @if jp
     * @brief LocalServiceのサービス名
     * @else
     * @brief The name of LocalService
     * @endif
     */
    std::string name;
    /*!
     * @if jp
     * @brief LocalServiceの固有ID
     * @else
     * @brief The unique ID of LocalService
     * @endif
     */
    std::string uuid;
    /*!
     * @if jp
     * @brief LocalServiceのプロパティ
     * @else
     * @brief Properties of LocalService
     * @endif
     */
    coil::Properties properties;
    /*!
     * @if jp
     * @brief LocalServiceのポインタ
     * @else
     * @brief The pointer to LocalService
     * @endif
     */
    LocalServiceBase* service;
  };

  /*!
   * @if jp
   * @brief LocalServiceのリスト
   * @else
   * @brief List of LocalServiceProfile
   * @endif
   */
  using LocalServiceProfileList = std::vector<LocalServiceProfile>;

  /*!
   * @if jp
   *
   * @brief LocalService　基底クラス
   *
   * Local Service とは RT-Middlewareのマネージャデーモン内で各種サービ
   * スを提供するための仕組みである。Local Serviceにはいくつかのユース
   * ケースが考えられる。
   *
   * 1. RTCの内部ロジックに対してサービスを提供する。コンポーネントの開
   * 発者は、一定の艇順を経て、Local Serviceのインスタンスへの参照を得
   * て、このサービスを利用することができる。
   *
   * 2. マネージャ等に機能を追加するためのサービス。たとえは、RTCの名前
   * を登録するCORBAネームサービスの代わりに、新たなディレクトリサービ
   * スやブロードキャストによる名前通知機能などを新たな機能をマネージャ
   * に追加するために利用することができる。マネージャには、様々なアクショ
   * ンに対するフックが用意されており、これを利用することで、マネージャ
   * の様々なイベントに対してアクションを起こすことができる。
   *
   * 3. マネージャは自身のサービス以外に付加的に外部に対してサービスを
   * 提供する機能を有する。外部からCORBA経由で、ローカルの機能、たとえ
   * ばデバイスやリソースのマネジメント機能に対してアクセスする方法を提
   * 供する。サービスのインスタンス化後に、マネージャに対して、マネージャ
   * サービスとして登録することで、外部からのアクセスを受け付けることが
   * できるようになる。
   *
   * Local Service はモジュールとしてロードされ、通常唯一のインスタンス
   * が生成される。すなわち、LocalService はシングルトンとして実装され
   * る。インスタンス化されたサービスはサービスリストに登録され、RTC等
   * からは名前やUUIDによってインスタンスの参照を入手することができる。
   *
   * このオブジェクトのライフサイクルは以下の通り。
   *
   * -# オブジェクトは通常、共有オブジェクト (so, DLL) としてコンパイル・
   *    リンクされる。
   * -# マネージャに対してロードされるとモジュール初期化関数によりオブ
   *    ジェクトファクトリが、LocalServiceFactory に対して登録される。
   *    登録のキーにはUUIDと名前が利用され、これによりサービスが区別さ
   *    れる。
   * -# rtc.conf等のコンフィギュレーション指定により、有効化することが
   *    指定されているサービスインプロバイダは、RTCの起動と同時にインス
   *    タンス化される。
   * -# インスタンス化後、初期化関数 init() が呼ばれる。引数には当該サー
   *    ビスのためのコンフィギュレーションオプションが coil::Propertyに
   *    より渡される。
   * -# マネージャサービスオペレーション reinit が呼ばれると、すべての
   *     Local Service お reinit が呼び出され、更新されたコンフィギュレー
   *     ション情報が読み込まれる。
   * -# マネージャ解体時には、すべての Local Service の finalizeが呼び
   *       出され、解体されるので、ここでリソースの解放など終了処理を行
   *       う。
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
   * LocalService は通常共有オブジェクトとしてコンパイル・リンク
   * される。共有オブジェクトのエントリポイントは通常コンパイルされたファ
   * イル名の basename + "Init" にしておく。以下に、クラス名、ファイル
   * 名、エントリポイント関数名の推奨例を示す。
   *
   * - 実装クラス名: MyLocalService
   * - ファイル名: MyLocalService.h. MyLocalService.cpp
   * - 共有オブジェクト名: MyLocalService.so (or DLL)
   * - エントリポイント関数名: MyLocalServiceInit()
   *
   * エントリポイント関数は通常以下のように、LocalServiceFactory
   * に当該コンシューマのファクトリ (と解体ファンクタ) を登録する以下の
   * ような関数になる。
   *
   * <pre>
   * extern "C"
   * {
   *   void MyLocalServiceInit()
   *   {
   *     RTC::LocalServiceFactory& factory
   *       = RTC::LocalServiceFactory::instance();
   *     factory.addFactory(::RTC::MyLocalSerivce::name,
   *                        ::coil::Creator< ::RTC::LocalServiceBase,
   *                        ::RTC::MyLocalService>);
   *   }
   * };
   * </pre>
   *
   * @else
   *
   * @endif
   *
   */
  class LocalServiceBase
  {
  public:
    /*!
     * @if jp
     * @brief 仮想デストラクタ
     * @else
     * @brief virtual destructor
     * @endif
     */
    virtual ~LocalServiceBase() = default;

    /*!
     * @if jp
     * @brief LocalServiceクラスの初期化関数
     *
     * このオブジェクトの初期化を行う。LocalService を実装する場合、外
     * 部からの設定情報は、このinit()関数により与えられる。
     *
     * @param profile 外部から与えられた LocalServiceProfile
     * @return 与えられた LocalServiceProfile が不正の場合 false
     *
     * @else
     * @brief Initialization function of the LocalService class
     *
     * @endif
     */
    virtual bool
    init(const coil::Properties& props) = 0;
    /*!
     * @if jp
     * @brief LocalServiceクラスの再初期化関数
     *
     * TODO: ドキュメント作成
     *
     * @param profile 新たに与えられた LocalServiceProfile
     * @return 不正な LocalServiceProfile が与えられた場合は false
     *
     * @else
     * @brief Reinitialization function of the LocalService class
     *
     * TODO: Documentation
     *
     * @endif
     */
    virtual bool
    reinit(const coil::Properties& props) = 0;

    /*!
     * @if jp
     * @brief LocalServiceProfile を返す
     *
     * init()/reinit()で与えられた LocalServiceProfile は通常オブジェク
     * ト内で保持される。この関数では保持されている ServiceProfile を返
     * す。
     *
     * @return このオブジェクトが保持している LocalServiceProfile
     *
     * @else
     * @brief Getting LocalServiceProfile
     *
     * TODO: Documentation
     *
     * @endif
     */
    virtual const LocalServiceProfile&
    getProfile() const = 0;

    /*!
     * @if jp
     * @brief 終了処理
     *
     * LocalService が終了する際に呼び出される終了処理用関数。当該オブ
     * ジェクトが保持するリソースを解放するなどの処理を行う。
     *
     * @else
     * @brief Finalization
     *
     * TODO: Documentation
     *
     * @endif
     */
    virtual void
    finalize() = 0;
  };

  /*!
   * @if jp
   * @brief LocalServiceFactory の typedef
   * @else
   * @brief typedef of sdoServiceProviderFactory
   * @endif
   */
  using LocalServiceFactory = ::coil::GlobalFactory< ::RTM::LocalServiceBase>;
} // namespace RTM


#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
  /*!
   * @if jp
   * @brief クラステンプレートの明示的インスタンス化
   * @else
   * @brief Explicit instantiation of class template
   * @endif
   */
EXTERN template class DLL_PLUGIN coil::GlobalFactory<RTM::LocalServiceBase >;
#elif defined(__GNUC__)
EXTERN template class coil::GlobalFactory<RTM::LocalServiceBase>;
#endif

#endif  // RTM_LOCALSERVICEBASE_H
