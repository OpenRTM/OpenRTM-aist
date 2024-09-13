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


#include <coil/Factory.h>
#include "OpenSpliceTransport.h"
#include "OpenSpliceOutPort.h"
#include "OpenSpliceInPort.h"
#include "OpenSpliceMessageInfo.h"
#include "OpenSpliceManagerFunc.h"


static const char* const opensplice_sub_option[] =
{
  "topic.__value__", "chatter",
  "topic.__widget__", "text",
  "topic.__constraint__", "none",
  "reader_qos.id.__value__", "",
  "reader_qos.id.__widget__", "text",
  "reader_qos.id.__constraint__", "none",
  "topic_qos.id.__value__", "",
  "topic_qos.id.__widget__", "text",
  "topic_qos.id.__constraint__", "none",
  "reader_qos.durability.kind.__value__", "TRANSIENT_DURABILITY_QOS",
  "reader_qos.durability.kind.__widget__", "radio",
  "reader_qos.durability.kind.__constraint__", "(VOLATILE_DURABILITY_QOS, TRANSIENT_LOCAL_DURABILITY_QOS, TRANSIENT_DURABILITY_QOS, PERSISTENT_DURABILITY_QOS)",
  "reader_qos.deadline.period.sec.__value__", "2147483647",
  "reader_qos.deadline.period.sec.__widget__", "spin",
  "reader_qos.deadline.period.sec.__constraint__", "0 <= x <= 2147483647",
  "reader_qos.deadline.period.nanosec.__value__", "2147483647",
  "reader_qos.deadline.period.nanosec.__widget__", "text",
  "reader_qos.deadline.period.nanosec.__constraint__", "0 <= x <= 2147483647",
  "reader_qos.latency_budget.duration.sec.__value__", "0",
  "reader_qos.latency_budget.duration.sec.__widget__", "spin",
  "reader_qos.latency_budget.duration.sec.__constraint__", "0 <= x <= 2147483647",
  "reader_qos.latency_budget.duration.nanosec.__value__", "0",
  "reader_qos.latency_budget.duration.nanosec.__widget__", "spin",
  "reader_qos.latency_budget.duration.nanosec.__constraint__", "0 <= x <= 2147483647",
  "reader_qos.liveliness.kind.__value__", "AUTOMATIC_LIVELINESS_QOS",
  "reader_qos.liveliness.kind.__widget__", "radio",
  "reader_qos.liveliness.kind.__constraint__", "(AUTOMATIC_LIVELINESS_QOS, MANUAL_BY_PARTICIPANT_LIVELINESS_QOS, MANUAL_BY_TOPIC_LIVELINESS_QOS)",
  "reader_qos.liveliness.lease_duration.sec.__value__", "2147483647",
  "reader_qos.liveliness.lease_duration.sec.__widget__", "spin",
  "reader_qos.liveliness.lease_duration.sec.__constraint__", "0 <= x <= 2147483647",
  "reader_qos.liveliness.lease_duration.nanosec.__value__", "2147483647",
  "reader_qos.liveliness.lease_duration.nanosec.__widget__", "spin",
  "reader_qos.liveliness.lease_duration.nanosec.__constraint__", "0 <= x <= 2147483647",
  "reader_qos.reliability.kind.__value__", "BEST_EFFORT_RELIABILITY_QOS",
  "reader_qos.reliability.kind.__widget__", "radio",
  "reader_qos.reliability.kind.__constraint__", "(BEST_EFFORT_RELIABILITY_QOS, RELIABLE_RELIABILITY_QOS)",
  "reader_qos.reliability.max_blocking_time.sec.__value__", "2147483647",
  "reader_qos.reliability.max_blocking_time.sec.__widget__", "spin",
  "reader_qos.reliability.max_blocking_time.sec.__constraint__", "0 <= x <= 2147483647",
  "reader_qos.reliability.max_blocking_time.nanosec.__value__", "2147483647",
  "reader_qos.reliability.max_blocking_time.nanosec.__widget__", "spin",
  "reader_qos.reliability.max_blocking_time.nanosec.__constraint__", "0 <= x <= 2147483647",
  "reader_qos.reliability.synchronous.__value__", "NO",
  "reader_qos.reliability.synchronous.__widget__", "radio",
  "reader_qos.reliability.synchronous.__constraint__", "(YES, NO)",
  "reader_qos.destination_order.kind.__value__", "BY_RECEPTION_TIMESTAMP_DESTINATIONORDER_QOS",
  "reader_qos.destination_order.kind.__widget__", "radio",
  "reader_qos.destination_order.kind.__constraint__", "(BY_RECEPTION_TIMESTAMP_DESTINATIONORDER_QOS, BY_SOURCE_TIMESTAMP_DESTINATIONORDER_QOS)",
  "reader_qos.history.kind.__value__", "KEEP_LAST_HISTORY_QOS",
  "reader_qos.history.kind.__widget__", "radio",
  "reader_qos.history.kind.__constraint__", "(KEEP_LAST_HISTORY_QOS, KEEP_ALL_HISTORY_QOS)",
  "reader_qos.history.depth.__value__", "1",
  "reader_qos.history.depth.__widget__", "spin",
  "reader_qos.history.depth.__constraint__", "0 <= x <= 2147483647",
  "reader_qos.resource_limits.max_samples.__value__", "-1",
  "reader_qos.resource_limits.max_samples.__widget__", "spin",
  "reader_qos.resource_limits.max_samples.__constraint__", "-1 <= x <= 2147483647",
  "reader_qos.resource_limits.max_instances.__value__", "-1",
  "reader_qos.resource_limits.max_instances.__widget__", "spin",
  "reader_qos.resource_limits.max_instances.__constraint__", "-1 <= x <= 2147483647",
  "reader_qos.resource_limits.max_samples_per_instance.__value__", "-1",
  "reader_qos.resource_limits.max_samples_per_instance.__widget__", "spin",
  "reader_qos.resource_limits.max_samples_per_instance.__constraint__", "-1 <= x <= 2147483647",
  "reader_qos.ownership.kind.__value__", "SHARED_OWNERSHIP_QOS",
  "reader_qos.ownership.kind.__widget__", "radio",
  "reader_qos.ownership.kind.__constraint__", "(SHARED_OWNERSHIP_QOS, EXCLUSIVE_OWNERSHIP_QOS)",
  "reader_qos.time_based_filter.minimum_separation.sec.__value__", "0",
  "reader_qos.time_based_filter.minimum_separation.sec.__widget__", "spin",
  "reader_qos.time_based_filter.minimum_separation.sec.__constraint__", "0 <= x <= 2147483647",
  "reader_qos.time_based_filter.minimum_separation.nanosec.__value__", "0",
  "reader_qos.time_based_filter.minimum_separation.nanosec.__widget__", "spin",
  "reader_qos.time_based_filter.minimum_separation.nanosec.__constraint__", "0 <= x <= 2147483647",
  "reader_qos.reader_data_lifecycle.autopurge_disposed_samples_delay.sec.__value__", "2147483647",
  "reader_qos.reader_data_lifecycle.autopurge_disposed_samples_delay.sec.__widget__", "spin",
  "reader_qos.reader_data_lifecycle.autopurge_disposed_samples_delay.sec.__constraint__", "0 <= x <= 2147483647",
  "reader_qos.reader_data_lifecycle.autopurge_disposed_samples_delay.nanosec.__value__", "2147483647",
  "reader_qos.reader_data_lifecycle.autopurge_disposed_samples_delay.nanosec.__widget__", "spin",
  "reader_qos.reader_data_lifecycle.autopurge_disposed_samples_delay.nanosec.__constraint__", "0 <= x <= 2147483647",
  "reader_qos.reader_data_lifecycle.autopurge_dispose_all.__value__", "NO",
  "reader_qos.reader_data_lifecycle.autopurge_dispose_all.__widget__", "radio",
  "reader_qos.reader_data_lifecycle.autopurge_dispose_all.__constraint__", "(YES, NO)",
  "reader_qos.reader_data_lifecycle.autopurge_nowriter_samples_delay.sec.__value__", "2147483647",
  "reader_qos.reader_data_lifecycle.autopurge_nowriter_samples_delay.sec.__widget__", "spin",
  "reader_qos.reader_data_lifecycle.autopurge_nowriter_samples_delay.sec.__constraint__", "0 <= x <= 2147483647",
  "reader_qos.reader_data_lifecycle.autopurge_nowriter_samples_delay.nanosec.__value__", "2147483647",
  "reader_qos.reader_data_lifecycle.autopurge_nowriter_samples_delay.nanosec.__widget__", "spin",
  "reader_qos.reader_data_lifecycle.autopurge_nowriter_samples_delay.nanosec.__constraint__", "0 <= x <= 2147483647",
  "reader_qos.reader_data_lifecycle.enable_invalid_samples.__value__", "YES",
  "reader_qos.reader_data_lifecycle.enable_invalid_samples.__widget__", "radio",
  "reader_qos.reader_data_lifecycle.enable_invalid_samples.__constraint__", "(YES, NO)",
  "reader_qos.reader_data_lifecycle.invalid_sample_visibility.kind.__value__", "MINIMUM_INVALID_SAMPLES",
  "reader_qos.reader_data_lifecycle.invalid_sample_visibility.kind.__widget__", "radio",
  "reader_qos.reader_data_lifecycle.invalid_sample_visibility.kind.__constraint__", "(NO_INVALID_SAMPLES, MINIMUM_INVALID_SAMPLES, ALL_INVALID_SAMPLES)",
  "topic_qos.durability.kind.__value__", "TRANSIENT_DURABILITY_QOS",
  "topic_qos.durability.kind.__widget__", "radio",
  "topic_qos.durability.kind.__constraint__", "(VOLATILE_DURABILITY_QOS, TRANSIENT_LOCAL_DURABILITY_QOS, TRANSIENT_DURABILITY_QOS, PERSISTENT_DURABILITY_QOS)",
  "topic_qos.deadline.period.sec.__value__", "2147483647",
  "topic_qos.deadline.period.sec.__widget__", "spin",
  "topic_qos.deadline.period.sec.__constraint__", "0 <= x <= 2147483647",
  "topic_qos.deadline.period.nanosec.__value__", "2147483647",
  "topic_qos.deadline.period.nanosec.__widget__", "spin",
  "topic_qos.deadline.period.nanosec.__constraint__", "0 <= x <= 2147483647",
  "topic_qos.latency_budget.duration.sec.__value__", "0",
  "topic_qos.latency_budget.duration.sec.__widget__", "spin",
  "topic_qos.latency_budget.duration.sec.__constraint__", "0 <= x <= 2147483647",
  "topic_qos.latency_budget.duration.nanosec.__value__", "0",
  "topic_qos.latency_budget.duration.nanosec.__widget__", "spin",
  "topic_qos.latency_budget.duration.nanosec.__constraint__", "0 <= x <= 2147483647",
  "topic_qos.liveliness.kind.__value__", "AUTOMATIC_LIVELINESS_QOS",
  "topic_qos.liveliness.kind.__widget__", "radio",
  "topic_qos.liveliness.kind.__constraint__", "(AUTOMATIC_LIVELINESS_QOS, MANUAL_BY_PARTICIPANT_LIVELINESS_QOS, MANUAL_BY_TOPIC_LIVELINESS_QOS)",
  "topic_qos.liveliness.lease_duration.sec.__value__", "2147483647",
  "topic_qos.liveliness.lease_duration.sec.__widget__", "spin",
  "topic_qos.liveliness.lease_duration.sec.__constraint__", "0 <= x <= 2147483647",
  "topic_qos.liveliness.lease_duration.nanosec.__value__", "2147483647",
  "topic_qos.liveliness.lease_duration.nanosec.__widget__", "spin",
  "topic_qos.liveliness.lease_duration.nanosec.__constraint__", "0 <= x <= 2147483647",
  "topic_qos.reliability.kind.__value__", "RELIABLE_RELIABILITY_QOS",
  "topic_qos.reliability.kind.__widget__", "radio",
  "topic_qos.reliability.kind.__constraint__", "(BEST_EFFORT_RELIABILITY_QOS, RELIABLE_RELIABILITY_QOS)",
  "topic_qos.reliability.max_blocking_time.sec.__value__", "2147483647",
  "topic_qos.reliability.max_blocking_time.sec.__widget__", "spin",
  "topic_qos.reliability.max_blocking_time.sec.__constraint__", "0 <= x <= 2147483647",
  "topic_qos.reliability.max_blocking_time.nanosec.__value__", "2147483647",
  "topic_qos.reliability.max_blocking_time.nanosec.__widget__", "spin",
  "topic_qos.reliability.max_blocking_time.nanosec.__constraint__", "0 <= x <= 2147483647",
  "topic_qos.reliability.synchronous.__value__", "NO",
  "topic_qos.reliability.synchronous.__widget__", "radio",
  "topic_qos.reliability.synchronous.__constraint__", "(YES, NO)",
  "topic_qos.destination_order.kind.__value__", "BY_RECEPTION_TIMESTAMP_DESTINATIONORDER_QOS",
  "topic_qos.destination_order.kind.__widget__", "radio",
  "topic_qos.destination_order.kind.__constraint__", "(BY_RECEPTION_TIMESTAMP_DESTINATIONORDER_QOS, BY_SOURCE_TIMESTAMP_DESTINATIONORDER_QOS)",
  "topic_qos.history.kind.__value__", "KEEP_ALL_HISTORY_QOS",
  "topic_qos.history.kind.__widget__", "radio",
  "topic_qos.history.kind.__constraint__", "(KEEP_LAST_HISTORY_QOS, KEEP_ALL_HISTORY_QOS)",
  "topic_qos.history.depth.__value__", "1",
  "topic_qos.history.depth.__widget__", "spin",
  "topic_qos.history.depth.__constraint__", "0 <= x <= 2147483647",
  "topic_qos.resource_limits.max_samples.__value__", "-1",
  "topic_qos.resource_limits.max_samples.__widget__", "spin",
  "topic_qos.resource_limits.max_samples.__constraint__", "-1 <= x <= 2147483647",
  "topic_qos.resource_limits.max_instances.__value__", "-1",
  "topic_qos.resource_limits.max_instances.__widget__", "spin",
  "topic_qos.resource_limits.max_instances.__constraint__", "-1 <= x <= 2147483647",
  "topic_qos.resource_limits.max_samples_per_instance.__value__", "-1",
  "topic_qos.resource_limits.max_samples_per_instance.__widget__", "spin",
  "topic_qos.resource_limits.max_samples_per_instance.__constraint__", "-1 <= x <= 2147483647",
  "topic_qos.transport_priority.value.__value__", "0",
  "topic_qos.transport_priority.value.__widget__", "spin",
  "topic_qos.transport_priority.value.__constraint__", "0 <= x <= 2147483647",
  "topic_qos.lifespan.duration.sec.__value__", "2147483647",
  "topic_qos.lifespan.duration.sec.__widget__", "spin",
  "topic_qos.lifespan.duration.sec.__constraint__", "0 <= x <= 2147483647",
  "topic_qos.lifespan.duration.nanosec.__value__", "2147483647",
  "topic_qos.lifespan.duration.nanosec.__widget__", "spin",
  "topic_qos.lifespan.duration.nanosec.__constraint__", "0 <= x <= 2147483647",
  "topic_qos.ownership.kind.__value__", "SHARED_OWNERSHIP_QOS",
  "topic_qos.ownership.kind.__widget__", "radio",
  "topic_qos.ownership.kind.__constraint__", "(SHARED_OWNERSHIP_QOS, EXCLUSIVE_OWNERSHIP_QOS)",
  "topic_qos.transport_priority.value.__value__", "0",
  "topic_qos.transport_priority.value.__widget__", "spin",
  "topic_qos.transport_priority.value.__constraint__", "0 <= x <= 2147483647",
  "topic_qos.durability_service.history_depth.__value__", "1",
  "topic_qos.durability_service.history_depth.__widget__", "spin",
  "topic_qos.durability_service.history_depth.__constraint__", "0 <= x <= 2147483647",
  "topic_qos.durability_service.history_kind.__value__", "KEEP_LAST_HISTORY_QOS",
  "topic_qos.durability_service.history_kind.__widget__", "radio",
  "topic_qos.durability_service.history_kind.__constraint__", "(KEEP_LAST_HISTORY_QOS, KEEP_ALL_HISTORY_QOS)",
  "topic_qos.durability_service.max_instances.__value__", "-1",
  "topic_qos.durability_service.max_instances.__widget__", "spin",
  "topic_qos.durability_service.max_instances.__constraint__", "-1 <= x <= 2147483647",
  "topic_qos.durability_service.max_samples.__value__", "-1",
  "topic_qos.durability_service.max_samples.__widget__", "spin",
  "topic_qos.durability_service.max_samples.__constraint__", "-1 <= x <= 2147483647",
  "topic_qos.durability_service.max_samples_per_instance.__value__", "-1",
  "topic_qos.durability_service.max_samples_per_instance.__widget__", "spin",
  "topic_qos.durability_service.max_samples_per_instance.__constraint__", "-1 <= x <= 2147483647",
  "topic_qos.durability_service.service_cleanup_delay.sec.__value__", "0",
  "topic_qos.durability_service.service_cleanup_delay.sec.__widget__", "spin",
  "topic_qos.durability_service.service_cleanup_delay.sec.__constraint__", "0 <= x <= 2147483647",
  "topic_qos.durability_service.service_cleanup_delay.nanosec.__value__", "0",
  "topic_qos.durability_service.service_cleanup_delay.nanosec.__widget__", "spin",
  "topic_qos.durability_service.service_cleanup_delay.nanosec.__constraint__", "0 <= x <= 2147483647",
  ""
};

static const char* const opensplice_pub_option[] =
{
  "topic.__value__", "chatter",
  "topic.__widget__", "text",
  "topic.__constraint__", "none",
  "writer_qos.id.__value__", "",
  "writer_qos.id.__widget__", "text",
  "writer_qos.id.__constraint__", "none",
  "topic_qos.id.__value__", "",
  "topic_qos.id.__widget__", "text",
  "topic_qos.id.__constraint__", "none",
  "writer_qos.durability.kind.__value__", "TRANSIENT_DURABILITY_QOS",
  "writer_qos.durability.kind.__widget__", "radio",
  "writer_qos.durability.kind.__constraint__", "(VOLATILE_DURABILITY_QOS, TRANSIENT_LOCAL_DURABILITY_QOS, TRANSIENT_DURABILITY_QOS, PERSISTENT_DURABILITY_QOS)",
  "writer_qos.deadline.period.sec.__value__", "2147483647",
  "writer_qos.deadline.period.sec.__widget__", "spin",
  "writer_qos.deadline.period.sec.__constraint__", "0 <= x <= 2147483647",
  "writer_qos.deadline.period.nanosec.__value__", "2147483647",
  "writer_qos.deadline.period.nanosec.__widget__", "text",
  "writer_qos.deadline.period.nanosec.__constraint__", "0 <= x <= 2147483647",
  "writer_qos.latency_budget.duration.sec.__value__", "0",
  "writer_qos.latency_budget.duration.sec.__widget__", "spin",
  "writer_qos.latency_budget.duration.sec.__constraint__", "0 <= x <= 2147483647",
  "writer_qos.latency_budget.duration.nanosec.__value__", "0",
  "writer_qos.latency_budget.duration.nanosec.__widget__", "spin",
  "writer_qos.latency_budget.duration.nanosec.__constraint__", "0 <= x <= 2147483647",
  "writer_qos.liveliness.kind.__value__", "AUTOMATIC_LIVELINESS_QOS",
  "writer_qos.liveliness.kind.__widget__", "radio",
  "writer_qos.liveliness.kind.__constraint__", "(AUTOMATIC_LIVELINESS_QOS, MANUAL_BY_PARTICIPANT_LIVELINESS_QOS, MANUAL_BY_TOPIC_LIVELINESS_QOS)",
  "writer_qos.liveliness.lease_duration.sec.__value__", "2147483647",
  "writer_qos.liveliness.lease_duration.sec.__widget__", "spin",
  "writer_qos.liveliness.lease_duration.sec.__constraint__", "0 <= x <= 2147483647",
  "writer_qos.liveliness.lease_duration.nanosec.__value__", "2147483647",
  "writer_qos.liveliness.lease_duration.nanosec.__widget__", "spin",
  "writer_qos.liveliness.lease_duration.nanosec.__constraint__", "0 <= x <= 2147483647",
  "writer_qos.reliability.kind.__value__", "RELIABLE_RELIABILITY_QOS",
  "writer_qos.reliability.kind.__widget__", "radio",
  "writer_qos.reliability.kind.__constraint__", "(BEST_EFFORT_RELIABILITY_QOS, RELIABLE_RELIABILITY_QOS)",
  "writer_qos.reliability.max_blocking_time.sec.__value__", "2147483647",
  "writer_qos.reliability.max_blocking_time.sec.__widget__", "spin",
  "writer_qos.reliability.max_blocking_time.sec.__constraint__", "0 <= x <= 2147483647",
  "writer_qos.reliability.max_blocking_time.nanosec.__value__", "2147483647",
  "writer_qos.reliability.max_blocking_time.nanosec.__widget__", "spin",
  "writer_qos.reliability.max_blocking_time.nanosec.__constraint__", "0 <= x <= 2147483647",
  "writer_qos.reliability.synchronous.__value__", "NO",
  "writer_qos.reliability.synchronous.__widget__", "radio",
  "writer_qos.reliability.synchronous.__constraint__", "(YES, NO)",
  "writer_qos.destination_order.kind.__value__", "BY_RECEPTION_TIMESTAMP_DESTINATIONORDER_QOS",
  "writer_qos.destination_order.kind.__widget__", "radio",
  "writer_qos.destination_order.kind.__constraint__", "(BY_RECEPTION_TIMESTAMP_DESTINATIONORDER_QOS, BY_SOURCE_TIMESTAMP_DESTINATIONORDER_QOS)",
  "writer_qos.history.kind.__value__", "KEEP_LAST_HISTORY_QOS",
  "writer_qos.history.kind.__widget__", "radio",
  "writer_qos.history.kind.__constraint__", "(KEEP_LAST_HISTORY_QOS, KEEP_ALL_HISTORY_QOS)",
  "writer_qos.history.depth.__value__", "1",
  "writer_qos.history.depth.__widget__", "spin",
  "writer_qos.history.depth.__constraint__", "0 <= x <= 2147483647",
  "writer_qos.resource_limits.max_samples.__value__", "-1",
  "writer_qos.resource_limits.max_samples.__widget__", "spin",
  "writer_qos.resource_limits.max_samples.__constraint__", "-1 <= x <= 2147483647",
  "writer_qos.resource_limits.max_instances.__value__", "-1",
  "writer_qos.resource_limits.max_instances.__widget__", "spin",
  "writer_qos.resource_limits.max_instances.__constraint__", "-1 <= x <= 2147483647",
  "writer_qos.resource_limits.max_samples_per_instance.__value__", "-1",
  "writer_qos.resource_limits.max_samples_per_instance.__widget__", "spin",
  "writer_qos.resource_limits.max_samples_per_instance.__constraint__", "-1 <= x <= 2147483647",
  "writer_qos.transport_priority.value.__value__", "0",
  "writer_qos.transport_priority.value.__widget__", "spin",
  "writer_qos.transport_priority.value.__constraint__", "0 <= x <= 2147483647",
  "writer_qos.lifespan.duration.sec.__value__", "2147483647",
  "writer_qos.lifespan.duration.sec.__widget__", "spin",
  "writer_qos.lifespan.duration.sec.__constraint__", "0 <= x <= 2147483647",
  "writer_qos.lifespan.duration.nanosec.__value__", "2147483647",
  "writer_qos.lifespan.duration.nanosec.__widget__", "spin",
  "writer_qos.lifespan.duration.nanosec.__constraint__", "0 <= x <= 2147483647",
  "writer_qos.ownership.kind.__value__", "SHARED_OWNERSHIP_QOS",
  "writer_qos.ownership.kind.__widget__", "radio",
  "writer_qos.ownership.kind.__constraint__", "(SHARED_OWNERSHIP_QOS, EXCLUSIVE_OWNERSHIP_QOS)",
  "writer_qos.ownership_strength.value.__value__", "0",
  "writer_qos.ownership_strength.value.__widget__", "spin",
  "writer_qos.ownership_strength.value.__constraint__", "0 <= x <= 2147483647",
  "writer_qos.writer_data_lifecycle.autodispose_unregistered_instances.__value__", "YES",
  "writer_qos.writer_data_lifecycle.autodispose_unregistered_instances.__widget__", "radio",
  "writer_qos.writer_data_lifecycle.autodispose_unregistered_instances.__constraint__", "(YES, NO)",
  "writer_qos.writer_data_lifecycle.autopurge_suspended_samples_delay.sec.__value__", "2147483647",
  "writer_qos.writer_data_lifecycle.autopurge_suspended_samples_delay.sec.__widget__", "spin",
  "writer_qos.writer_data_lifecycle.autopurge_suspended_samples_delay.sec.__constraint__", "0 <= x <= 2147483647",
  "writer_qos.writer_data_lifecycle.autopurge_suspended_samples_delay.nanosec.__value__", "2147483647",
  "writer_qos.writer_data_lifecycle.autopurge_suspended_samples_delay.nanosec.__widget__", "spin",
  "writer_qos.writer_data_lifecycle.autopurge_suspended_samples_delay.nanosec.__constraint__", "0 <= x <= 2147483647",
  "writer_qos.writer_data_lifecycle.autounregister_instance_dela.sec.__value__", "2147483647",
  "writer_qos.writer_data_lifecycle.autounregister_instance_dela.sec.__widget__", "spin",
  "writer_qos.writer_data_lifecycle.autounregister_instance_dela.sec.__constraint__", "0 <= x <= 2147483647",
  "writer_qos.writer_data_lifecycle.autounregister_instance_dela.nanosec.__value__", "2147483647",
  "writer_qos.writer_data_lifecycle.autounregister_instance_dela.nanosec.__widget__", "spin",
  "writer_qos.writer_data_lifecycle.autounregister_instance_dela.nanosec.__constraint__", "0 <= x <= 2147483647",
  "topic_qos.durability.kind.__value__", "TRANSIENT_DURABILITY_QOS",
  "topic_qos.durability.kind.__widget__", "radio",
  "topic_qos.durability.kind.__constraint__", "(VOLATILE_DURABILITY_QOS, TRANSIENT_LOCAL_DURABILITY_QOS, TRANSIENT_DURABILITY_QOS, PERSISTENT_DURABILITY_QOS)",
  "topic_qos.deadline.period.sec.__value__", "2147483647",
  "topic_qos.deadline.period.sec.__widget__", "spin",
  "topic_qos.deadline.period.sec.__constraint__", "0 <= x <= 2147483647",
  "topic_qos.deadline.period.nanosec.__value__", "2147483647",
  "topic_qos.deadline.period.nanosec.__widget__", "spin",
  "topic_qos.deadline.period.nanosec.__constraint__", "0 <= x <= 2147483647",
  "topic_qos.latency_budget.duration.sec.__value__", "0",
  "topic_qos.latency_budget.duration.sec.__widget__", "spin",
  "topic_qos.latency_budget.duration.sec.__constraint__", "0 <= x <= 2147483647",
  "topic_qos.latency_budget.duration.nanosec.__value__", "0",
  "topic_qos.latency_budget.duration.nanosec.__widget__", "spin",
  "topic_qos.latency_budget.duration.nanosec.__constraint__", "0 <= x <= 2147483647",
  "topic_qos.liveliness.kind.__value__", "AUTOMATIC_LIVELINESS_QOS",
  "topic_qos.liveliness.kind.__widget__", "radio",
  "topic_qos.liveliness.kind.__constraint__", "(AUTOMATIC_LIVELINESS_QOS, MANUAL_BY_PARTICIPANT_LIVELINESS_QOS, MANUAL_BY_TOPIC_LIVELINESS_QOS)",
  "topic_qos.liveliness.lease_duration.sec.__value__", "2147483647",
  "topic_qos.liveliness.lease_duration.sec.__widget__", "spin",
  "topic_qos.liveliness.lease_duration.sec.__constraint__", "0 <= x <= 2147483647",
  "topic_qos.liveliness.lease_duration.nanosec.__value__", "2147483647",
  "topic_qos.liveliness.lease_duration.nanosec.__widget__", "spin",
  "topic_qos.liveliness.lease_duration.nanosec.__constraint__", "0 <= x <= 2147483647",
  "topic_qos.reliability.kind.__value__", "RELIABLE_RELIABILITY_QOS",
  "topic_qos.reliability.kind.__widget__", "radio",
  "topic_qos.reliability.kind.__constraint__", "(BEST_EFFORT_RELIABILITY_QOS, RELIABLE_RELIABILITY_QOS)",
  "topic_qos.reliability.max_blocking_time.sec.__value__", "2147483647",
  "topic_qos.reliability.max_blocking_time.sec.__widget__", "spin",
  "topic_qos.reliability.max_blocking_time.sec.__constraint__", "0 <= x <= 2147483647",
  "topic_qos.reliability.max_blocking_time.nanosec.__value__", "2147483647",
  "topic_qos.reliability.max_blocking_time.nanosec.__widget__", "spin",
  "topic_qos.reliability.max_blocking_time.nanosec.__constraint__", "0 <= x <= 2147483647",
  "topic_qos.reliability.synchronous.__value__", "NO",
  "topic_qos.reliability.synchronous.__widget__", "radio",
  "topic_qos.reliability.synchronous.__constraint__", "(YES, NO)",
  "topic_qos.destination_order.kind.__value__", "BY_RECEPTION_TIMESTAMP_DESTINATIONORDER_QOS",
  "topic_qos.destination_order.kind.__widget__", "radio",
  "topic_qos.destination_order.kind.__constraint__", "(BY_RECEPTION_TIMESTAMP_DESTINATIONORDER_QOS, BY_SOURCE_TIMESTAMP_DESTINATIONORDER_QOS)",
  "topic_qos.history.kind.__value__", "KEEP_ALL_HISTORY_QOS",
  "topic_qos.history.kind.__widget__", "radio",
  "topic_qos.history.kind.__constraint__", "(KEEP_LAST_HISTORY_QOS, KEEP_ALL_HISTORY_QOS)",
  "topic_qos.history.depth.__value__", "1",
  "topic_qos.history.depth.__widget__", "spin",
  "topic_qos.history.depth.__constraint__", "0 <= x <= 2147483647",
  "topic_qos.resource_limits.max_samples.__value__", "-1",
  "topic_qos.resource_limits.max_samples.__widget__", "spin",
  "topic_qos.resource_limits.max_samples.__constraint__", "-1 <= x <= 2147483647",
  "topic_qos.resource_limits.max_instances.__value__", "-1",
  "topic_qos.resource_limits.max_instances.__widget__", "spin",
  "topic_qos.resource_limits.max_instances.__constraint__", "-1 <= x <= 2147483647",
  "topic_qos.resource_limits.max_samples_per_instance.__value__", "-1",
  "topic_qos.resource_limits.max_samples_per_instance.__widget__", "spin",
  "topic_qos.resource_limits.max_samples_per_instance.__constraint__", "-1 <= x <= 2147483647",
  "topic_qos.transport_priority.value.__value__", "0",
  "topic_qos.transport_priority.value.__widget__", "spin",
  "topic_qos.transport_priority.value.__constraint__", "0 <= x <= 2147483647",
  "topic_qos.lifespan.duration.sec.__value__", "2147483647",
  "topic_qos.lifespan.duration.sec.__widget__", "spin",
  "topic_qos.lifespan.duration.sec.__constraint__", "0 <= x <= 2147483647",
  "topic_qos.lifespan.duration.nanosec.__value__", "2147483647",
  "topic_qos.lifespan.duration.nanosec.__widget__", "spin",
  "topic_qos.lifespan.duration.nanosec.__constraint__", "0 <= x <= 2147483647",
  "topic_qos.ownership.kind.__value__", "SHARED_OWNERSHIP_QOS",
  "topic_qos.ownership.kind.__widget__", "radio",
  "topic_qos.ownership.kind.__constraint__", "(SHARED_OWNERSHIP_QOS, EXCLUSIVE_OWNERSHIP_QOS)",
  "topic_qos.transport_priority.value.__value__", "0",
  "topic_qos.transport_priority.value.__widget__", "spin",
  "topic_qos.transport_priority.value.__constraint__", "0 <= x <= 2147483647",
  "topic_qos.durability_service.history_depth.__value__", "1",
  "topic_qos.durability_service.history_depth.__widget__", "spin",
  "topic_qos.durability_service.history_depth.__constraint__", "0 <= x <= 2147483647",
  "topic_qos.durability_service.history_kind.__value__", "KEEP_LAST_HISTORY_QOS",
  "topic_qos.durability_service.history_kind.__widget__", "radio",
  "topic_qos.durability_service.history_kind.__constraint__", "(KEEP_LAST_HISTORY_QOS, KEEP_ALL_HISTORY_QOS)",
  "topic_qos.durability_service.max_instances.__value__", "-1",
  "topic_qos.durability_service.max_instances.__widget__", "spin",
  "topic_qos.durability_service.max_instances.__constraint__", "-1 <= x <= 2147483647",
  "topic_qos.durability_service.max_samples.__value__", "-1",
  "topic_qos.durability_service.max_samples.__widget__", "spin",
  "topic_qos.durability_service.max_samples.__constraint__", "-1 <= x <= 2147483647",
  "topic_qos.durability_service.max_samples_per_instance.__value__", "-1",
  "topic_qos.durability_service.max_samples_per_instance.__widget__", "spin",
  "topic_qos.durability_service.max_samples_per_instance.__constraint__", "-1 <= x <= 2147483647",
  "topic_qos.durability_service.service_cleanup_delay.sec.__value__", "0",
  "topic_qos.durability_service.service_cleanup_delay.sec.__widget__", "spin",
  "topic_qos.durability_service.service_cleanup_delay.sec.__constraint__", "0 <= x <= 2147483647",
  "topic_qos.durability_service.service_cleanup_delay.nanosec.__value__", "0",
  "topic_qos.durability_service.service_cleanup_delay.nanosec.__widget__", "spin",
  "topic_qos.durability_service.service_cleanup_delay.nanosec.__constraint__", "0 <= x <= 2147483647",
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
    //終了処理をすると異常終了するため一旦コメントアウトする
    //RTC_OpenSplice::shutdown();
}
void ManagerActionListener::postReinit()
{

}

void ManagerActionListener::preReinit()
{

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
  void OpenSpliceTransportInit(RTC::Manager* manager)
  {
      {
          coil::Properties prop(opensplice_sub_option);
          RTC::InPortConsumerFactory& factory(RTC::InPortConsumerFactory::instance());
          factory.addFactory("opensplice",
              ::coil::Creator< ::RTC::InPortConsumer,
              ::RTC::OpenSpliceOutPort>,
              ::coil::Destructor< ::RTC::InPortConsumer,
              ::RTC::OpenSpliceOutPort>,
              prop);
      }
      {
          coil::Properties prop(opensplice_pub_option);
          RTC::InPortProviderFactory& factory(RTC::InPortProviderFactory::instance());
          factory.addFactory("opensplice",
              ::coil::Creator< ::RTC::InPortProvider,
              ::RTC::OpenSpliceInPort>,
              ::coil::Destructor< ::RTC::InPortProvider,
              ::RTC::OpenSpliceInPort>,
              prop);
      }
      
      
      RTC_OpenSplice::start(manager->getConfig().getNode("opensplice"));
      OpenSpliceMessageInfoInit(manager->getConfig().getNode("opensplice"));
    
      ManagerActionListener *listener = new ManagerActionListener();
      manager->addManagerActionListener(listener);
      

  }
  
}


