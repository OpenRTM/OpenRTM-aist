// -*- C++ -*-
/*!
 * @file  FastRTPSOutPort.cpp
 * @brief FastRTPSOutPort class
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


#include "FastRTPSTransport.h"
#include "FastRTPSOutPort.h"
#include "FastRTPSInPort.h"
#include "FastRTPSManager.h"


namespace FastRTPSRTC
{
  static const char* const fastrtps_sub_option[] =
  {
    "subscriber.qos.deadline.period.seconds.__value__", "2147483647",
    "subscriber.qos.deadline.period.seconds.__widget__", "spin",
    "subscriber.qos.deadline.period.seconds.__constraint__", "0 <= x <= 2147483647",
    "subscriber.qos.deadline.period.nanosec.__value__", "4294967295",
    "subscriber.qos.deadline.period.nanosec.__widget__", "spin",
    "subscriber.qos.deadline.period.nanosec.__constraint__", "0 <= x <= 4294967295",
    "subscriber.qos.destinationOrder.kind.__value__", "BY_RECEPTION_TIMESTAMP_DESTINATIONORDER_QOS",
    "subscriber.qos.destinationOrder.kind.__widget__", "radio",
    "subscriber.qos.destinationOrder.kind.__constraint__", "(BY_RECEPTION_TIMESTAMP_DESTINATIONORDER_QOS, BY_SOURCE_TIMESTAMP_DESTINATIONORDER_QOS)",
    "subscriber.qos.disablePositiveACKs.enabled.__value__", "NO",
    "subscriber.qos.disablePositiveACKs.enabled.__widget__", "radio",
    "subscriber.qos.disablePositiveACKs.enabled.__constraint__", "(YES, NO)",
    "subscriber.qos.disablePositiveACKs.duration.seconds.__value__", "2147483647",
    "subscriber.qos.disablePositiveACKs.duration.seconds.__widget__", "spin",
    "subscriber.qos.disablePositiveACKs.duration.seconds.__constraint__", "0 <= x <= 2147483647",
    "subscriber.qos.disablePositiveACKs.duration.nanosec.__value__", "4294967295",
    "subscriber.qos.disablePositiveACKs.duration.nanosec.__widget__", "spin",
    "subscriber.qos.disablePositiveACKs.duration.nanosec.__constraint__", "0 <= x <= 4294967295",
    "subscriber.qos.durability.kind.__value__", "VOLATILE_DURABILITY_QOS",
    "subscriber.qos.durability.kind.__widget__", "radio",
    "subscriber.qos.durability.kind.__constraint__", "(VOLATILE_DURABILITY_QOS, TRANSIENT_LOCAL_DURABILITY_QOS, TRANSIENT_DURABILITY_QOS, PERSISTENT_DURABILITY_QOS)",
    "subscriber.qos.durabilityService.history_depth.__value__", "1",
    "subscriber.qos.durabilityService.history_depth.__widget__", "spin",
    "subscriber.qos.durabilityService.history_depth.__constraint__", "0 <= x <= 2147483647",
    "subscriber.qos.durabilityService.history_kind.__value__", "KEEP_LAST_HISTORY_QOS",
    "subscriber.qos.durabilityService.history_kind.__widget__", "radio",
    "subscriber.qos.durabilityService.history_kind.__constraint__", "(KEEP_LAST_HISTORY_QOS, KEEP_ALL_HISTORY_QOS)",
    "subscriber.qos.durabilityService.max_instances.__value__", "-1",
    "subscriber.qos.durabilityService.max_instances.__widget__", "spin",
    "subscriber.qos.durabilityService.max_instances.__constraint__", "-1 <= x <= 2147483647",
    "subscriber.qos.durabilityService.max_samples.__value__", "-1",
    "subscriber.qos.durabilityService.max_samples.__widget__", "spin",
    "subscriber.qos.durabilityService.max_samples.__constraint__", "-1 <= x <= 2147483647",
    "subscriber.qos.durabilityService.max_samples_per_instance.__value__", "-1",
    "subscriber.qos.durabilityService.max_samples_per_instance.__widget__", "spin",
    "subscriber.qos.durabilityService.max_samples_per_instance.__constraint__", "-1 <= x <= 2147483647",
    "subscriber.qos.durabilityService.service_cleanup_delay.seconds.__value__", "0",
    "subscriber.qos.durabilityService.service_cleanup_delay.seconds.__widget__", "spin",
    "subscriber.qos.durabilityService.service_cleanup_delay.seconds.__constraint__", "0 <= x <= 2147483647",
    "subscriber.qos.durabilityService.service_cleanup_delay.nanosec.__value__", "0",
    "subscriber.qos.durabilityService.service_cleanup_delay.nanosec.__widget__", "spin",
    "subscriber.qos.durabilityService.service_cleanup_delay.nanosec.__constraint__", "0 <= x <= 4294967295",
    "subscriber.qos.latencyBudget.duration.seconds.__value__", "0",
    "subscriber.qos.latencyBudget.duration.seconds.__widget__", "spin",
    "subscriber.qos.latencyBudget.duration.seconds.__constraint__", "0 <= x <= 2147483647",
    "subscriber.qos.latencyBudget.duration.nanosec.__value__", "0",
    "subscriber.qos.latencyBudget.duration.nanosec.__widget__", "spin",
    "subscriber.qos.latencyBudget.duration.nanosec.__constraint__", "0 <= x <= 4294967295",
    "subscriber.qos.lifespan.duration.seconds.__value__", "2147483647",
    "subscriber.qos.lifespan.duration.seconds.__widget__", "spin",
    "subscriber.qos.lifespan.duration.seconds.__constraint__", "0 <= x <= 2147483647",
    "subscriber.qos.lifespan.duration.nanosec.__value__", "4294967295",
    "subscriber.qos.lifespan.duration.nanosec.__widget__", "spin",
    "subscriber.qos.lifespan.duration.nanosec.__constraint__", "0 <= x <= 4294967295",
    "subscriber.qos.liveliness.announcement_period.seconds.__value__", "2147483647",
    "subscriber.qos.liveliness.announcement_period.seconds.__widget__", "spin",
    "subscriber.qos.liveliness.announcement_period.seconds.__constraint__", "0 <= x <= 2147483647",
    "subscriber.qos.liveliness.announcement_period.nanosec.__value__", "4294967295",
    "subscriber.qos.liveliness.announcement_period.nanosec.__widget__", "spin",
    "subscriber.qos.liveliness.announcement_period.nanosec.__constraint__", "0 <= x <= 4294967295",
    "subscriber.qos.liveliness.__value__", "AUTOMATIC_LIVELINESS_QOS",
    "subscriber.qos.liveliness.__widget__", "radio",
    "subscriber.qos.liveliness.__constraint__", "(AUTOMATIC_LIVELINESS_QOS, MANUAL_BY_PARTICIPANT_LIVELINESS_QOS, MANUAL_BY_TOPIC_LIVELINESS_QOS)",
    "subscriber.qos.liveliness.lease_duration.seconds.__value__", "2147483647",
    "subscriber.qos.liveliness.lease_duration.seconds.__widget__", "spin",
    "subscriber.qos.liveliness.lease_duration.seconds.__constraint__", "0 <= x <= 2147483647",
    "subscriber.qos.liveliness.lease_duration.nanosec.__value__", "4294967295",
    "subscriber.qos.liveliness.lease_duration.nanosec.__widget__", "spin",
    "subscriber.qos.liveliness.lease_duration.nanosec.__constraint__", "0 <= x <= 4294967295",
    "subscriber.qos.ownership.kind.__value__", "SHARED_OWNERSHIP_QOS",
    "subscriber.qos.ownership.kind.__widget__", "radio",
    "subscriber.qos.ownership.kind.__constraint__", "(SHARED_OWNERSHIP_QOS, EXCLUSIVE_OWNERSHIP_QOS)",
    "subscriber.qos.presentation.access_scope.__value__", "INSTANCE_PRESENTATION_QOS",
    "subscriber.qos.presentation.access_scope.__widget__", "radio",
    "subscriber.qos.presentation.access_scope.__constraint__", "(INSTANCE_PRESENTATION_QOS, TOPIC_PRESENTATION_QOS, GROUP_PRESENTATION_QOS)",
    "subscriber.qos.presentation.coherent_access.__value__", "NO",
    "subscriber.qos.presentation.coherent_access.__widget__", "radio",
    "subscriber.qos.presentation.coherent_access.__constraint__", "(YES, NO)",
    "subscriber.qos.presentation.ordered_access.__value__", "NO",
    "subscriber.qos.presentation.ordered_access.__widget__", "radio",
    "subscriber.qos.presentation.ordered_access.__constraint__", "(YES, NO)",
    "subscriber.qos.reliability.kind.__value__", "BEST_EFFORT_RELIABILITY_QOS",
    "subscriber.qos.reliability.kind.__widget__", "radio",
    "subscriber.qos.reliability.kind.__constraint__", "(BEST_EFFORT_RELIABILITY_QOS, RELIABLE_RELIABILITY_QOS)",
    "subscriber.qos.reliability.max_blocking_time.seconds.__value__", "0",
    "subscriber.qos.reliability.max_blocking_time.seconds.__widget__", "spin",
    "subscriber.qos.reliability.max_blocking_time.seconds.__constraint__", "0 <= x <= 2147483647",
    "subscriber.qos.reliability.max_blocking_time.nanosec.__value__", "100000000",
    "subscriber.qos.reliability.max_blocking_time.nanosec.__widget__", "spin",
    "subscriber.qos.reliability.max_blocking_time.nanosec.__constraint__", "0 <= x <= 4294967295",
    "subscriber.qos.timeBasedFilter.minimum_separation.seconds.__value__", "0",
    "subscriber.qos.timeBasedFilter.minimum_separation.seconds.__widget__", "spin",
    "subscriber.qos.timeBasedFilter.minimum_separation.seconds.__constraint__", "0 <= x <= 2147483647",
    "subscriber.qos.timeBasedFilter.minimum_separation.nanosec.__value__", "0",
    "subscriber.qos.timeBasedFilter.minimum_separation.nanosec.__widget__", "spin",
    "subscriber.qos.timeBasedFilter.minimum_separation.nanosec.__constraint__", "0 <= x <= 4294967295",
    "subscriber.qos.type_consistency.force_type_validation.__value__", "NO",
    "subscriber.qos.type_consistency.force_type_validation.__widget__", "radio",
    "subscriber.qos.type_consistency.force_type_validation.__constraint__", "(YES, NO)",
    "subscriber.qos.type_consistency.ignore_member_names.__value__", "NO",
    "subscriber.qos.type_consistency.ignore_member_names.__widget__", "radio",
    "subscriber.qos.type_consistency.ignore_member_names.__constraint__", "(YES, NO)",
    "subscriber.qos.type_consistency.ignore_sequence_bounds.__value__", "YES",
    "subscriber.qos.type_consistency.ignore_sequence_bounds.__widget__", "radio",
    "subscriber.qos.type_consistency.ignore_sequence_bounds.__constraint__", "(YES, NO)",
    "subscriber.qos.type_consistency.ignore_string_bounds.__value__", "YES",
    "subscriber.qos.type_consistency.ignore_string_bounds.__widget__", "radio",
    "subscriber.qos.type_consistency.ignore_string_bounds.__constraint__", "(YES, NO)",
    "subscriber.qos.type_consistency.kind.__value__", "ALLOW_TYPE_COERCION",
    "subscriber.qos.type_consistency.kind.__widget__", "radio",
    "subscriber.qos.type_consistency.kind.__constraint__", "(DISALLOW_TYPE_COERCION, ALLOW_TYPE_COERCION)",
    "subscriber.qos.type_consistency.prevent_type_widening.__value__", "NO",
    "subscriber.qos.type_consistency.prevent_type_widening.__widget__", "radio",
    "subscriber.qos.type_consistency.prevent_type_widening.__constraint__", "(YES, NO)",
    "subscriber.history_memory_policy.__value__", "PREALLOCATED_WITH_REALLOC_MEMORY_MODE",
    "subscriber.history_memory_policy.__widget__", "radio",
    "subscriber.history_memory_policy.__constraint__", "(PREALLOCATED_MEMORY_MODE, PREALLOCATED_WITH_REALLOC_MEMORY_MODE, DYNAMIC_RESERVE_MEMORY_MODE, DYNAMIC_REUSABLE_MEMORY_MODE)",
    "subscriber.topic.historyQos.depth.__value__", "1",
    "subscriber.topic.historyQos.depth.__widget__", "spin",
    "subscriber.topic.historyQos.depth.__constraint__", "0 <= x <= 4294967295",
    "subscriber.topic.historyQos.kind.__value__", "KEEP_LAST_HISTORY_QOS",
    "subscriber.topic.historyQos.kind.__widget__", "radio",
    "subscriber.topic.historyQos.kind.__constraint__", "(KEEP_LAST_HISTORY_QOS, KEEP_ALL_HISTORY_QOS)",
    ""
  };

  static const char* const fastrtps_pub_option[] =
  {
    "publisher.qos.deadline.period.seconds.__value__", "2147483647",
    "publisher.qos.deadline.period.seconds.__widget__", "spin",
    "publisher.qos.deadline.period.seconds.__constraint__", "0 <= x <= 2147483647",
    "publisher.qos.deadline.period.nanosec.__value__", "4294967295",
    "publisher.qos.deadline.period.nanosec.__widget__", "spin",
    "publisher.qos.deadline.period.nanosec.__constraint__", "0 <= x <= 4294967295",
    "publisher.qos.destinationOrder.kind.__value__", "BY_RECEPTION_TIMESTAMP_DESTINATIONORDER_QOS",
    "publisher.qos.destinationOrder.kind.__widget__", "radio",
    "publisher.qos.destinationOrder.kind.__constraint__", "(BY_RECEPTION_TIMESTAMP_DESTINATIONORDER_QOS, BY_SOURCE_TIMESTAMP_DESTINATIONORDER_QOS)",
    "publisher.qos.disablePositiveACKs.enabled.__value__", "NO",
    "publisher.qos.disablePositiveACKs.enabled.__widget__", "radio",
    "publisher.qos.disablePositiveACKs.enabled.__constraint__", "(YES, NO)",
    "publisher.qos.disablePositiveACKs.duration.seconds.__value__", "2147483647",
    "publisher.qos.disablePositiveACKs.duration.seconds.__widget__", "spin",
    "publisher.qos.disablePositiveACKs.duration.seconds.__constraint__", "0 <= x <= 2147483647",
    "publisher.qos.disablePositiveACKs.duration.nanosec.__value__", "4294967295",
    "publisher.qos.disablePositiveACKs.duration.nanosec.__widget__", "spin",
    "publisher.qos.disablePositiveACKs.duration.nanosec.__constraint__", "0 <= x <= 4294967295",
    "publisher.qos.durability.kind.__value__", "VOLATILE_DURABILITY_QOS",
    "publisher.qos.durability.kind.__widget__", "radio",
    "publisher.qos.durability.kind.__constraint__", "(VOLATILE_DURABILITY_QOS, TRANSIENT_LOCAL_DURABILITY_QOS, TRANSIENT_DURABILITY_QOS, PERSISTENT_DURABILITY_QOS)",
    "publisher.qos.durabilityService.history_depth.__value__", "1",
    "publisher.qos.durabilityService.history_depth.__widget__", "spin",
    "publisher.qos.durabilityService.history_depth.__constraint__", "0 <= x <= 2147483647",
    "publisher.qos.durabilityService.history_kind.__value__", "KEEP_LAST_HISTORY_QOS",
    "publisher.qos.durabilityService.history_kind.__widget__", "radio",
    "publisher.qos.durabilityService.history_kind.__constraint__", "(KEEP_LAST_HISTORY_QOS, KEEP_ALL_HISTORY_QOS)",
    "publisher.qos.durabilityService.max_instances.__value__", "-1",
    "publisher.qos.durabilityService.max_instances.__widget__", "spin",
    "publisher.qos.durabilityService.max_instances.__constraint__", "-1 <= x <= 2147483647",
    "publisher.qos.durabilityService.max_samples.__value__", "-1",
    "publisher.qos.durabilityService.max_samples.__widget__", "spin",
    "publisher.qos.durabilityService.max_samples.__constraint__", "-1 <= x <= 2147483647",
    "publisher.qos.durabilityService.max_samples_per_instance.__value__", "-1",
    "publisher.qos.durabilityService.max_samples_per_instance.__widget__", "spin",
    "publisher.qos.durabilityService.max_samples_per_instance.__constraint__", "-1 <= x <= 2147483647",
    "publisher.qos.durabilityService.service_cleanup_delay.seconds.__value__", "0",
    "publisher.qos.durabilityService.service_cleanup_delay.seconds.__widget__", "spin",
    "publisher.qos.durabilityService.service_cleanup_delay.seconds.__constraint__", "0 <= x <= 2147483647",
    "publisher.qos.durabilityService.service_cleanup_delay.nanosec.__value__", "0",
    "publisher.qos.durabilityService.service_cleanup_delay.nanosec.__widget__", "spin",
    "publisher.qos.durabilityService.service_cleanup_delay.nanosec.__constraint__", "0 <= x <= 4294967295",
    "publisher.qos.latencyBudget.duration.seconds.__value__", "0",
    "publisher.qos.latencyBudget.duration.seconds.__widget__", "spin",
    "publisher.qos.latencyBudget.duration.seconds.__constraint__", "0 <= x <= 2147483647",
    "publisher.qos.latencyBudget.duration.nanosec.__value__", "0",
    "publisher.qos.latencyBudget.duration.nanosec.__widget__", "spin",
    "publisher.qos.latencyBudget.duration.nanosec.__constraint__", "0 <= x <= 4294967295",
    "publisher.qos.lifespan.duration.seconds.__value__", "2147483647",
    "publisher.qos.lifespan.duration.seconds.__widget__", "spin",
    "publisher.qos.lifespan.duration.seconds.__constraint__", "0 <= x <= 2147483647",
    "publisher.qos.lifespan.duration.nanosec.__value__", "4294967295",
    "publisher.qos.lifespan.duration.nanosec.__widget__", "spin",
    "publisher.qos.lifespan.duration.nanosec.__constraint__", "0 <= x <= 4294967295",
    "publisher.qos.liveliness.announcement_period.seconds.__value__", "2147483647",
    "publisher.qos.liveliness.announcement_period.seconds.__widget__", "spin",
    "publisher.qos.liveliness.announcement_period.seconds.__constraint__", "0 <= x <= 2147483647",
    "publisher.qos.liveliness.announcement_period.nanosec.__value__", "4294967295",
    "publisher.qos.liveliness.announcement_period.nanosec.__widget__", "spin",
    "publisher.qos.liveliness.announcement_period.nanosec.__constraint__", "0 <= x <= 4294967295",
    "publisher.qos.liveliness.__value__", "AUTOMATIC_LIVELINESS_QOS",
    "publisher.qos.liveliness.__widget__", "radio",
    "publisher.qos.liveliness.__constraint__", "(AUTOMATIC_LIVELINESS_QOS, MANUAL_BY_PARTICIPANT_LIVELINESS_QOS, MANUAL_BY_TOPIC_LIVELINESS_QOS)",
    "publisher.qos.liveliness.lease_duration.seconds.__value__", "2147483647",
    "publisher.qos.liveliness.lease_duration.seconds.__widget__", "spin",
    "publisher.qos.liveliness.lease_duration.seconds.__constraint__", "0 <= x <= 2147483647",
    "publisher.qos.liveliness.lease_duration.nanosec.__value__", "4294967295",
    "publisher.qos.liveliness.lease_duration.nanosec.__widget__", "spin",
    "publisher.qos.liveliness.lease_duration.nanosec.__constraint__", "0 <= x <= 4294967295",
    "publisher.qos.ownership.kind.__value__", "SHARED_OWNERSHIP_QOS",
    "publisher.qos.ownership.kind.__widget__", "radio",
    "publisher.qos.ownership.kind.__constraint__", "(SHARED_OWNERSHIP_QOS, EXCLUSIVE_OWNERSHIP_QOS)",
    "publisher.qos.presentation.access_scope.__value__", "INSTANCE_PRESENTATION_QOS",
    "publisher.qos.presentation.access_scope.__widget__", "radio",
    "publisher.qos.presentation.access_scope.__constraint__", "(INSTANCE_PRESENTATION_QOS, TOPIC_PRESENTATION_QOS, GROUP_PRESENTATION_QOS)",
    "publisher.qos.presentation.coherent_access.__value__", "NO",
    "publisher.qos.presentation.coherent_access.__widget__", "radio",
    "publisher.qos.presentation.coherent_access.__constraint__", "(YES, NO)",
    "publisher.qos.presentation.ordered_access.__value__", "NO",
    "publisher.qos.presentation.ordered_access.__widget__", "radio",
    "publisher.qos.presentation.ordered_access.__constraint__", "(YES, NO)",
    "publisher.qos.publishMode.kind.__value__", "SYNCHRONOUS_PUBLISH_MODE",
    "publisher.qos.publishMode.kind.__widget__", "radio",
    "publisher.qos.publishMode.kind.__constraint__", "(SYNCHRONOUS_PUBLISH_MODE, ASYNCHRONOUS_PUBLISH_MODE)",
    "publisher.qos.reliability.kind.__value__", "RELIABLE_RELIABILITY_QOS",
    "publisher.qos.reliability.kind.__widget__", "radio",
    "publisher.qos.reliability.kind.__constraint__", "(BEST_EFFORT_RELIABILITY_QOS, RELIABLE_RELIABILITY_QOS)",
    "publisher.qos.reliability.max_blocking_time.seconds.__value__", "0",
    "publisher.qos.reliability.max_blocking_time.seconds.__widget__", "spin",
    "publisher.qos.reliability.max_blocking_time.seconds.__constraint__", "0 <= x <= 2147483647",
    "publisher.qos.reliability.max_blocking_time.nanosec.__value__", "100000000",
    "publisher.qos.reliability.max_blocking_time.nanosec.__widget__", "spin",
    "publisher.qos.reliability.max_blocking_time.nanosec.__constraint__", "0 <= x <= 4294967295",
    "publisher.qos.timeBasedFilter.minimum_separation.seconds.__value__", "0",
    "publisher.qos.timeBasedFilter.minimum_separation.seconds.__widget__", "spin",
    "publisher.qos.timeBasedFilter.minimum_separation.seconds.__constraint__", "0 <= x <= 2147483647",
    "publisher.qos.timeBasedFilter.minimum_separation.nanosec.__value__", "0",
    "publisher.qos.timeBasedFilter.minimum_separation.nanosec.__widget__", "spin",
    "publisher.qos.timeBasedFilter.minimum_separation.nanosec.__constraint__", "0 <= x <= 4294967295",
    "publisher.history_memory_policy.__value__", "PREALLOCATED_WITH_REALLOC_MEMORY_MODE",
    "publisher.history_memory_policy.__widget__", "radio",
    "publisher.history_memory_policy.__constraint__", "(PREALLOCATED_MEMORY_MODE, PREALLOCATED_WITH_REALLOC_MEMORY_MODE, DYNAMIC_RESERVE_MEMORY_MODE, DYNAMIC_REUSABLE_MEMORY_MODE)",
    "publisher.topic.historyQos.depth.__value__", "1",
    "publisher.topic.historyQos.depth.__widget__", "spin",
    "publisher.topic.historyQos.depth.__constraint__", "0 <= x <= 4294967295",
    "publisher.topic.historyQos.kind.__value__", "KEEP_LAST_HISTORY_QOS",
    "publisher.topic.historyQos.kind.__widget__", "radio",
    "publisher.topic.historyQos.kind.__constraint__", "(KEEP_LAST_HISTORY_QOS, KEEP_ALL_HISTORY_QOS)",
    ""
  };

  ManagerActionListener::ManagerActionListener()
  {

  }
  ManagerActionListener::~ManagerActionListener()
  {

  }
  void ManagerActionListener::preShutdown()
  {

  }
  void ManagerActionListener::postShutdown()
  {
      RTC::FastRTPSManager::shutdown_global();
  }
  void ManagerActionListener::postReinit()
  {

  }

  void ManagerActionListener::preReinit()
  {

  }
}

extern "C"
{
  /*!
   * @if jp
   * @brief モジュール初期化関数
   * @else
   * @brief Module initialization
   * @endif
   */
  void FastRTPSTransportInit(RTC::Manager* manager)
  {

    {
      coil::Properties prop(FastRTPSRTC::fastrtps_sub_option);

        
      RTC::InPortProviderFactory& factory(RTC::InPortProviderFactory::instance());
      factory.addFactory("fast-rtps",
                        ::coil::Creator< ::RTC::InPortProvider,
                                          ::RTC::FastRTPSInPort>,
                        ::coil::Destructor< ::RTC::InPortProvider,
                                            ::RTC::FastRTPSInPort>,
                        prop);
    }

    {
      coil::Properties prop(FastRTPSRTC::fastrtps_pub_option);

      RTC::InPortConsumerFactory& factory(RTC::InPortConsumerFactory::instance());
      factory.addFactory("fast-rtps",
                        ::coil::Creator< ::RTC::InPortConsumer,
                                          ::RTC::FastRTPSOutPort>,
                        ::coil::Destructor< ::RTC::InPortConsumer,
                                            ::RTC::FastRTPSOutPort>,
                        prop);
    }

    RTC::FastRTPSManager::init(manager->getConfig().getNode("fast-rtps"));

    FastRTPSRTC::ManagerActionListener *listener = new FastRTPSRTC::ManagerActionListener();
    manager->addManagerActionListener(listener);
    
  }
  
}


