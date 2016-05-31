LOCAL_PATH:= $(call my-dir)


ALL_SOURCES := \
	../../../src/nhr_common.c \
	../../../src/nhr_memory.c \
	../../../src/nhr_request_private.c \
	../../../src/nhr_request_public.c \
	../../../src/nhr_response.c \
	../../../src/nhr_string.c \
	../../../src/nhr_thread.c


ALL_INCLUDES := $(LOCAL_PATH)/../../../

ALL_CFLAGS := -w

include $(CLEAR_VARS)
LOCAL_SRC_FILES := $(ALL_SOURCES)
LOCAL_C_INCLUDES += $(ALL_INCLUDES)
LOCAL_CFLAGS += $(ALL_CFLAGS)
LOCAL_MODULE := libnhr
LOCAL_LDLIBS += -llog
include $(BUILD_SHARED_LIBRARY)

