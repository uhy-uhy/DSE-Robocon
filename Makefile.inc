APPL_COBJS += balancer.o balancer_param.o

APPL_CXXOBJS += LineMonitor.o CheckPointStrategy.o PassLookUpGate.o

SRCLANG := c++

ifdef CONFIG_EV3RT_APPLICATION

# Include libraries
include $(EV3RT_SDK_LIB_DIR)/libcpp-ev3/Makefile

endif