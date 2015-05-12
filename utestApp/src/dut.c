#include <epicsMath.h>

#include <subRecord.h>
#include <recGbl.h>
#include <alarm.h>
#include <registryFunction.h>
#include <epicsExport.h>

static
long mysub(subRecord *prec)
{
    double dV = prec->b - prec->a;
    if(dV<0) {
        (void)recGblSetSevr(prec, READ_ALARM, INVALID_ALARM);
        return -1;
    }
    prec->val = !!prec->c ? log(dV) : prec->d;
    return 0;
}

epicsRegisterFunction(mysub);
