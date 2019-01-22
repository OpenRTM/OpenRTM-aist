cmake_minimum_required (VERSION 2.6)

add_subdirectory(sdo)
add_subdirectory(local_service)
add_subdirectory(ec)
add_subdirectory(logger)


if(VXWORKS)
	if(RTP)
	else(RTP)
		add_subdirectory(interrupt_task)
	endif(RTP)
endif(VXWORKS)

if(SSL_ENABLE)
	add_subdirectory(ssl)
endif(SSL_ENABLE)