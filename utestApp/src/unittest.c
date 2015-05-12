
#include <dbStaticLib.h>
#include <dbAccess.h>
#include <dbDefs.h>
#include <alarm.h>
#include <errlog.h>
#include <subRecord.h>

#include <dbUnitTest.h>
#include <testMain.h>

/* hook for the registration code generated from utest.dbd */
void utest_registerRecordDeviceDriver(DBBASE *pdbbase);

static void doSub(void);
static void doCalc(void);

/* The entry point for this test */
MAIN(unittest)
{
    testPlan(31);
    doSub();
    doCalc();
    return testDone();
}

static struct {
    double a, b, c, d, out;
    short sevr;
} testData[] = {
    {1, 4, 3, 4, 0, 0},
    {2, 1,-3, 4, 0, INVALID_ALARM},
    {1, 1, 3, 4, 0, INVALID_ALARM},
    {1, 1, 0, 4, 4, 0},
};

static void doSub(void)
{
    size_t i;
    subRecord *prec;

    testDiag("Test subRecord calculation");

    /* Call this before any other PDB functions */
    testdbPrepare();

    /* Activities and ordering is the same as normal IOC startup
     * 1. Load utest.dbd
     * 2. Run registeration
     * 3. Load dut.db w/ some macro definitions
     * 4. iocInit()
     */

    testdbReadDatabase("utest.dbd", NULL, NULL);
    utest_registerRecordDeviceDriver(pdbbase);
    testdbReadDatabase("dut.db", NULL, "N=test:,A=42");

    /* Once a record has been loaded we can peek at it's storage.
     * As usual, not all fields are initialized yet (not arrays).
     */
    prec = (subRecord*)testdbRecordPtr("test:s");
    testOk(prec!=NULL, "%p!=NULL", prec);

    eltc(0);
    testIocInitOk();
    eltc(1);

    /* The IOC is now running.
     * Records are fully initialized and scan tasks are looping.
     * Lock records before access.
     */

    for(i=0; i<NELEMENTS(testData); i++)
    {
        testDiag("Inputs A=%f B=%f C=%f D=%f", testData[i].a, testData[i].b,
                 testData[i].c, testData[i].d);
        testDiag("Outputs VAL=%f SEVR=%d", testData[i].out, testData[i].sevr);

        /* Use dbPutField and dbGetField just like RSRV (CA server).
         * These functions handle locking internally
         */

        testdbPutFieldOk("test:s.A", DBR_DOUBLE, testData[i].a);
        testdbPutFieldOk("test:s.B", DBR_DOUBLE, testData[i].b);
        testdbPutFieldOk("test:s.C", DBR_DOUBLE, testData[i].c);
        testdbPutFieldOk("test:s.D", DBR_DOUBLE, testData[i].d);

        testdbGetFieldEqual("test:s", DBR_DOUBLE, testData[i].out);
        testdbGetFieldEqual("test:s.SEVR", DBR_SHORT, testData[i].sevr);
    }

    /* Shut down scanning (and other) threads.
     * Record fields are de-initialized (eg. DBLINK are cleaned and zero'd)
     * Allocations made be record and device supports are leaked
     * as no cleanup callbacks are presently available.
     */
    testIocShutdownOk();

    /* Final de-allocation of PDB, initHook, and function registry */
    testdbCleanup();
}

static void doCalc(void)
{
    dbCommon *pcalc;
    subRecord *prec;
    testDiag("Test subRecord DB_LINK");

    /* One more time to demonstrate safe access
     * to record struct
     */

    testdbPrepare();

    testdbReadDatabase("utest.dbd", NULL, NULL);
    utest_registerRecordDeviceDriver(pdbbase);
    testdbReadDatabase("dut.db", NULL, "N=test:,A=42");

    prec = (subRecord*)testdbRecordPtr("test:s");
    pcalc = testdbRecordPtr("test:t");
    testOk(prec!=NULL, "%p!=NULL", prec);
    testOk(pcalc!=NULL, "%p!=NULL", pcalc);

    eltc(0);
    testIocInitOk();
    eltc(1);

    /* Lock the subRecord "test:s" so we can inspect it's fields. */
    dbScanLock((dbCommon*)prec);
    testOk(prec->a==42,"%f==42", prec->a);
    dbScanUnlock((dbCommon*)prec);

    /* Put to .PROC to trigger this calcout record to process */
    testdbPutFieldOk("test:t.PROC", DBR_LONG, 1);

    /* See that something happend */
    dbScanLock((dbCommon*)prec);
    testOk(prec->a==43,"%f==43", prec->a);
    dbScanUnlock((dbCommon*)prec);

    /* Again by scanning in this thread */
    dbScanLock(pcalc);
    dbProcess(pcalc);
    dbScanUnlock(pcalc);

    dbScanLock((dbCommon*)prec);
    testOk(prec->a==44,"%f==44", prec->a);
    dbScanUnlock((dbCommon*)prec);

    testIocShutdownOk();
    testdbCleanup();
}
