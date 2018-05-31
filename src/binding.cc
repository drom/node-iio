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

/*
    Get the version of the libiio library.

    Parameters
        major:  A pointer to an unsigned integer (NULL accepted)
        minor:  A pointer to an unsigned integer (NULL accepted)
        git_tag:A pointer to a 8-characters buffer (NULL accepted)
*/
NAN_METHOD(library_get_version) {
    ASSERT_BUFFER(info[0], jsver)
    char *ver = CDATA(jsver);
    // unsigned int *major;
    // unsigned int *minor;
    // char git_tag[8];
    iio_library_get_version(
        (unsigned int*)ver,
        (unsigned int*)(ver + 4),
        ver + 8
    );
}

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
    CALL_LIBIIO_CONST_CHAR_OR_NULL(iio_get_backend(index))
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

// TODO
// iio_context_destroy
// iio_context_find_device


// Retrieve the device ID (e.g. iio:device0)
// Parameters
//      dev: A pointer to an iio_device structure
// Returns
//      A pointer to a static NULL-terminated string
NAN_METHOD(device_get_id) {
    ASSERT_OBJECT(info[0], jsdev)
    iio_device *dev = (struct iio_device *)iioDevice::Resolve(jsdev);
    CALL_LIBIIO_CONST_CHAR_OR_NULL(iio_device_get_id(dev))
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
    CALL_LIBIIO_CONST_CHAR_OR_NULL(iio_device_get_name(dev))
}

/*
    Enumerate the device-specific attributes of the given device.

    Parameters
        dev:  A pointer to an iio_device structure
    Returns
        The number of device-specific attributes found
*/
NAN_METHOD(device_get_attrs_count) {
    ASSERT_OBJECT(info[0], jsdev)
    iio_device *dev = (struct iio_device *)iioDevice::Resolve(jsdev);
    CALL_LIBIIO_UINT(iio_device_get_attrs_count(dev))
}

/*
    Get the device-specific attribute present at the given index.

    Parameters
        dev:    A pointer to an iio_device structure
        index:  The index corresponding to the attribute
    Returns
        On success, a pointer to a static NULL-terminated string
        If the index is invalid, NULL is returned
*/
NAN_METHOD(device_get_attr) {
    ASSERT_OBJECT(info[0], jsdev)
    iio_device *dev = (struct iio_device *)iioDevice::Resolve(jsdev);
    ASSERT_UINT(info[1], index)
    CALL_LIBIIO_CONST_CHAR_OR_NULL(iio_device_get_attr(dev, index))
}

/*
    Read the content of the given device-specific attribute.

    Parameters
        dev:    A pointer to an iio_device structure
        attr:   A NULL-terminated string corresponding to the name of the attribute
        dst:    A pointer to the memory area where the NULL-terminated string
                corresponding to the value read will be stored
        len:    The available length of the memory area, in bytes

    Returns
        On success, the number of bytes written to the buffer
        On error, a negative errno code is returned

    NOTE: By passing NULL as the "attr" argument to iio_device_attr_read,
    it is now possible to read all of the attributes of a device.

    The buffer is filled with one block of data per attribute of the device,
    by the order they appear in the iio_device structure.

    The first four bytes of one block correspond to a 32-bit signed value in
    network order. If negative, it corresponds to the errno code that were
    returned when reading the attribute; if positive, it corresponds to the
    length of the data read. In that case, the rest of the block contains the data.
*/
NAN_METHOD(device_attr_read) {
    ASSERT_OBJECT(info[0], jsdev)
    iio_device *dev = (struct iio_device *)iioDevice::Resolve(jsdev);
    ASSERT_BUFFER(info[1], attr)
    ASSERT_BUFFER(info[2], dst)
    ASSERT_UINT(info[3], len)
    CALL_LIBIIO_INT(iio_device_attr_read(dev, CDATA(attr), CDATA(dst), len))
}

// TODO
// iio_device_attr_read_all()
// iio_device_attr_read_bool() +
// iio_device_attr_read_longlong() +
// iio_device_attr_read_double()

// iio_device_attr_write()
// iio_device_attr_write_all()
// iio_device_attr_write_bool() +
// iio_device_attr_write_longlong()
// iio_device_attr_write_double()
// iio_device_attr_write_raw() +

// iio_device_reg_read() +
// iio_device_reg_write() +


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
    iio_device *dev = (struct iio_device *)iioDevice::Resolve(jsdev);
    ASSERT_UINT(info[1], index)
    iio_channel *cha;
    cha = iio_device_get_channel(dev, index);
    Local<Object> jscha = iioChannel::New(cha);
    info.GetReturnValue().Set(jscha);
}

// TODO
// iio_device_find_channel() +
// iio_device_get_sample_size() +
// iio_device_create_buffer() +

/*
    Return True if the given channel is an output channel.

    Parameters
        chn:  A pointer to an iio_channel structure
    Returns
        True if the channel is an output channel, False otherwise
*/
NAN_METHOD(channel_is_output) {
  ASSERT_OBJECT(info[0], jscha)
  iio_channel *cha = (struct iio_channel *)iioChannel::Resolve(jscha);
  CALL_LIBIIO_BOOL(iio_channel_is_output(cha))
}

/*
    Retrieve the channel ID (e.g. voltage0)

    Parameters
        chn:  A pointer to an iio_channel structure
    Returns
        A pointer to a static NULL-terminated string
*/
NAN_METHOD(channel_get_id) {
    ASSERT_OBJECT(info[0], jscha)
    iio_channel *cha = (struct iio_channel *)iioChannel::Resolve(jscha);
    CALL_LIBIIO_CONST_CHAR_OR_NULL(iio_channel_get_id(cha))
}

/*
    Retrieve the channel name (e.g. vccint)

    Parameters
        chn:    A pointer to an iio_channel structure
    Returns
        A pointer to a static NULL-terminated string
        NOTE: if the channel has no name, NULL is returned.
*/
NAN_METHOD(channel_get_name) {
    ASSERT_OBJECT(info[0], jscha)
    iio_channel *cha = (struct iio_channel *)iioChannel::Resolve(jscha);
    CALL_LIBIIO_CONST_CHAR_OR_NULL(iio_channel_get_name(cha))
}

/*
    Returns True if the channel is enabled.

    Parameters
        chn:  A pointer to an iio_channel structure
    Returns
        True if the channel is enabled, False otherwise
*/
NAN_METHOD(channel_is_enabled) {
  ASSERT_OBJECT(info[0], jscha)
  iio_channel *cha = (struct iio_channel *)iioChannel::Resolve(jscha);
  CALL_LIBIIO_BOOL(iio_channel_is_enabled(cha))
}

/*
    Get the type of the given channel.

    Parameters
        chn:  A pointer to an iio_channel structure
    Returns
        The type of the channel
*/
NAN_METHOD(channel_get_type) {
  ASSERT_OBJECT(info[0], jscha)
  iio_channel *cha = (struct iio_channel *)iioChannel::Resolve(jscha);
  CALL_LIBIIO_INT(iio_channel_get_type(cha))
}

/*
    Disable the given channel.

    Parameters
        chn:  A pointer to an iio_channel structure
*/
NAN_METHOD(channel_disable) {
  ASSERT_OBJECT(info[0], jscha)
  iio_channel *cha = (struct iio_channel *)iioChannel::Resolve(jscha);
  iio_channel_disable(cha);
}

/*
    Enable the given channel.

    Parameters
        chn:  A pointer to an iio_channel structure

    NOTE:Before creating an iio_buffer structure with iio_device_create_buffer,
    it is required to enable at least one channel of the device to read from.
*/
NAN_METHOD(channel_enable) {
  ASSERT_OBJECT(info[0], jscha)
  iio_channel *cha = (struct iio_channel *)iioChannel::Resolve(jscha);
  iio_channel_enable(cha);
}

/*
    Enumerate the channel-specific attributes of the given channel.

    Parameters
        chn:  A pointer to an iio_channel structure
    Returns
        The number of channel-specific attributes found
*/
NAN_METHOD(channel_get_attrs_count) {
    ASSERT_OBJECT(info[0], jscha)
    iio_channel *cha = (struct iio_channel *)iioChannel::Resolve(jscha);
    CALL_LIBIIO_UINT(iio_channel_get_attrs_count(cha))
}

/*
    Get the channel-specific attribute present at the given index.

    Parameters
        chn:    A pointer to an iio_channel structure
        index:  The index corresponding to the attribute
    Returns
        On success, a pointer to a static NULL-terminated string
        If the index is invalid, NULL is returned
*/
NAN_METHOD(channel_get_attr) {
    ASSERT_OBJECT(info[0], jscha)
    iio_channel *cha = (struct iio_channel *)iioChannel::Resolve(jscha);
    ASSERT_UINT(info[1], index)
    CALL_LIBIIO_CONST_CHAR_OR_NULL(iio_channel_get_attr(cha, index))
}

/*
    Read the content of the given channel-specific attribute.

    Parameters
        chn:  A pointer to an iio_channel structure
        attr: A NULL-terminated string corresponding to the name of the attribute
        dst:  A pointer to the memory area where the NULL-terminated string
              corresponding to the value read will be stored
        len:  The available length of the memory area, in bytes
    Returns
        On success, the number of bytes written to the buffer
        On error, a negative errno code is returned

    NOTE:By passing NULL as the "attr" argument to iio_channel_attr_read, it is
    now possible to read all of the attributes of a channel.

    The buffer is filled with one block of data per attribute of the channel,
    by the order they appear in the iio_channel structure.

    The first four bytes of one block correspond to a 32-bit signed value in
    network order. If negative, it corresponds to the errno code that were
    returned when reading the attribute; if positive, it corresponds to the
    length of the data read. In that case, the rest of the block contains the data.
*/
NAN_METHOD(channel_attr_read) {
    ASSERT_OBJECT(info[0], jscha)
    iio_channel *cha = (struct iio_channel *)iioChannel::Resolve(jscha);
    ASSERT_BUFFER(info[1], attr)
    ASSERT_BUFFER(info[2], dst)
    ASSERT_UINT(info[3], len)
    CALL_LIBIIO_INT(iio_channel_attr_read(cha, CDATA(attr), CDATA(dst), len))
}

// TODO
// iio_channel_attr_read_all()
// iio_channel_attr_read_bool()
// iio_channel_attr_read_longlong() +
// iio_channel_attr_read_double()

/*
    Set the value of the given channel-specific attribute.

    Parameters
        chn:    A pointer to an iio_channel structure
        attr:   A NULL-terminated string corresponding to the name of the attribute
        src:    A pointer to the data to be written
        len:    The number of bytes that should be written
    Returns
        On success, the number of bytes written
        On error, a negative errno code is returned
*/
NAN_METHOD(channel_attr_write_raw) {
    ASSERT_OBJECT(info[0], jscha)
    iio_channel *cha = (struct iio_channel *)iioChannel::Resolve(jscha);
    ASSERT_BUFFER(info[1], attr)
    ASSERT_BUFFER(info[2], src)
    ASSERT_UINT(info[3], len)
    CALL_LIBIIO_INT(iio_channel_attr_write_raw(cha, CDATA(attr), CDATA(src), len))
}

/*
    Set the value of the given channel-specific attribute.

    Parameters
        chn:    A pointer to an iio_channel structure
        attr:   A NULL-terminated string corresponding to the name of the attribute
        src:    A NULL-terminated string to set the attribute to
    Returns
        On success, the number of bytes written
        On error, a negative errno code is returned

    NOTE: By passing NULL as the "attr" argument to iio_channel_attr_write,
    it is now possible to write all of the attributes of a channel.

    The buffer must contain one block of data per attribute of the channel,
    by the order they appear in the iio_channel structure.

    The first four bytes of one block correspond to a 32-bit signed value in
    network order. If negative, the attribute is not written; if positive,
    it corresponds to the length of the data to write. In that case,
    the rest of the block must contain the data.
*/
NAN_METHOD(channel_attr_write) {
    ASSERT_OBJECT(info[0], jscha)
    iio_channel *cha = (struct iio_channel *)iioChannel::Resolve(jscha);
    ASSERT_BUFFER(info[1], attr)
    ASSERT_BUFFER(info[2], src)
    CALL_LIBIIO_INT(iio_channel_attr_write(cha, CDATA(attr), CDATA(src)))
}
// iio_channel_attr_write()
// iio_channel_attr_write_all()
// iio_channel_attr_write_bool()
// iio_channel_attr_write_longlong() +
// iio_channel_attr_write_double()

// iio_channel_get_data_format
// iio_channel_convert_inverse

// iio_buffer_first
// iio_buffer_start
// iio_buffer_end
// iio_buffer_step
// iio_buffer_destroy

// iio_buffer_foreach_sample() +



NAN_MODULE_INIT(Init) {
    EXPORT_FUNCTION(library_get_version)
    EXPORT_FUNCTION(get_backends_count)
    EXPORT_FUNCTION(get_backend)
    EXPORT_FUNCTION(create_scan_context)
    EXPORT_FUNCTION(scan_context_get_info_list)
    EXPORT_FUNCTION(create_context_from_uri)
    EXPORT_FUNCTION(context_get_devices_count)
    EXPORT_FUNCTION(context_get_device)
    EXPORT_FUNCTION(device_get_id)
    EXPORT_FUNCTION(device_get_name)
    EXPORT_FUNCTION(device_get_attrs_count)
    EXPORT_FUNCTION(device_get_attr)
    EXPORT_FUNCTION(device_attr_read)
    EXPORT_FUNCTION(device_get_channels_count)
    EXPORT_FUNCTION(device_get_channel)
    EXPORT_FUNCTION(channel_is_output)
    EXPORT_FUNCTION(channel_get_id)
    EXPORT_FUNCTION(channel_get_name)
    EXPORT_FUNCTION(channel_is_enabled)
    EXPORT_FUNCTION(channel_get_type)
    EXPORT_FUNCTION(channel_disable)
    EXPORT_FUNCTION(channel_enable)
    EXPORT_FUNCTION(channel_get_attrs_count)
    EXPORT_FUNCTION(channel_get_attr)
    EXPORT_FUNCTION(channel_attr_read)
    EXPORT_FUNCTION(channel_attr_write_raw)
    EXPORT_FUNCTION(channel_attr_write)
}

NODE_MODULE(libiio, Init)

#undef CDATA
#undef LOCAL_STRING
#undef LOCAL_FUNCTION
#undef EXPORT_NUMBER
#undef EXPORT_FUNCTION
#undef CALL_LIBIIO_BOOL
#undef CALL_LIBIIO_INT
#undef CALL_LIBIIO_UINT
#undef CALL_LIBIIO_CONST_CHAR
#undef CALL_LIBIIO_CONST_CHAR_OR_NULL
#undef ASSERT_UINT
#undef ASSERT_BUFFER
#undef ASSERT_OBJECT
#undef ASSERT_SCAN_CXT
#undef ASSERT_CXT
#undef ASSERT_CXT_INFO
