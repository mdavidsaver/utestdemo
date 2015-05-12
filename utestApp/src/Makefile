TOP=../..

include $(TOP)/configure/CONFIG

TESTPROD_HOST += utest
TESTS += utest
TARGETS += $(COMMON_DIR)/utest.dbd

utest_DBD += base.dbd
utest_DBD += dut.dbd

utest_SRCS += utest_registerRecordDeviceDriver.cpp
utest_SRCS += dut.c
utest_SRCS += unittest.c

utest_LIBS += $(EPICS_BASE_IOC_LIBS)

TESTSCRIPTS_HOST += $(TESTS:%=%.t)

include $(TOP)/configure/RULES
