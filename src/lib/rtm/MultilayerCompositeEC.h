// -*- C++ -*-
/*!
 * @file MultilayerCompositeEC.cpp
 * @brief 
 * @date $Date: 2018-10-03 15:44:03 $
 * @author Nobuhiko Miyamoto <n-miyamoto@aist.go.jp>
 *
 * Copyright (C) 2018
 *     Robot Innovation Research Center,
 *     Intelligent Systems Research Institute,
 *     National Institute of
 *         Advanced Industrial Science and Technology (AIST), Japan
 *     All rights reserved.
 *
 *
 */

#ifndef RTC_MULTILAYERCOMPOSITEEC_H
#define RTC_MULTILAYERCOMPOSITEEC_H


#include <rtm/PeriodicExecutionContext.h>
#include <coil/PeriodicTask.h>



namespace RTC_exp
{
  /*!
   * @if jp
   * @class MultilayerCompositeEC
   * @brief MultilayerCompositeEC クラス
   *
   * Periodic Sampled Data Processing(周期実行用)ExecutionContextクラス。
   *
   * @since 0.4.0
   *
   * @else
   * @class MultilayerCompositeEC
   * @brief MultilayerCompositeEC class
   *
   * Periodic Sampled Data Processing (for the execution cycles)
   * ExecutionContext class
   *
   * @since 0.4.0
   *
   * @endif
   */
  class MultilayerCompositeEC
    : public virtual RTC_exp::PeriodicExecutionContext
  {
  public:
    /*!
     * @if jp
     * @brief デフォルトコンストラクタ
     *
     * デフォルトコンストラクタ
     * プロファイルに以下の項目を設定する。
     *  - kind : PERIODIC
     *  - rate : 0.0
     *
     * @else
     * @brief Default Constructor
     *
     * Default Constructor
     * Set the following items to profile.
     *  - kind : PERIODIC
     *  - rate : 0.0
     *
     * @endif
     */
    MultilayerCompositeEC();

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
    ~MultilayerCompositeEC() override;

    /*!
     * @if jp
     * @brief ExecutionContextの初期化を行う
     *
     * ExecutionContextの初期化処理
     *
     * @else
     * @brief Initialize the ExecutionContext
     *
     * This operation initialize the ExecutionContext
     *
     * @endif
     */
     void init(coil::Properties& props) override;


    /*!
     * @if jp
     * @brief ExecutionContext 用のスレッド実行関数
     *
     * ExecutionContext 用のスレッド実行関数。登録されたコンポーネント
     * の処理を呼び出す。
     *
     * @return 実行結果
     *
     * @else
     * @brief Thread execution function for ExecutionContext
     *
     * Thread execution function for ExecutionContext.  Invoke the
     * registered components operation.
     *
     * @return The execution result
     *
     * @endif
     */
    int svc() override;

    /*!
     * @if jp
     * @brief コンポーネントをバインドする。
     *
     * コンポーネントをバインドする。
     *
     * @else
     * @brief Bind the component.
     *
     * Bind the component.
     *
     * @endif
     */
    RTC::ReturnCode_t bindComponent(RTC::RTObject_impl* rtc) override;


    virtual RTC_impl::RTObjectStateMachine* findComponent(RTC::LightweightRTObject_ptr comp);
    virtual void addTask(std::vector<RTC::LightweightRTObject_ptr> rtcs);
    


  protected:
      class ChildTask
      {
      public:
          ChildTask(coil::PeriodicTaskBase* task, MultilayerCompositeEC* ec);
          virtual ~ChildTask();
          void addComponent(RTC::LightweightRTObject_ptr rtc);
          void updateCompList();
          virtual int svc();
          void signal();
          void join();
          coil::TimeMeasure::Statistics getPeriodStat();
          coil::TimeMeasure::Statistics getExecStat();
          void finalize();
      private:
          std::vector<RTC::LightweightRTObject_var> m_rtcs;
          bool m_stopping{false};
          coil::PeriodicTaskBase* m_task;
          MultilayerCompositeEC* m_ec;
          std::vector<RTC_impl::RTObjectStateMachine*> m_comps;
          WorkerThreadCtrl m_worker;
          WorkerThreadCtrl m_signal_worker;

      };

      virtual void addRTCToTask(ChildTask* task, RTC::LightweightRTObject_ptr rtobj);

      std::vector<ChildTask*> m_tasklist;
      RTC_impl::RTObjectStateMachine* m_ownersm{nullptr};


  };  // class MultilayerCompositeEC
} // namespace RTC_exp


extern "C"
{
  /*!
   * @if jp
   * @brief ECFactoryへの登録のための初期化関数
   * @else
   * @brief Initialization function to register to ECFactory
   * @endif
   */
  void MultilayerCompositeECInit(RTC::Manager* manager);
}

#endif  // RTC_MULTILAYERCOMPOSITEEC_H
