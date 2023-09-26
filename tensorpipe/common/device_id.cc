#include <iostream>

#include "device_id.h"
#include "defs.h"

namespace tensorpipe_npu {

int gDeviceId;
void setDeviceId(int deviceId){
    TP_VLOG(5) << "Set device id for tensorpipe:" << deviceId;
    gDeviceId = deviceId;
}
int getDeviceId(){return gDeviceId;}

}
