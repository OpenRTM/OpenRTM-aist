#include <rtm/Manager.h>
#include <omniORB4/httpContext.h>



void HTTPTransportSet(RTC::Manager* manager)
{
	coil::Properties &prop = manager->getConfig();
	
	std::string http_certificate_authority_file = prop["corba.http.certificate_authority_file"];
  std::string http_key_file = prop["corba.http.key_file"];
  std::string http_key_file_password = prop["corba.http.key_file_password"];
  if(!http_certificate_authority_file.empty())
    omni::httpContext::certificate_authority_file = CORBA::string_dup(http_certificate_authority_file.c_str());
  if(!http_key_file.empty())
    omni::httpContext::key_file = CORBA::string_dup(http_key_file.c_str());
  if(!http_key_file_password.empty())
    omni::httpContext::key_file_password = CORBA::string_dup(http_key_file_password.c_str());
	

	std::string corba_args = prop["corba.args"];

	if (corba_args.find("giop:http") == std::string::npos)
	{
		corba_args += " -ORBendPoint giop:http:ws:///ws";
	}

	if (!coil::toBool(prop["manager.is_master"], "YES", "NO", true))
	{
		if (prop.getProperty("corba.endpoints").empty())
		{
			if (prop.getProperty("corba.endpoint").empty())
			{
				if (prop["corba.args"].find("-ORBendPoint") == std::string::npos)
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
 	DLL_EXPORT void HTTPTransportInit(RTC::Manager* manager)
	{
		
		HTTPTransportSet(manager);
	}
}
