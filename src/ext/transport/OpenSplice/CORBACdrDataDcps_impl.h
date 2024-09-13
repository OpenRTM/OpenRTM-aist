#ifndef CORBACDRDATADCPS_IMPL_H_
#define CORBACDRDATADCPS_IMPL_H_

#include "ccpp.h"
#include "ccpp_CORBACdrData.h"
#include "TypeSupportMetaHolder.h"
#include "TypeSupport.h"
#include "FooDataWriter_impl.h"
#include "FooDataReader_impl.h"
#include "FooDataReaderView_impl.h"
#include <string>


namespace OpenRTM_OpenSplice {

    class  CORBACdrDataTypeSupportMetaHolder : public ::DDS::OpenSplice::TypeSupportMetaHolder
    {
    public:
        CORBACdrDataTypeSupportMetaHolder (::DDS::String datatype, ::DDS::String keys, ::DDS::String descriptor);
        ~CORBACdrDataTypeSupportMetaHolder () override;
    
    private:
        ::DDS::OpenSplice::TypeSupportMetaHolder * clone() override;
    
        ::DDS::OpenSplice::DataWriter * create_datawriter () override;
    
        ::DDS::OpenSplice::DataReader * create_datareader () override;
    
        ::DDS::OpenSplice::DataReaderView * create_view () override;

        ::DDS::String_var m_datatype;
        ::DDS::String_var m_descriptor;
        ::DDS::String_var m_keys;
    };
    
    class  CORBACdrDataTypeSupport : public virtual CORBACdrDataTypeSupportInterface,
                                                       public ::DDS::OpenSplice::TypeSupport
    {
    public:
        CORBACdrDataTypeSupport (::DDS::String datatype, ::DDS::String keys, ::DDS::String descriptor);
    
        ~CORBACdrDataTypeSupport () override;
    
    private:
        CORBACdrDataTypeSupport (const CORBACdrDataTypeSupport &);
    
        CORBACdrDataTypeSupport & operator= (const CORBACdrDataTypeSupport &);
    };
    
    using CORBACdrDataTypeSupport_var = CORBACdrDataTypeSupportInterface_var;
    using CORBACdrDataTypeSupport_ptr = CORBACdrDataTypeSupportInterface_ptr;
    
    class  CORBACdrDataDataWriter_impl : public virtual CORBACdrDataDataWriter,
                                                           public ::DDS::OpenSplice::FooDataWriter_impl
    {
        friend class DDS::OpenSplice::Publisher;
        friend class CORBACdrDataTypeSupportMetaHolder;
    
    public:
        ::DDS::InstanceHandle_t register_instance (
            const CORBACdrData & instance_data) THROW_ORB_EXCEPTIONS override;
    
        ::DDS::InstanceHandle_t register_instance_w_timestamp (
            const CORBACdrData & instance_data,
            const ::DDS::Time_t & source_timestamp) THROW_ORB_EXCEPTIONS override;
    
        ::DDS::ReturnCode_t unregister_instance (
            const CORBACdrData & instance_data,
            ::DDS::InstanceHandle_t handle_) THROW_ORB_EXCEPTIONS override;
    
        ::DDS::ReturnCode_t unregister_instance_w_timestamp (
            const CORBACdrData & instance_data,
            ::DDS::InstanceHandle_t handle_,
            const ::DDS::Time_t & source_timestamp) THROW_ORB_EXCEPTIONS override;
    
        ::DDS::ReturnCode_t write (
            const CORBACdrData & instance_data,
            ::DDS::InstanceHandle_t handle_) THROW_ORB_EXCEPTIONS override;

        ::DDS::ReturnCode_t write_cdr(
            const ::DDS::CDRSample & data,
            ::DDS::InstanceHandle_t handle_) THROW_ORB_EXCEPTIONS override;
    
        ::DDS::ReturnCode_t write_w_timestamp (
            const CORBACdrData & instance_data,
            ::DDS::InstanceHandle_t handle_,
            const ::DDS::Time_t & source_timestamp) THROW_ORB_EXCEPTIONS override;
    
        ::DDS::ReturnCode_t dispose (
            const CORBACdrData & instance_data,
            ::DDS::InstanceHandle_t handle_) THROW_ORB_EXCEPTIONS override;
    
        ::DDS::ReturnCode_t dispose_w_timestamp (
            const CORBACdrData & instance_data,
            ::DDS::InstanceHandle_t handle_,
            const ::DDS::Time_t & source_timestamp) THROW_ORB_EXCEPTIONS override;
    
        ::DDS::ReturnCode_t writedispose (
            const CORBACdrData & instance_data,
            ::DDS::InstanceHandle_t handle_) THROW_ORB_EXCEPTIONS override;
    
        ::DDS::ReturnCode_t writedispose_w_timestamp (
            const CORBACdrData & instance_data,
            ::DDS::InstanceHandle_t handle_,
            const ::DDS::Time_t & source_timestamp) THROW_ORB_EXCEPTIONS override;
    
        ::DDS::ReturnCode_t get_key_value (
            CORBACdrData & key_holder,
            ::DDS::InstanceHandle_t handle_) THROW_ORB_EXCEPTIONS override;
    
        ::DDS::InstanceHandle_t lookup_instance (
            const CORBACdrData & instance_data) THROW_ORB_EXCEPTIONS override;
    
    protected:
        CORBACdrDataDataWriter_impl ();
    
        ~CORBACdrDataDataWriter_impl () override;
    
        DDS::ReturnCode_t init (
                        DDS::OpenSplice::Publisher *publisher_,
                        DDS::OpenSplice::DomainParticipant *participant_,
                        const DDS::DataWriterQos &qos,
                        DDS::OpenSplice::Topic *a_topic,
                        const char *name,
                        DDS::OpenSplice::cxxCopyIn copyIn_,
                        DDS::OpenSplice::cxxCopyOut copyOut_,
                        u_writerCopy writerCopy_,
                        void *cdrMarshaler_) override;
    
    private:
        CORBACdrDataDataWriter_impl (const CORBACdrDataDataWriter_impl &);
    
        CORBACdrDataDataWriter_impl & operator= (const CORBACdrDataDataWriter_impl &);
    };
    
    class  CORBACdrDataDataReader_impl : public virtual CORBACdrDataDataReader,
                                                           public ::DDS::OpenSplice::FooDataReader_impl
    {
        friend class DDS::OpenSplice::Subscriber;
        friend class CORBACdrDataTypeSupportMetaHolder;
        friend class CORBACdrDataDataReaderView_impl;
    
    public:
        ::DDS::ReturnCode_t read (
            CORBACdrDataSeq & received_data,
            ::DDS::SampleInfoSeq & info_seq,
            ::DDS::Long max_samples,
            ::DDS::SampleStateMask sample_states,
            ::DDS::ViewStateMask view_states,
            ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS override;

        ::DDS::Long read_cdr(
            ::DDS::CDRSample& received_data, 
            ::DDS::SampleInfo& info,
            ::DDS::ULong sample_states,
            ::DDS::ULong view_states,
            ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS override;

    
        ::DDS::ReturnCode_t take (
            CORBACdrDataSeq & received_data,
            ::DDS::SampleInfoSeq & info_seq,
            ::DDS::Long max_samples,
            ::DDS::SampleStateMask sample_states,
            ::DDS::ViewStateMask view_states,
            ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS override;
    
        ::DDS::ReturnCode_t read_w_condition (
            CORBACdrDataSeq & received_data,
            ::DDS::SampleInfoSeq & info_seq,
            ::DDS::Long max_samples,
            ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS override;
    
        ::DDS::ReturnCode_t take_w_condition (
            CORBACdrDataSeq & received_data,
            ::DDS::SampleInfoSeq & info_seq,
            ::DDS::Long max_samples,
            ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS override;
    
        ::DDS::ReturnCode_t read_next_sample (
            CORBACdrData & received_data,
            ::DDS::SampleInfo & sample_info) THROW_ORB_EXCEPTIONS override;
    
        ::DDS::ReturnCode_t take_next_sample (
            CORBACdrData & received_data,
            ::DDS::SampleInfo & sample_info) THROW_ORB_EXCEPTIONS override;
    
        ::DDS::ReturnCode_t read_instance (
            CORBACdrDataSeq & received_data,
            ::DDS::SampleInfoSeq & info_seq,
            ::DDS::Long max_samples,
            ::DDS::InstanceHandle_t a_handle,
            ::DDS::SampleStateMask sample_states,
            ::DDS::ViewStateMask view_states,
            ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS override;
    
        ::DDS::ReturnCode_t take_instance (
            CORBACdrDataSeq & received_data,
            ::DDS::SampleInfoSeq & info_seq,
            ::DDS::Long max_samples,
            ::DDS::InstanceHandle_t a_handle,
            ::DDS::SampleStateMask sample_states,
            ::DDS::ViewStateMask view_states,
            ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS override;
    
        ::DDS::ReturnCode_t read_next_instance (
            CORBACdrDataSeq & received_data,
            ::DDS::SampleInfoSeq & info_seq,
            ::DDS::Long max_samples,
            ::DDS::InstanceHandle_t a_handle,
            ::DDS::SampleStateMask sample_states,
            ::DDS::ViewStateMask view_states,
            ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS override;
    
        ::DDS::ReturnCode_t take_next_instance (
            CORBACdrDataSeq & received_data,
            ::DDS::SampleInfoSeq & info_seq,
            ::DDS::Long max_samples,
            ::DDS::InstanceHandle_t a_handle,
            ::DDS::SampleStateMask sample_states,
            ::DDS::ViewStateMask view_states,
            ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS override;
    
        ::DDS::ReturnCode_t read_next_instance_w_condition (
            CORBACdrDataSeq & received_data,
            ::DDS::SampleInfoSeq & info_seq,
            ::DDS::Long max_samples,
            ::DDS::InstanceHandle_t a_handle,
            ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS override;
    
        ::DDS::ReturnCode_t take_next_instance_w_condition (
            CORBACdrDataSeq & received_data,
            ::DDS::SampleInfoSeq & info_seq,
            ::DDS::Long max_samples,
            ::DDS::InstanceHandle_t a_handle,
            ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS override;
    
        ::DDS::ReturnCode_t return_loan (
            CORBACdrDataSeq & received_data,
            ::DDS::SampleInfoSeq & info_seq) THROW_ORB_EXCEPTIONS override;
    
        ::DDS::ReturnCode_t get_key_value (
            CORBACdrData & key_holder,
            ::DDS::InstanceHandle_t handle_) THROW_ORB_EXCEPTIONS override;
    
        ::DDS::InstanceHandle_t lookup_instance (
            const CORBACdrData & instance) THROW_ORB_EXCEPTIONS override;
    
    protected:
        CORBACdrDataDataReader_impl ();
    
        ~CORBACdrDataDataReader_impl () override;
    
        DDS::ReturnCode_t init (
                DDS::OpenSplice::Subscriber *subscriber,
                const DDS::DataReaderQos &qos,
                DDS::OpenSplice::TopicDescription *a_topic,
                const char *name,
                DDS::OpenSplice::cxxCopyIn copyIn,
                DDS::OpenSplice::cxxCopyOut copyOut,
                DDS::OpenSplice::cxxReaderCopy readerCopy,
                void *cdrMarshaler) override;
    
        static void* dataSeqAlloc (
            void * data_values,
            DDS::ULong len);
    
        static void dataSeqLength (
            void * data_values,
            DDS::ULong len);
    
        static void * dataSeqGetBuffer (
            void * data_values,
            DDS::ULong index);
    
        static void dataSeqCopyOut (
            const void * from,
            void * received_data);
        static void copyDataOut(const void *from, void *to);
    
    private:
        CORBACdrDataDataReader_impl (const CORBACdrDataDataReader_impl &);
        CORBACdrDataDataReader_impl & operator= (const CORBACdrDataDataReader_impl &);
    
        static ::DDS::ReturnCode_t check_preconditions (
            CORBACdrDataSeq & received_data,
            ::DDS::SampleInfoSeq & info_seq,
            ::DDS::Long max_samples);
    };
    
    class  CORBACdrDataDataReaderView_impl : public virtual CORBACdrDataDataReaderView,
                                                               public ::DDS::OpenSplice::FooDataReaderView_impl
    {
        friend class DDS::OpenSplice::DataReader;
        friend class CORBACdrDataTypeSupportMetaHolder;
    
    public:
        ::DDS::ReturnCode_t read (
            CORBACdrDataSeq & received_data,
            ::DDS::SampleInfoSeq & info_seq,
            ::DDS::Long max_samples,
            ::DDS::SampleStateMask sample_states,
            ::DDS::ViewStateMask view_states,
            ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS override;
    
        ::DDS::ReturnCode_t take (
            CORBACdrDataSeq & received_data,
            ::DDS::SampleInfoSeq & info_seq,
            ::DDS::Long max_samples,
            ::DDS::SampleStateMask sample_states,
            ::DDS::ViewStateMask view_states,
            ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS override;
    
        ::DDS::ReturnCode_t read_w_condition (
            CORBACdrDataSeq & received_data,
            ::DDS::SampleInfoSeq & info_seq,
            ::DDS::Long max_samples,
            ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS override;
    
        ::DDS::ReturnCode_t take_w_condition (
            CORBACdrDataSeq & received_data,
            ::DDS::SampleInfoSeq & info_seq,
            ::DDS::Long max_samples,
            ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS override;
    
        ::DDS::ReturnCode_t read_next_sample (
            CORBACdrData & received_data,
            ::DDS::SampleInfo & sample_info) THROW_ORB_EXCEPTIONS override;
    
        ::DDS::ReturnCode_t take_next_sample (
            CORBACdrData & received_data,
            ::DDS::SampleInfo & sample_info) THROW_ORB_EXCEPTIONS override;
    
        ::DDS::ReturnCode_t read_instance (
            CORBACdrDataSeq & received_data,
            ::DDS::SampleInfoSeq & info_seq,
            ::DDS::Long max_samples,
            ::DDS::InstanceHandle_t a_handle,
            ::DDS::SampleStateMask sample_states,
            ::DDS::ViewStateMask view_states,
            ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS override;
    
        ::DDS::ReturnCode_t take_instance (
            CORBACdrDataSeq & received_data,
            ::DDS::SampleInfoSeq & info_seq,
            ::DDS::Long max_samples,
            ::DDS::InstanceHandle_t a_handle,
            ::DDS::SampleStateMask sample_states,
            ::DDS::ViewStateMask view_states,
            ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS override;
    
        ::DDS::ReturnCode_t read_next_instance (
            CORBACdrDataSeq & received_data,
            ::DDS::SampleInfoSeq & info_seq,
            ::DDS::Long max_samples,
            ::DDS::InstanceHandle_t a_handle,
            ::DDS::SampleStateMask sample_states,
            ::DDS::ViewStateMask view_states,
            ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS override;
    
        ::DDS::ReturnCode_t take_next_instance (
            CORBACdrDataSeq & received_data,
            ::DDS::SampleInfoSeq & info_seq,
            ::DDS::Long max_samples,
            ::DDS::InstanceHandle_t a_handle,
            ::DDS::SampleStateMask sample_states,
            ::DDS::ViewStateMask view_states,
            ::DDS::InstanceStateMask instance_states) THROW_ORB_EXCEPTIONS override;
    
        ::DDS::ReturnCode_t read_next_instance_w_condition (
            CORBACdrDataSeq & received_data,
            ::DDS::SampleInfoSeq & info_seq,
            ::DDS::Long max_samples,
            ::DDS::InstanceHandle_t a_handle,
            ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS override;
    
        ::DDS::ReturnCode_t take_next_instance_w_condition (
            CORBACdrDataSeq & received_data,
            ::DDS::SampleInfoSeq & info_seq,
            ::DDS::Long max_samples,
            ::DDS::InstanceHandle_t a_handle,
            ::DDS::ReadCondition_ptr a_condition) THROW_ORB_EXCEPTIONS override;
    
        ::DDS::ReturnCode_t return_loan (
            CORBACdrDataSeq & received_data,
            ::DDS::SampleInfoSeq & info_seq) THROW_ORB_EXCEPTIONS override;
    
        ::DDS::ReturnCode_t get_key_value (
            CORBACdrData & key_holder,
            ::DDS::InstanceHandle_t handle_) THROW_ORB_EXCEPTIONS override;
    
        ::DDS::InstanceHandle_t lookup_instance (
            const CORBACdrData & instance) THROW_ORB_EXCEPTIONS override;
    
    protected:
        CORBACdrDataDataReaderView_impl ();
    
        ~CORBACdrDataDataReaderView_impl () override;
    
        DDS::ReturnCode_t init (
            DDS::OpenSplice::DataReader *reader,
            const char *name,
            const DDS::DataReaderViewQos &qos,
            DDS::OpenSplice::cxxCopyIn copyIn,
            DDS::OpenSplice::cxxCopyOut copyOut) override;
    
    private:
        CORBACdrDataDataReaderView_impl (const CORBACdrDataDataReaderView_impl &);
    
        CORBACdrDataDataReaderView_impl & operator= (const CORBACdrDataDataReaderView_impl &);
    };
    
}

#undef OS_API
#endif
