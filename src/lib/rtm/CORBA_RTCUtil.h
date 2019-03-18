// -*- C++ -*-
/*!
 * @file CORBA_RTCUtil.h
 * @brief CORBA RTC utility
 * @date $Date$
 * @author Noriaki Ando <n-ando@aist.go.jp>
 *
 * Copyright (C) 2015
 *     Intelligent Systems Research Institute,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 * $Id$
 *
 */

#ifndef CORBA_RTCUTIL_H
#define CORBA_RTCUTIL_H

#include <rtm/RTC.h>
#include <rtm/NVUtil.h>
#include <rtm/CORBA_SeqUtil.h>
#include <rtm/RTObject.h>


namespace CORBA_RTCUtil
{
  /*!
   * @if jp
   * @brief RTC操作ユーティリティー関数群
   *
   * - RTC基本操作系
   * get_profile()
   * is_existing()
   * is_alive_in_default_ec()
   *
   * - ECおよび状態操作系
   * get_actual_ec()
   * get_ec_id()
   * activate()
   * deactivate()
   * reset()
   * get_state()
   * is_in_inactive()
   * is_in_active()
   * is_in_error()
   * get_default_rate()
   * set_default_rate()
   * get_current_rate()
   * set_current_rate()
   * add_rtc_to_default_ec()
   * remove_rtc_to_default_ec()
   * get_participants_rtc()
   *
   * - ポート操作系
   * get_port_names()
   * get_inport_names()
   * get_outport_names()
   * get_svcport_names()
   * get_port_by_name()
   * get_connector_names(RTObject_ptr)
   * get_connector_ids()
   * create_connector()
   * already_connected()
   * connect()
   * connect_by_name()
   * disconnect()
   * disconnect_by_connector_name()
   * disconnect_by_connector_id()
   * disconnect_by_port_name()
   *
   * - コンフィギュレーション系
   * get_configuration()
   * get_parameter_by_key()
   * get_current_configuration_name()
   * get_active_configuration()
   * set_configuration()
   *
   * @else
   * @brief RTC handling utility functions
   *
   * @endif
   */
  /*!
   * @brief Getting component profile
   * This is an example obtained properties from a RTC.
   *
   * - implementation_id: ConsoleOut
   * - type_name: ConsoleOut
   * - description: Console output component
   * - version: 1.0
   * - vendor: Noriaki Ando, AIST
   * - category: example
   * - activity_type: DataFlowComponent
   * - max_instance: 10
   * - language: C++
   * - lang_type: compile
   * - conf:
   * - instance_name: ConsoleOut1
   * - sdo
   *   - service
   *     - provider
   *       - enabled_services:
   *       - available_services:
   *     - consumer
   *       - enabled_services:
   *       - available_services:
   * - config
   *   - version: 1.1.1
   * - openrtm
   *   - name: OpenRTM-aist-1.1.1
   *   - version: 1.1.1
   * - os
   *   - name: Linux
   *   - release: 3.13.0-24-generic
   *   - version: #46-Ubuntu SMP Thu Apr 10 19:11:08 UTC 2014
   *   - arch: x86_64
   *   - hostname: ubuntu1404
   * - corba
   *   - endpoint:
   *   - id: omniORB
   * - exec_cxt
   *   - periodic
   *     - type: PeriodicExecutionContext
   *     - rate: 1000
   *   - sync_transition: YES
   *   - transition_timeout: 0.5
   * - logger
   *   - enable: YES
   *   - log_level: PARANOID
   * - naming
   *   - enable: YES
   *   - type: corba
   *   - formats: %h.host_cxt/%n.rtc
   *   - names: ubuntu1404.host_cxt/ConsoleOut1.rtc
   * - port
   *   - inport
   *     - in
   *       - buffer
   *         - read
   *           - empty_policy: block
   *           - timeout: 2.0
   *         - write
   *           - full_policy: overwrite
   *           - timeout: 2.0
   *       - subscribe_topic: TimedLong
   * - config_file: consout.conf
   */


  /*!
   * @if jp
   * @brief コンポーネントのプロパティ取得
   * @param rtc RTコンポーネント
   * @return コンポーネントのプロパティ 
   * @else
   * @brief
   * @param rtc
   * @return 
   * @endif
   */
  coil::Properties get_component_profile(RTC::RTObject_ptr rtc);
  /*!
   * @if jp
   * @brief コンポーネントのオブジェクトリファレンスが存在しているかを判定
   * @param rtc RTコンポーネント
   * @return True:生存、False:終了済み
   * @else
   * @brief
   * @param rtc
   * @return 
   * @endif
   */
  bool is_existing(RTC::RTObject_ptr rtc);
  /*!
   * @if jp
   * @brief RTCがデフォルトの実行コンテキストでalive状態かを判定する
   * @param rtc RTコンポーネント
   * @return True:alive状態
   * @else
   * @brief
   * @param rtc
   * @return 
   * @endif
   */
  bool is_alive_in_default_ec(RTC::RTObject_ptr rtc);


  /*!
   * @if jp
   * @brief RTコンポーネントに関連付けした実行コンテキストから指定したIDの実行コンテキストを取得
   * @param rtc 対象のRTコンポーネント
   * @param ec_id 実行コンテキストのID
   * @return 実行コンテキストのオブジェクトリファレンス
   * @else
   * @brief
   * @param rtc
   * @param ec_id
   * @return 
   * @endif
   */
  RTC::ExecutionContext_var get_actual_ec(RTC::RTObject_ptr rtc,
                                          RTC::UniqueId ec_id = 0);

  /*!
   * @if jp
   * @brief 対象のRTコンポーネントから指定した実行コンテキストのIDを取得する
   * @param rtc RTコンポーネント
   * @param ec 実行コンテキスト
   * @return 実行コンテキストのID
   * 指定した実行コンテキストがRTコンポーネントに関連付けられていなかっ
   * た場合は-1を返す
   * @else
   * @brief
   * @param rtc
   * @param ec 
   * @return 
   * @endif
   */
  RTC::UniqueId get_ec_id(RTC::RTObject_ptr rtc,
                          RTC::ExecutionContext_ptr ec);

  /*!
   * @if jp
   * @brief RTCを指定した実行コンテキストでアクティベーションする
   * @param rtc 対象のRTコンポーネント
   * @param ec_id 実行コンテキストのID
   * @return RTC、ECのオブジェクトリファレンスがnilの場合は
   * BAD_PARAMETERを返すnilではない場合はactivate_component関数の戻り値
   * を返す。RTC_OKの場合はアクティベーションが成功
   * @else
   * @brief
   * @param rtc
   * @param ec_id
   * @return
   * @endif
   */
  RTC::ReturnCode_t activate(RTC::RTObject_ptr rtc, RTC::UniqueId ec_id = 0);

  /*!
   * @if jp
   * @brief  RTCを指定した実行コンテキストで非アクティベーションする
   * @param rtc 対象のRTコンポーネント
   * @param ec_id 実行コンテキストのID
   * @return RTC、ECのオブジェクトリファレンスがnilの場合は
   * BAD_PARAMETERを返すnilではない場合はdeactivate_component関数の戻り
   * 値を返す。RTC_OKの場合は非アクティベーションが成功
   * @else
   * @brief
   * @param rtc
   * @param ec_id
   * @return
   * @endif
   */
  RTC::ReturnCode_t deactivate(RTC::RTObject_ptr rtc, RTC::UniqueId ec_id = 0);

  /*!
   * @if jp
   * @brief  RTCを指定した実行コンテキストでリセットする
   * @param rtc 対象のRTコンポーネント
   * @param ec_id 実行コンテキストのID
   * @return RTC、ECのオブジェクトリファレンスがnilの場合は
   * BAD_PARAMETERを返すnilではない場合はreset_component関数の戻り値を
   * 返す。RTC_OKの場合はリセットが成功
   * @else
   * @brief
   * @param rtc
   * @param ec_id
   * @return
   * @endif
   */
  RTC::ReturnCode_t reset(RTC::RTObject_ptr rtc, RTC::UniqueId ec_id = 0);



  /*!
   * @if jp
   * @brief 対象のRTコンポーネントの指定した実行コンテキストでの状態を取得
   * @param state RTCの状態
   * @param rtc 対象のRTコンポーネント
   * @param ec_id 実行コンテキストのID
   * @return  rtc、ecがnilの場合はFalseを返す。
   * nilではない場合はstateに状態を代入してTrueを返す。
   * @else
   * @brief
   * @param state
   * @param rtc
   * @param ec_id 
   * @return 
   * @endif
   */
  bool get_state(RTC::LifeCycleState  &state, RTC::RTObject_ptr rtc, RTC::UniqueId ec_id = 0);


  /*!
   * @if jp
   * @brief 対象のRTコンポーネントの指定したECでINACTIVE状態かどうか判定
   * @param rtc 対象のRTコンポーネント
   * @param ec_id  実行コンテキストのID
   * @return  INACTIVE状態の時はTrue、それ以外はFalse
   * rtc、ecがnilの場合もFalseを返す
   * @else
   * @brief
   * @param state
   * @param rtc
   * @param ec_id
   * @return 
   * @endif
   */
  bool is_in_inactive(RTC::RTObject_ptr rtc, RTC::UniqueId ec_id = 0);


  /*!
   * @if jp
   * @brief 対象のRTコンポーネントの指定したECでACTIVE状態かどうか判定
   * @param rtc 対象のRTコンポーネント
   * @param ec_id  実行コンテキストのID
   * @return  ACTIVE状態の時はTrue、それ以外はFalse
   * rtc、ecがnilの場合もFalseを返す
   * @else
   * @brief
   * @param rtc
   * @param ec_id
   * @return 
   * @endif
   */
  bool is_in_active(RTC::RTObject_ptr rtc, RTC::UniqueId ec_id = 0);


  /*!
   * @if jp
   * @brief 対象のRTコンポーネントの指定したECでERROR状態かどうか判定
   * @param rtc 対象のRTコンポーネント
   * @param ec_id  実行コンテキストのID
   * @return  ERROR状態の時はTrue、それ以外はFalse
   * rtc、ecがnilの場合もFalseを返す
   * @else
   * @brief
   * @param rtc
   * @param ec_id
   * @return 
   * @endif
   */
  bool is_in_error(RTC::RTObject_ptr rtc, RTC::UniqueId ec_id = 0);



  /*!
   * @if jp
   * @brief RTCのデフォルトの実行コンテキストの実行周期を取得する
   * @param rtc RTコンポーネント
   * @return  実行周期
   * @else
   * @brief
   * @param rtc
   * @return 
   * @endif
   */
  CORBA::Double get_default_rate(RTC::RTObject_ptr rtc);


  /*!
   * @if jp
   * @brief RTCのデフォルトの実行コンテキストの実行周期を設定する
   * @param rtc RTコンポーネント
   * @param rate 実行周期
   * @return set_rate関数の戻り値を返す。
   * RTC_OKで設定が成功
   * @else
   * @brief
   * @param rtc
   * @param rate 
   * @return 
   * @endif
   */
  RTC::ReturnCode_t set_default_rate(RTC::RTObject_ptr rtc,
                                     CORBA::Double rate);


  /*!
   * @if jp
   * @brief RTCの指定IDの実行コンテキストの周期を取得
   * @param rtc RTコンポーネント
   * @param ec_id 指定の実行コンテキストのID
   * @return 実行周期
   * @else
   * @brief
   * @param rtc
   * @param ec_id 
   * @return 
   * @endif
   */
  CORBA::Double get_current_rate(RTC::RTObject_ptr rtc,
                                 RTC::UniqueId ec_id);

  /*!
   * @if jp
   * @brief RTCの指定IDの実行コンテキストの周期を設定
   * @param rtc RTコンポーネント
   * @param ec_id 指定の実行コンテキストのID
   * @param rate 実行周期
   * @return set_rate関数の戻り値を返す。
   * RTC_OKで設定が成功
   * @else
   * @brief
   * @param rtc
   * @param ec_id 
   * @param rate
   * @return 
   * @endif
   */
  RTC::ReturnCode_t set_current_rate(RTC::RTObject_ptr rtc,
                                     RTC::UniqueId ec_id,
                                     CORBA::Double rate);


  /*!
   * @if jp
   * @brief 対象のRTCのデフォルトの実行コンテキストに指定のRTCを関連付ける
   * @param localcomp 対象のRTコンポーネント
   * @param othercomp 実行コンテキストに関連付けるRTコンポーネント
   * @return ecの取得に失敗した場合はBAD_PARAMETERを返す
   * そうでない場合はaddComponent関数の戻り値を返す。RTC_OKで接続成功。
   * @else
   * @brief
   * @param localcomp
   * @param othercomp 
   * @return 
   * @endif
   */
  RTC::ReturnCode_t add_rtc_to_default_ec(RTC::RTObject_ptr localcomp,
                                          RTC::RTObject_ptr othercomp);


  /*!
   * @if jp
   * @brief 対象のRTCのデフォルトのECの指定のRTCへの関連付けを解除する
   * @param localcomp 対象のRTコンポーネント
   * @param othercomp 実行コンテキストとの関連付けを解除するRTコンポーネント
   * @return ecの取得に失敗した場合はBAD_PARAMETERを返す
   * そうでない場合はremoveComponent関数の戻り値を返す。RTC_OKで解除成功。
   * @else
   * @brief
   * @param localcomp
   * @param othercomp 
   * @return 
   * @endif
   */
  RTC::ReturnCode_t remove_rtc_to_default_ec(RTC::RTObject_ptr localcomp,
                                             RTC::RTObject_ptr othercomp);


  /*!
   * @if jp
   * @brief RTCのデフォルトの実行コンテキストに参加しているRTCのリストを取得する
   * 実行コンテキストがnilの場合は空のリストを返す
   * @param rtc 対象のRTコンポーネント
   * @return RTCのリスト
   * @else
   * @brief
   * @param rtc
   * @return 
   * @endif
   */
  const RTC::RTCList get_participants_rtc(RTC::RTObject_ptr rtc);



  /*!
   * @if jp
   * @brief 指定したRTCの保持するポートの名前を取得
   * @param rtc 対象のRTコンポーネント
   * @return ポート名のリスト
   * @else
   * @brief
   * @param rtc
   * @return 
   * @endif
   */
  coil::vstring get_port_names(RTC::RTObject_ptr rtc);


  /*!
   * @if jp
   * @brief 指定したRTCの保持するインポートの名前を取得
   * @param rtc 対象のRTコンポーネント
   * @return ポート名のリスト
   * @else
   * @brief
   * @param rtc
   * @return 
   * @endif
   */
  coil::vstring get_inport_names(RTC::RTObject_ptr rtc);


  /*!
   * @if jp
   * @brief 指定したRTCの保持するアウトポートの名前を取得
   * @param rtc 対象のRTコンポーネント
   * @return ポート名のリスト
   * @else
   * @brief
   * @param rtc
   * @return 
   * @endif
   */
  coil::vstring get_outport_names(RTC::RTObject_ptr rtc);


  /*!
   * @if jp
   * @brief 指定したRTCの保持するサービスポートの名前を取得
   * @param rtc 対象のRTコンポーネント
   * @return ポート名のリスト
   * @else
   * @brief
   * @param rtc
   * @return 
   * @endif
   */
  coil::vstring get_svcport_names(RTC::RTObject_ptr rtc);


  /*!
   * @if jp
   * @brief 指定したポートの保持しているコネクタの名前のリストを取得
   * @param port 対象のポート
   * @return コネクタ名のリスト
   * @else
   * @brief
   * @param port 
   * @return 
   * @endif
   */
  coil::vstring get_connector_names(RTC::PortService_ptr port);


  /*!
   * @if jp
   * @brief 対象のRTCの指定したポートのコネクタの名前のリストを取得
   * @param rtc 対象のRTコンポーネント
   * @param port_name ポート名
   * @return コネクタ名のリスト
   * @else
   * @brief
   * @param rtc
   * @param port_name 
   * @return 
   * @endif
   */
  coil::vstring get_connector_names(RTC::RTObject_ptr rtc,
                                    std::string port_name);

  /*!
   * @if jp
   * @brief 指定したポートの保持しているコネクタのIDのリストを取得
   * @param port 対象のポート
   * @return コネクタのIDのリスト
   * @else
   * @brief
   * @param port 
   * @return 
   * @endif
   */
  coil::vstring get_connector_ids(RTC::PortService_ptr port);


  /*!
   * @if jp
   * @brief 対象のRTCの指定したポートのコネクタのIDのリストを取得
   * @param port RTコンポーネント
   * @param port_name ポート名
   * @return コネクタのIDのリスト
   * @else
   * @brief
   * @param port
   * @param port_name  
   * @return 
   * @endif
   */
  coil::vstring get_connector_ids(RTC::RTObject_ptr rtc,
                                  std::string port_name);


  /*!
   * @if jp
   * @brief 指定したポートを接続するためのコネクタプロファイルを取得
   * @param name コネクタ名
   * @param prop_arg 設定
   * @param port0 対象のポート1
   * @param port1 対象のポート2
   * @return コネクタプロファイル
   * @else
   * @brief
   * @param name
   * @param prop_arg 
   * @param port0 
   * @param port1 
   * @return 
   * @endif
   */
  RTC::ConnectorProfile_var create_connector(std::string name,
                                             coil::Properties prop_arg,
                                             RTC::PortService_ptr port0,
                                             RTC::PortService_ptr port1);

  /*!
   * @if jp
   * @brief 指定したポート同士が接続されているかを判定
   * @param port0 対象のポート1
   * @param port1 対象のポート2
   * @return True: 接続済み、False: 未接続
   * @else
   * @brief
   * @param port0 
   * @param port1 
   * @return 
   * @endif
   */
  bool already_connected(RTC::PortService_ptr localport,
                         RTC::PortService_ptr otherport);

  /*!
   * @if jp
   * @brief 指定したポートを接続する
   * @param name コネクタ名
   * @param prop 設定
   * @param port0 対象のポート1
   * @param port1 対象のポート2
   * @return ポートのオブジェクトリファレンスがnilの場合はBAD_PARAMETERを返す
   * nilではない場合はport0.connect関数の戻り値を返す。RTC_OKの場合は接続が成功
   * @else
   * @brief
   * @param name 
   * @param prop 
   * @param port0 
   * @param port1 
   * @return 
   * @endif
   */
  RTC::ReturnCode_t connect(std::string name,
                            coil::Properties prop,
                            RTC::PortService_ptr port0,
                            RTC::PortService_ptr port1);


  /*!
   * @if jp
   * @brief 対象のRTCの指定したポートのコネクタのIDのリストを取得
   * @param port RTコンポーネント
   * @param port_name ポート名
   * @return コネクタのIDのリスト
   * @else
   * @brief
   * @param port
   * @param port_name  
   * @return 
   * @endif
   */
  RTC::ReturnCode_t disconnect(RTC::ConnectorProfile connector_prof);


  /*!
   * @if jp
   * @brief 対象のポートで指定した名前のコネクタを切断
   * @param port_ref 対象のポート
   * @param conn_name コネクタ名
   * @return portがnilの場合はBAD_PARAMETERを返す
   * nilではない場合はdisconnect関数の戻り値を返す。RTC_OKの場合は切断が成功
   * @else
   * @brief
   * @param port_ref
   * @param conn_name  
   * @return 
   * @endif
   */
  RTC::ReturnCode_t disconnect_connector_name(RTC::PortService_ptr port_ref, std::string conn_name);



  /*!
   * @if jp
   * @brief 対象の名前のポートで指定した名前のコネクタを切断
   * @param port_name 対象のポート名
   * @param conn_name コネクタ名
   * @return portが存在しない場合はBAD_PARAMETERを返す
   * nilではない場合はdisconnect関数の戻り値を返す。RTC_OKの場合は切断が成功
   * @else
   * @brief
   * @param port_name
   * @param conn_name  
   * @return 
   * @endif
   */
  RTC::ReturnCode_t disconnect_connector_name(std::string port_name,
                                              std::string conn_name);


  /*!
   * @if jp
   * @brief 対象のポートで指定したIDのコネクタを切断
   * @param port 対象のポート
   * @param name コネクタID
   * @return portが存在しない場合はBAD_PARAMETERを返す
   * nilではない場合はdisconnect関数の戻り値を返す。RTC_OKの場合は切断が成功
   * @else
   * @brief
   * @param port
   * @param name  
   * @return 
   * @endif
   */
  RTC::ReturnCode_t
  disconnect_connector_id(RTC::PortService_ptr port_ref,
                          std::string conn_id);


  /*!
   * @if jp
   * @brief 対象の名前のポートで指定したIDのコネクタを切断
   * @param port 対象のポート
   * @param name コネクタID
   * @return portが存在しない場合はBAD_PARAMETERを返す
   * nilではない場合はdisconnect関数の戻り値を返す。RTC_OKの場合は切断が成功
   * @else
   * @brief
   * @param port
   * @param name  
   * @return 
   * @endif
   */
  RTC::ReturnCode_t disconnect_connector_id(std::string port_name,
                                            std::string conn_id);

  /*!
   * @if jp
   * @brief 対象のポートのコネクタを全て切断
   * @param port_ref ポートのオブジェクトリファレンス
   * @return portが存在しない場合はBAD_PARAMETERを返す
   * 切断できた場合はRTC_OKを返す
   * @else
   * @brief
   * @param port_ref  
   * @return 
   * @endif
   */
  RTC::ReturnCode_t disconnect_all(RTC::PortService_ptr port_ref);


  /*!
   * @if jp
   * @brief 指定ポート名のポートのコネクタを全て切断
   * @param port_name ポート名
   * @return portが存在しない場合はBAD_PARAMETERを返す
   * 切断できた場合はRTC_OKを返す
   * @else
   * @brief
   * @param port_ref  
   * @return 
   * @endif
   */
  RTC::ReturnCode_t disconnect_all(std::string port_name);

  /*!
  * @if jp
  * @brief 指定した名前のポートを取得
  * @param port_name ポート名
  * @return ポートのオブジェクトリファレンス
  * portが存在しない場合はnilを返す
  * @else
  * @brief
  * @param port_ref
  * @return
  * @endif
  */
  RTC::PortService_var get_port_by_url(std::string port_name);

  /*!
   * @if jp
   * @brief 対象ポートと接続しているポートで指定したポート名と一致した場合に切断
   * @param localport 対象のポート
   * @param othername 接続しているポート名
   * @return ポートがnilの場合、localportの名前とothernameが一致する場
   *         合、接続しているポートの名前でothernameと一致するものがな
   *         い場合にBAD_PARAMETERを返す上記の条件に当てはまらない場合
   *         はdisconnect関数の戻り値を返す。RTC_OKの場合は切断が成功
   * @else
   * @brief
   * @param localport 
   * @param othername 
   * @return 
   * @endif
   */
  RTC::ReturnCode_t disconnect_name(RTC::PortService_ptr localport,
                                    std::string othername);
  /*!
   * @if jp
   * @brief 指定したポートと指定したリスト内のポート全てと接続する
   * @param name コネクタ名
   * @param prop 設定
   * @param port 対象のポート
   * @param target_ports 対象のポートのリスト
   * @return 全ての接続が成功した場合はRTC_OKを返す。
   * connect関数がRTC_OK以外を返した場合はBAD_PARAMETERを返す。
   * @else
   * @brief
   * @param name 
   * @param prop 
   * @param port0 
   * @param port1 
   * @return 
   * @endif
   */
  RTC::ReturnCode_t connect_multi(std::string name,
    coil::Properties prop,
    RTC::PortService_ptr port,
    RTC::PortServiceList_var& target_ports);
  /*!
   * @if jp
   * @brief ポートを名前から検索
   * @else
   * @brief
   * @endif
   */
  struct find_port
  {
    const std::string m_name;
    /*!
     * @if jp
     * @brief 検索するポート名を指定する
     * @param name ポート名
     * @else
     * @brief
     * @param name
     * @endif
     */
    find_port(const std::string name) : m_name(name) {}
    /*!
     * @if jp
     * @brief 対象のポートの名前と指定したポート名が一致するか判定
     * @param p 対象のポート
     * @return True: 名前が一致、False:　名前が不一致
     * @else
     * @brief
     * @param p 
     * @return 
     * @endif
     */
  bool operator()(RTC::PortService_var p);
  };

  /*!
   * @if jp
   * @brief 対象のRTCから指定した名前のポートを取得
   * @param rtc RTコンポーネント
   * @param port_name ポート名
   * @return ポート
   * @else
   * @brief
   * @param rtc
   * @param port_name
   * @return
   * @endif
   */
  RTC::PortService_var get_port_by_name(RTC::RTObject_ptr rtc,
                                        std::string name);

  /*!
   * @if jp
   * @brief 対象のRTCの指定した名前のポートを接続する
   * @param name コネクタ名
   * @param prop 設定
   * @param rtc0 対象のRTCコンポーネント1
   * @param portName0 対象のポート名1
   * @param rtc1 対象のRTCコンポーネント2
   * @param portName1 対象のRTCコンポーネント2
   * @return RTC、ポートがnilの場合はBAD_PARAMETERを返す。
   * nilではない場合はport0.connect関数の戻り値を返す。RTC_OKの場合は接続が成功
   * @else
   * @brief
   * @param name 
   * @param prop 
   * @param rtc0 
   * @param portName0 
   * @param rtc1 
   * @param portName1 
   * @return
   * @endif
   */
  RTC::ReturnCode_t connect_by_name(std::string name, coil::Properties prop,
                                    RTC::RTObject_ptr rtc0,
                                    std::string portName0,
                                    RTC::RTObject_ptr rtc1,
                                    std::string portName1);

  /*!
   * @if jp
   * @brief 対象RTCの指定した名前のコンフィギュレーションセットをkey-valueで取得
   * @param rtc 対象のRTコンポーネント
   * @param conf_name コンフィギュレーションセット名
   * @return コンフィギュレーションセット
   * @else
   * @brief
   * @param rtc 
   * @param conf_name 
   * @return 
   * @endif
   */
  coil::Properties get_configuration(RTC::RTObject_ptr rtc, std::string conf_name);


  /*!
   * @if jp
   * @brief 指定したコンフィギュレーションセット名、パラメータ名のコン
   *        フィギュレーションパラメータを取得
   * @param rtc RTコンポーネント
   * @param confset_name コンフィギュレーションセット名
   * @param value_name パラメータ名
   * @return パラメータ
   * @else
   * @brief
   * @param rtc 
   * @param conf_name 
   * @param value_name 
   * @return 
   * @endif
   */
  std::string get_parameter_by_key(RTC::RTObject_ptr rtc,
                                   std::string confset_name,
                                   std::string value_name);

  /*!
   * @if jp
   * @brief 対象のRTCのアクティブなコンフィギュレーションセット名を取得する
   * @param rtc RTコンポーネント
   * @return コンフィギュレーションセット名
   * コンフィギュレーションの取得に失敗した場合は空の文字列を返す
   * @else
   * @brief
   * @param rtc 
   * @return 
   * @endif
   */
  std::string get_active_configuration_name(RTC::RTObject_ptr rtc);



  /*!
   * @if jp
   * @brief アクティブなコンフィギュレーションセットをkey-valueで取得する
   * @param rtc 対象のRTコンポーネント
   * @return アクティブなコンフィギュレーションセット
   * @else
   * @brief
   * @param rtc 
   * @return 
   * @endif
   */
  coil::Properties get_active_configuration(RTC::RTObject_ptr rtc);


  /*!
   * @if jp
   * @brief コンフィギュレーションパラメータを設定
   * @param rtc 対象のRTコンポーネント
   * @param confset_name コンフィギュレーションセット名
   * @param value_name パラメータ名 
   * @param value パラメータ 
   * @return True:設定に成功、False:設定に失敗
   * @else
   * @brief
   * @param rtc 
   * @param confset_name 
   * @param value_name 
   * @param value 
   * @return 
   * @endif
   */
  bool set_configuration(RTC::RTObject_ptr rtc, std::string confset_name, std::string value_name, std::string value);



  /*!
   * @if jp
   * @brief アクティブなコンフィギュレーションセットのパラメータを設定
   * @param rtc 対象のRTコンポーネント
   * @param value_name パラメータ名 
   * @param value パラメータ 
   * @return True:設定に成功、False:設定に失敗
   * @else
   * @brief
   * @param rtc 
   * @param value_name 
   * @param value 
   * @return 
   * @endif
   */
  bool set_active_configuration(RTC::RTObject_ptr rtc, std::string value_name, std::string value);


  /*!
   * @if jp
   * @brief コンフィギュレーションパラメータの設定
   * @param conf コンフィギュレーション
   * @param confset コンフィギュレーションセット 
   * @param value_name パラメータ名 
   * @param value パラメータ 
   * @return True:設定に成功、False:設定に失敗
   * @else
   * @brief
   * @param rtc 
   * @param conf 
   * @param confset 
   * @param value_name 
   * @param value 
   * @return 
   * @endif
   */
  bool set_configuration_parameter(SDOPackage::Configuration_ptr conf,
                                   SDOPackage::ConfigurationSet* confset,
                                   std::string value_name,
                                   std::string value);


} // namespace CORBA_RTCUtil
#endif // CORBA_SEQUTIL_H
