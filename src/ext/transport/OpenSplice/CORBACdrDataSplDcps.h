#ifndef CORBACDRDATASPLTYPES_H
#define CORBACDRDATASPLTYPES_H

#include <c_base.h>
#include <c_misc.h>
#include <c_sync.h>
#include <c_collection.h>
#include <c_field.h>
#include <v_copyIn.h>

#include "ccpp_CORBACdrData.h"


struct OpenSplice_OpenRTM_CORBACdrData ;
v_copyin_result OpenSplice_OpenRTM_CORBACdrData_copyIn(c_base base, const struct OpenRTM_OpenSplice::CORBACdrData *from, struct OpenSplice_OpenRTM_CORBACdrData *to);
void OpenSplice_OpenRTM_CORBACdrData_copyOut(const void *_from, void *_to);
struct OpenSplice_OpenRTM_CORBACdrData {
    c_sequence data;
};

#undef OS_API
#endif
