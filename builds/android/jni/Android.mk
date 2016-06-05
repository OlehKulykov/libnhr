LOCAL_PATH:= $(call my-dir)


ALL_SOURCES := \
	../../../src/nhr_common.c \
	../../../src/nhr_map.c \
	../../../src/nhr_memory.c \
	../../../src/nhr_request_method_common.c \
	../../../src/nhr_request_method_get.c \
	../../../src/nhr_request_method_post.c \
	../../../src/nhr_request_private.c \
	../../../src/nhr_request_public.c \
	../../../src/nhr_response.c \
	../../../src/nhr_string.c \
	../../../src/nhr_thread.c


ALL_INCLUDES := $(LOCAL_PATH)/../../../

ALL_CFLAGS := \
#	-DNHR_NO_GET=1 \
#	-DNHR_NO_POST=1 \
#	-DNHR_NO_CHUNKED=1 \
	-w
	

include $(CLEAR_VARS)
LOCAL_SRC_FILES := $(ALL_SOURCES)
LOCAL_C_INCLUDES += $(ALL_INCLUDES)
LOCAL_CFLAGS += $(ALL_CFLAGS)
LOCAL_MODULE := libnhr
LOCAL_LDLIBS += -llog
include $(BUILD_SHARED_LIBRARY)

