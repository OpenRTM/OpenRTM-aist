#include <rtm/Manager.h>
#include <omniORB4/sslContext.h>



void SSLTransportSet(RTC::Manager* manager)
{
	coil::Properties &prop = manager->getConfig();
	
	
	sslContext::certificate_authority_file = CORBA::string_dup(prop["corba.ssl.certificate_authority_file"].c_str());
	sslContext::key_file = CORBA::string_dup(prop["corba.ssl.key_file"].c_str());
	sslContext::key_file_password = CORBA::string_dup(prop["corba.ssl.key_file_password"].c_str());
	

	std::string corba_args = prop["corba.args"];

	corba_args += " -ORBendPoint giop:ssl::";

	if (!coil::toBool(prop["manager.is_master"], "YES", "NO", true))
	{
		if (prop.getProperty("corba.endpoints").empty())
		{
			if (prop.getProperty("corba.endpoint").empty())
			{
				if (prop["corba.args"].find("-ORBendPoint") == -1)
				{
					corba_args += " -ORBendPoint giop:tcp::";
				}
			}
		}
	}
	prop.setProperty("corba.args", corba_args);

}


extern "C"
{
 	DLL_EXPORT void SSLTransportInit(RTC::Manager* manager)
	{
		
		SSLTransportSet(manager);
	}
};