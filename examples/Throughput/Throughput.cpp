// -*- C++ -*-
/*!
 * @file  Throughput.cpp
 * @brief Clock Throughput Component
 * @date $Date$
 *
 * @author Noriaki Ando n-ando@aist.go.jp
 *
 * $Id$
 */

#include "Throughput.h"
#include <cmath>
#include <coil/Async.h>

#define DEBUG 1

// Module specification
// <rtc-template block="module_spec">
static const char* analyzer_spec[] =
  {
    "implementation_id", "Throughput",
    "type_name",         "Throughput",
    "description",       "Dataport throughput measurement component",
    "version",           "1.0.0",
    "vendor",            "AIST",
    "category",          "example",
    "activity_type",     "PERIODIC",
    "kind",              "DataFlowComponent",
    "max_instance",      "1",
    "language",          "C++",
    "lang_type",         "compile",
    // Configuration variables
    "conf.default.datatype", "double",
    "conf.default.outputfile", "test.dat",
    "conf.default.increment", "100",
    "conf.default.sleep_time", "0.1",
    "conf.default.mode", "logincr",
    "conf.default.maxsize", "1000000",
    "conf.default.maxsend", "1000",
    "conf.default.maxsample", "100",

    // Widget
    "conf.__widget__.datatype", "radio",
    "conf.__widget__.outputfile", "text",
    "conf.__widget__.increment", "text",
    "conf.__widget__.sleep_time", "text",
    "conf.__widget__.mode", "radio",
    "conf.__widget__.maxsize", "text",
    "conf.__widget__.maxsend", "text",
    "conf.__widget__.maxsample", "text",
    // Constraints
    "conf.__constraints__.datatype", "(octet,short,long,float,double)",
    "conf.__constraints__.mode", "(logincr,incr,const)",

    "conf.__type__.datatype", "string",
    "conf.__type__.outputfile", "string",
    "conf.__type__.increment", "long",
    "conf.__type__.sleep_time", "double",
    "conf.__type__.mode", "string",
    "conf.__type__.maxsize", "long",
    "conf.__type__.maxsend", "long",
    "conf.__type__.maxsample", "long",

    ""
  };
// </rtc-template>



/*!
 * @brief constructor
 * @param manager Maneger Object
 */
Throughput::Throughput(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    // Configuration parameters
    m_datatype("double"), m_outputfile("throughput.dat"),
    m_increment(100), m_mode("logincr"), m_maxsize(100000000),
    m_maxsend(1000), m_maxsample(100),

    // InPort, OutPort
    m_inOctetIn("in", m_inOctet),   m_outOctetOut("out", m_outOctet),
    m_inShortIn("in", m_inShort),   m_outShortOut("out", m_outShort),
    m_inLongIn("in", m_inLong),     m_outLongOut ("out", m_outLong),
    m_inFloatIn("in", m_inFloat),   m_outFloatOut("out", m_outFloat),
    m_inDoubleIn("in", m_inDouble), m_outDoubleOut("out", m_outDouble),

    
    m_datasize(1),
    m_record(m_maxsample),
    m_sendcount(0),
    m_logmulcnt(0)
    // </rtc-template>
{
}

/*!
 * @brief destructor
 */
Throughput::~Throughput()
{
}



RTC::ReturnCode_t Throughput::onInitialize()
{
  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  m_datatype = getProperties()["conf.default.datatype"];
  m_filesuffix = getProperties()["conf.default.filesuffix"];
  std::string datatype = coil::normalize(m_datatype);
  if(datatype == "octet")
    {
      addInPort("in", m_inOctetIn);
      m_inOctetIn.
        addConnectorDataListener(ON_BUFFER_WRITE,
                                 new DataListener<RTC::TimedOctetSeq>(this));
      m_inOctetIn.addConnectorListener(ON_CONNECT,
                                       new ConnListener(this));
      addOutPort("out", m_outOctetOut);
      m_varsize = sizeof(CORBA::Octet);
    }
  else if (datatype == "short")
    {
      addInPort("in", m_inShortIn);
      m_inShortIn.
        addConnectorDataListener(ON_BUFFER_WRITE,
                                 new DataListener<RTC::TimedShortSeq>(this));
      m_inShortIn.addConnectorListener(ON_CONNECT,
                                       new ConnListener(this));
      addOutPort("out", m_outShortOut);
      m_varsize = sizeof(CORBA::Short);
    }
  else if (datatype == "long")
    {
      addInPort("in", m_inLongIn);
      m_inLongIn.
        addConnectorDataListener(ON_BUFFER_WRITE,
                                 new DataListener<RTC::TimedLongSeq>(this));
      m_inLongIn.addConnectorListener(ON_CONNECT,
                                      new ConnListener(this));
      addOutPort("out", m_outLongOut);
      m_varsize = sizeof(CORBA::Long);
    }
  else if (datatype == "float")
    {
      addInPort("in", m_inFloatIn);
      m_inFloatIn.
        addConnectorDataListener(ON_BUFFER_WRITE,
                                 new DataListener<RTC::TimedFloatSeq>(this));
      m_inFloatIn.addConnectorListener(ON_CONNECT,
                                       new ConnListener(this));
      addOutPort("out", m_outFloatOut);
      m_varsize = sizeof(CORBA::Float);
    }
  else if (datatype == "double")
    {
      addInPort("inDouble", m_inDoubleIn);
      m_inDoubleIn.
        addConnectorDataListener(ON_BUFFER_WRITE,
                                 new DataListener<RTC::TimedDoubleSeq>(this));
      m_inDoubleIn.addConnectorListener(ON_CONNECT,
                                        new ConnListener(this));
      addOutPort("out", m_outDoubleOut);
      m_varsize = sizeof(CORBA::Double);
    }
  else
    {
      return RTC::RTC_ERROR;
    }
  std::cout << "varsize: " << m_varsize << std::endl;
  // <rtc-template block="bind_config">
  // Bind variables and configuration variable
  bindParameter("datatype", m_datatype, "double");
  bindParameter("outputfile", m_outputfile, "test.dat");
  bindParameter("increment", m_increment, "100");
  bindParameter("sleep_time", m_sleep_time, "0.1");
  bindParameter("mode", m_mode, "logincr");
  bindParameter("maxsize", m_maxsize, "100000");
  bindParameter("maxsend", m_maxsend, "1000");
  bindParameter("maxsample", m_maxsample, "100");
  // </rtc-template>

  m_record.reserve(m_maxsample);
//  for (size_t i(0); i < m_maxsample; ++i)
//    {
//      m_record[i] = coil::TimeValue(0, 0);
//    }
  return RTC::RTC_OK;
}

RTC::ReturnCode_t Throughput::onActivated(RTC::UniqueId ec_id)
{
  m_datasize = 1;
  setDataSize(m_datasize);

  m_sendcount = 0;
  m_logmulcnt = 0;

  return RTC::RTC_OK;
}

RTC::ReturnCode_t Throughput::onDeactivated(RTC::UniqueId ec_id)
{
  if (m_fs.is_open()) { m_fs.close(); }

  m_datasize = 1;
  setDataSize(m_datasize);

  m_sendcount = 0;
  m_logmulcnt = 0;


  

  if (getInPortConnectorSize() == 0)
  {
	  coil::Async* async(coil::AsyncInvoker(this, std::mem_fun(&Throughput::exit)));
	  async->invoke();
  }
  return RTC::RTC_OK;
}

RTC::ReturnCode_t Throughput::onExecute(RTC::UniqueId ec_id)
{
  static double logmul[] = {2.0, 2.5, 2.0};
  
  // Setting data length
  if (getDataSize() != m_datasize) { setDataSize(m_datasize); }
  writeData();
  m_sendcount++;

  if (m_sendcount % m_maxsample != 0) { return RTC::RTC_OK; }

  // increment data size
  if (m_mode == "logincr")
    {
#ifdef DEBUG
      std::cout << "sendcount: " << m_sendcount;
      std::cout << "\tmaxsample: " << m_maxsample;
      std::cout << "\tlogmulcnt%3: " << m_logmulcnt % 3;
      std::cout << "\tlogmul[]: " << logmul[m_logmulcnt % 3] << std::endl;
#endif // DEBUG
      m_datasize *= logmul[m_logmulcnt % 3];
      m_logmulcnt++;
    }
  else if (m_mode == "incr")
    {
      m_datasize += m_increment;
    }
  else
    {
      if((long)m_sendcount > m_maxsend)
        {
          exit();
          return RTC::RTC_OK;
        }
    }
#ifdef DEBUG
  std::cout << "######### data length changed #########" << std::endl;
  std::cout << "length(): " << getDataSize();
  std::cout << "\tm_datasize: " << m_datasize;
  std::cout << "\tm_maxsize: " << m_maxsize;
  std::cout << "\tsendcount: " << m_sendcount << std::endl;
#endif // DEBUG

  coil::sleep(m_sleep_time); // sleep for calculating measurement statistics

  // calculation is triggered data size change
  // to finish the last calculation, size 0 array is sent
  if(m_datasize > m_maxsize)
    {
      std::cout << "Exiting" << std::endl;
      setDataSize(1); // to finalize measurement
      writeData();
      deactivate(ec_id);
    }
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t Throughput::onAborting(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Throughput::onError(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Throughput::onReset(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Throughput::onStateUpdate(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t Throughput::onRateChanged(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

void Throughput::writeData()
{
  std::string datatype = coil::normalize(m_datatype);
  if(datatype == "octet")
    {
      setTimestamp(m_outOctet);
      m_outOctetOut.write();
    }
  else if (datatype == "short")
    {
      setTimestamp(m_outShort);
      m_outShortOut.write();
    }
  else if (datatype == "long")
    {
      setTimestamp(m_outLong);
      m_outLongOut.write();
    }
  else if (datatype == "float")
    {
      setTimestamp(m_outFloat);
      m_outFloatOut.write();
    }
  else if (datatype == "double")
    {
      setTimestamp(m_outDouble);
      m_outDoubleOut.write();
    }
}

void Throughput::setDataSize(CORBA::ULong size)
{
  std::string datatype = coil::normalize(m_datatype);
  if(datatype == "octet")
    {
      m_outOctet.data.length(size);
    }
  else if (datatype == "short")
    {
      m_outShort.data.length(size);
    }
  else if (datatype == "long")
    {
      m_outLong.data.length(size);
    }
  else if (datatype == "float")
    {
      m_outFloat.data.length(size);
    }
  else if (datatype == "double")
    {
      m_outDouble.data.length(size);
    }
}
CORBA::ULong Throughput::getDataSize()
{
  std::string datatype = coil::normalize(m_datatype);
  if(datatype == "octet")
    {
      return m_outOctet.data.length();
    }
  else if (datatype == "short")
    {
      return m_outShort.data.length();
    }
  else if (datatype == "long")
    {
      return m_outLong.data.length();
    }
  else if (datatype == "float")
    {
      return m_outFloat.data.length();
    }
  else if (datatype == "double")
    {
      return m_outDouble.data.length();
    }
  return 0;
}

void Throughput::receiveData(const RTC::Time &tm, const CORBA::ULong seq_length)
{
  static size_t size(0);
  static size_t record_num(0);
  static size_t record_ptr(0);

  // data arrived -> getting time
  coil::TimeValue received_time(coil::gettimeofday());
  if (size == 0) { size = seq_length; }

  // calculate latency statistics
#ifdef DEBUG
  std::cout << "Time: " << tm.sec << "[s]\t" << tm.nsec << "[ns]" << std::endl;
  std::cout << "length(): " << seq_length;
  std::cout << "\tsize: " << size;
  std::cout << "\trecord_num: " << record_num;
  std::cout << "\trecord_ptr: " << record_ptr << std::endl;
#endif // DEBUG

  if (seq_length != size && record_num != 0)
    {
      double max_latency(0.0), min_latency(10000.0), mean_latency(0.0);
      double variance(0.0), stddev(0.0), throughput(0.0);
      double sum(0.0), sq_sum(0.0);
      size_t record_len = record_num > record_ptr ? m_maxsample : record_ptr;

#ifdef DEBUG
      std::cout << "%%%%% record_num: " << record_num;
      std::cout << " record_ptr: " << record_ptr;
      std::cout << " record_len: " << record_len;
      std::cout << " maxsample: " << m_maxsample << std::endl;
#endif // DEBUG

      for (size_t i(0); i < record_len; ++i)
        {
          double tmp(m_record[i]);
          sum += tmp;
          sq_sum += tmp * tmp;
          if      (tmp > max_latency) { max_latency = tmp; }
          else if (tmp < min_latency) { min_latency = tmp; }
        }
      mean_latency = sum / record_len;
      variance = (sq_sum / record_len) - (mean_latency * mean_latency);
      stddev = sqrt(variance);
      // Time tm (long, long) = 4byte + 4byte [Mbps]
      throughput = ((((size * m_varsize) + 8) * 8) / mean_latency) / (1024 * 1024);

      // size[byte], min[s], max[s], mean[s], stddev[s], throughpiut[Mbps]
      m_fs << size << "\t";
      m_fs << min_latency << "\t" << max_latency << "\t";
      m_fs << mean_latency << "\t" << stddev << "\t";
      m_fs << throughput << std::endl;

#ifdef DEBUG
      std::cout << "==============================" << std::endl;
      std::cout << size << "\t";
      std::cout << min_latency << "\t" << max_latency << "\t";
      std::cout << mean_latency << "\t" << stddev << "\t";
      std::cout << throughput << std::endl;
#endif // DEBUG
      // reset size/index variables
      record_num = 0;
      record_ptr = 0;
      if (seq_length < size)
        {
          coil::Async* async;
          async = coil::AsyncInvoker(this, std::mem_fun(&Throughput::exit));
          async->invoke();
        }
    }
  // measuring latency
  coil::TimeValue send_time(tm.sec, tm.nsec/1000);
  m_record[record_ptr] = received_time - send_time;
  size = seq_length;
  record_ptr++; record_num++;
  if ((long)record_ptr == m_maxsample) { record_ptr = 0; }
  return;
}

void Throughput::setConnectorProfile(const RTC::ConnectorInfo &info)
{
  m_outputfile = m_datatype + "-" + info.properties["interface_type"]
    + m_filesuffix + ".dat";

  if (!m_fs.is_open())
    {
      m_fs.open(m_outputfile.c_str(), std::ios::out);
      if (!m_fs.is_open())
        {
          std::cerr << "File open failed!!" << std::endl;
          return;
        }
    }

  // print connector profile as comment lines
  m_fs << "# Profile::name:      " << info.name    << std::endl;
  m_fs << "# Profile::id:        " << info.id      << std::endl;
  m_fs << "# Profile::properties: "            << std::endl;
  std::stringstream ss;
  ss << info.properties;
  coil::vstring propv = coil::split(ss.str(), "\n");
  for (size_t i(0); i < propv.size(); ++i)
    {
      m_fs << "# " << propv[i] << std::endl;
    }

  // print header
  m_fs << "size[byte]\tmin[s]\tmax[s]\tmean[s]\tstddev[s]\tthroughpiut[Mbps]";
  m_fs << std::endl;
  m_record.resize(m_maxsample);

}

CORBA::ULong Throughput::getInPortConnectorSize()
{
	CORBA::ULong count = 0;
	std::string datatype = coil::normalize(m_datatype);
	if (datatype == "octet")
	{
		count = m_inOctetIn.get_connector_profiles()->length();
	}
	else if (datatype == "short")
	{
		count = m_inShortIn.get_connector_profiles()->length();
	}
	else if (datatype == "long")
	{
		count = m_inLongIn.get_connector_profiles()->length();
	}
	else if (datatype == "float")
	{
		count = m_inFloatIn.get_connector_profiles()->length();
	}
	else if (datatype == "double")
	{
		count = m_inDoubleIn.get_connector_profiles()->length();
	}
	return count;
}


extern "C"
{
 
  void ThroughputInit(RTC::Manager* manager)
  {
    coil::Properties profile(analyzer_spec);
    manager->registerFactory(profile,
                             RTC::Create<Throughput>,
                             RTC::Delete<Throughput>);
  }
  
};


