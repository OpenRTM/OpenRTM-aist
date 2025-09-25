FROM openrtm/devel-rtm:ubuntu22.04

COPY OpenRTM-aist /root/OpenRTM-aist
RUN cmake\
 -DSSL_ENABLE=ON\
 -DHTTP_ENABLE=ON\
 -DOBSERVER_ENABLE=ON\
 -DFLUENTBIT_ENABLE=ON\
 -DFLUENTBIT_ROOT=/usr/local\
 -DFLUENTBIT_ADD_INCLUDE=ON\
 -DFASTRTPS_ENABLE=ON\
 -DROS2_ENABLE=ON\
 -DCMAKE_BUILD_TYPE=Release\
 -S /root/OpenRTM-aist\
 -B /tmp/rtm/build\
 && cmake --build /tmp/rtm/build -j $(nproc)
