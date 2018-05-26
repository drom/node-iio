#include <nan.h>
#include <iio.h>
#include "jshandle.h"
#include "macros.h"

class iioContext : public JSHandle<iioContext> {
public:
    static const char *jsClassName() { return "iioContext"; }
};

class iioScanContext : public JSHandle<iioScanContext> {
public:
    static const char *jsClassName() { return "iioScanContext"; }
};

class iioContextInfo : public JSHandle<iioContextInfo> {
public:
    static const char *jsClassName() { return "iioContextInfo"; }
};

class iioDevice : public JSHandle<iioDevice> {
public:
    static const char *jsClassName() { return "iioDevice"; }
};

class iioChannel : public JSHandle<iioChannel> {
public:
    static const char *jsClassName() { return "iioChannel"; }
};

using namespace v8;

// Get the number of available backends.
// Returns:
//      The number of available backends
NAN_METHOD(get_backends_count) {
    CALL_LIBIIO_UINT(iio_get_backends_count())
}

// Retrieve the name of a given backend.
// Parameters:
//      index: The index corresponding to the attribute
// Returns:
//      On success, a pointer to a static NULL-terminated string
//      If the index is invalid, NULL is returned
NAN_METHOD(get_backend) {
    ASSERT_UINT(info[0], index)
    CALL_LIBIIO_CONST_CHAR(iio_get_backend(index))
}

// Create a scan context.
// Parameters:
//      backend: A NULL-terminated string containing the backend to use for scanning. If NULL, all the available backends are used.
//      flags: Unused for now. Set to 0.
// Returns:
//      on success, a pointer to a iio_scan_context structure
//      On failure, NULL is returned and errno is set appropriately
NAN_METHOD(create_scan_context) {
    ASSERT_BUFFER(info[0], uri)
    ASSERT_UINT(info[1], flags)
    iio_scan_context *cxt;
    cxt = iio_create_scan_context(CDATA(uri), flags);
    Local<Object> jscxt = iioScanContext::New(cxt);
    info.GetReturnValue().Set(jscxt);
}

// Enumerate available contexts.
// Parameters:
//      ctx: A pointer to an iio_scan_context structure
//      info: A pointer to a 'const struct iio_context_info **' typed variable. The pointed variable will be initialized on success.
// Returns:
//      On success, the number of contexts found.
//      On failure, a negative error number.

NAN_METHOD(scan_context_get_info_list) {
    ASSERT_OBJECT(info[0], jsctx)
    iio_scan_context *cxt = (struct iio_scan_context *)iioScanContext::Resolve(jsctx);
    ASSERT_OBJECT(info[1], jsinfoo)
    iio_context_info ***infoo = (struct iio_context_info ***)iioContextInfo::Resolve(jsinfoo);
    CALL_LIBIIO_UINT(iio_scan_context_get_info_list(cxt, infoo))
}

// Create a context from a URI description.
// Parameters:
//      uri: A URI describing the context location
// Returns:
//      On success, a pointer to a iio_context structure
//      On failure, NULL is returned and errno is set appropriately
NAN_METHOD(create_context_from_uri) {
    ASSERT_BUFFER(info[0], uri)
    iio_context *cxt;
    cxt = iio_create_context_from_uri(CDATA(uri));
    Local<Object> jscxt = iioContext::New(cxt);
    info.GetReturnValue().Set(jscxt);
}

// Enumerate the devices found in the given context.
// Parameters:
//      ctx: A pointer to an iio_context structure
// Returns
//      The number of devices found
NAN_METHOD(context_get_devices_count) {
    ASSERT_OBJECT(info[0], jsctx)
    iio_context *cxt = (struct iio_context *)iioContext::Resolve(jsctx);
    CALL_LIBIIO_UINT(iio_context_get_devices_count(cxt))
}

// Get the device present at the given index.
// Parameters
//      ctx: A pointer to an iio_context structure
//      index:The index corresponding to the device
// Returns
//      On success, a pointer to an iio_device structure
//      If the index is invalid, NULL is returned
NAN_METHOD(context_get_device) {
    ASSERT_OBJECT(info[0], jsctx)
    iio_context *cxt = (struct iio_context *)iioContext::Resolve(jsctx);
    ASSERT_UINT(info[1], index)
    iio_device *dev;
    dev = iio_context_get_device(cxt, index);
    Local<Object> jsdev = iioDevice::New(dev);
    info.GetReturnValue().Set(jsdev);
}

// Retrieve the device ID (e.g. iio:device0)
// Parameters
//      dev: A pointer to an iio_device structure
// Returns
//      A pointer to a static NULL-terminated string
NAN_METHOD(device_get_id) {
    ASSERT_OBJECT(info[0], jsdev)
    iio_device *dev = (struct iio_device *)iioDevice::Resolve(jsdev);
    CALL_LIBIIO_CONST_CHAR(iio_device_get_id(dev))
}

// Retrieve the device name (e.g. xadc)
// Parameters
//      dev: A pointer to an iio_device structure
// Returns
//      A pointer to a static NULL-terminated string
//      NOTE: if the device has no name, NULL is returned.
NAN_METHOD(device_get_name) {
    ASSERT_OBJECT(info[0], jsdev)
    iio_device *dev = (struct iio_device *)iioDevice::Resolve(jsdev);
    CALL_LIBIIO_CONST_CHAR(iio_device_get_name(dev))
}

// Enumerate the channels of the given device.
// Parameters
//      dev: A pointer to an iio_device structure
// Returns
//      The number of channels found
NAN_METHOD(device_get_channels_count) {
    ASSERT_OBJECT(info[0], jsdev)
    iio_device *dev = (struct iio_device *)iioDevice::Resolve(jsdev);
    CALL_LIBIIO_UINT(iio_device_get_channels_count(dev))
}

// Get the channel present at the given index.
// Parameters
//      dev: A pointer to an iio_device structure
//      index: The index corresponding to the channel
// Returns
//      On success, a pointer to an iio_channel structure
//      If the index is invalid, NULL is returned
NAN_METHOD(device_get_channel) {
    ASSERT_OBJECT(info[0], jsdev)
    iio_device *dev = (struct iio_device *)iioContext::Resolve(jsdev);
    ASSERT_UINT(info[1], index)
    iio_channel *cha;
    cha = iio_device_get_channel(dev, index);
    Local<Object> jscha = iioChannel::New(cha);
    info.GetReturnValue().Set(jscha);
}

// iio_context_destroy
// iio_context_find_device
// iio_channel_disable
// iio_channel_enable
// iio_channel_is_enabled
// iio_device_create_buffer
// iio_channel_get_data_format
// iio_buffer_first
// iio_buffer_start
// iio_buffer_end
// iio_buffer_step
// iio_channel_convert_inverse
// iio_device_find_channel



NAN_MODULE_INIT(Init) {
    EXPORT_FUNCTION(get_backends_count)
    EXPORT_FUNCTION(get_backend)
    EXPORT_FUNCTION(create_scan_context)
    EXPORT_FUNCTION(scan_context_get_info_list)
    EXPORT_FUNCTION(create_context_from_uri)
    EXPORT_FUNCTION(context_get_devices_count)
    EXPORT_FUNCTION(context_get_device)
    EXPORT_FUNCTION(device_get_id)
    EXPORT_FUNCTION(device_get_name)
    EXPORT_FUNCTION(device_get_channels_count)
    EXPORT_FUNCTION(device_get_channel)
}

NODE_MODULE(libiio, Init)

#undef CDATA
#undef LOCAL_STRING
#undef LOCAL_FUNCTION
#undef EXPORT_NUMBER
#undef EXPORT_FUNCTION
#undef CALL_LIBIIO_INT
#undef CALL_LIBIIO_UINT
#undef CALL_LIBIIO_CONST_CHAR
#undef ASSERT_UINT
#undef ASSERT_BUFFER
#undef ASSERT_OBJECT
#undef ASSERT_SCAN_CXT
#undef ASSERT_CXT
#undef ASSERT_CXT_INFO
