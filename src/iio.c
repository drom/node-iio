#include <stdio.h>
#include <iio.h>
#include <node_api.h>

#define DECLARE_NAPI_METHOD(name, func) { \
    napi_property_descriptor desc = { name, 0, func, 0, 0, 0, napi_default, 0 }; \
    if (napi_define_properties(env, exports, 1, &desc) != napi_ok) { \
        napi_throw_error(env, 0, "Error"); \
    } \
}

#define METHOD(name) \
    napi_value name(napi_env env, napi_callback_info info)

#define ASSERT(val, expr) \
    if (expr != napi_ok) { \
        napi_throw(env, val); \
    }

#define ASSERT_ARGC(count) \
    napi_value args[count]; \
    { \
        size_t argc = count; \
        if (napi_get_cb_info(env, info, &argc, args, 0, 0) != napi_ok) { \
            napi_throw_error(env, 0, "Error"); \
            return 0; \
        } \
        if (argc < count) { \
            napi_throw_type_error(env, 0, "Wrong number of arguments"); \
            return 0; \
        } \
    }

#define ASSERT_BOOL(name, var) \
    bool var; \
    { \
        napi_value tmp; \
        if (napi_coerce_to_bool(env, name, &tmp) != napi_ok) { \
            napi_throw(env, name); \
            return 0; \
        } \
        if (napi_get_value_bool(env, tmp, &var) != napi_ok) { \
            napi_throw(env, name); \
            return 0; \
        } \
    }

#define ASSERT_UINT(name, var) \
    uint32_t var; \
    { \
        napi_valuetype valuetype; \
        if (napi_typeof(env, name, &valuetype) != napi_ok) { \
            napi_throw(env, name); \
            return 0; \
        } \
        if (valuetype != napi_number) { \
            napi_throw_type_error(env, 0, "Wrong arguments"); \
            return 0; \
        } \
        if (napi_get_value_uint32(env, name, &var) != napi_ok) { \
            napi_throw(env, name); \
            return 0; \
        } \
    }

#define ASSERT_STRING(name, var) \
    char var[256]; \
    { \
        napi_value tmp; \
        if (napi_coerce_to_string(env, name, &tmp) != napi_ok) { \
            napi_throw(env, name); \
            return 0; \
        } \
        size_t result; \
        if (napi_get_value_string_latin1(env, tmp, var, 256, &result) != napi_ok) { \
            napi_throw(env, name); \
            return 0; \
        } \
    }

#define ASSERT_OBJECT(name, var) \
    napi_value var; \
    { \
        napi_valuetype valuetype; \
        if (napi_typeof(env, name, &valuetype) != napi_ok) { \
            napi_throw(env, name); \
            return 0; \
        } \
        if (valuetype != napi_object) { \
            napi_throw_type_error(env, 0, "Wrong arguments"); \
            return 0; \
        } \
    }

#define ASSERT_EXTERNAL(name, var) { \
    napi_valuetype valuetype; \
    if (napi_typeof(env, name, &valuetype) != napi_ok) { \
        napi_throw(env, name); \
        return 0; \
    } \
    if (valuetype != napi_external) { \
        napi_throw_type_error(env, 0, "Wrong arguments"); \
        return 0; \
    } \
    if (napi_get_value_external(env, name, (void **)(&var)) != napi_ok) { \
        napi_throw(env, name); \
        return 0; \
    } \
}

#define ASSERT_FUNCTION(name, var) \
    napi_value var; \
    { \
        napi_valuetype valuetype; \
        if (napi_typeof(env, name, &valuetype) != napi_ok) { \
            napi_throw(env, name); \
            return 0; \
        } \
        if (valuetype != napi_function) { \
            napi_throw_type_error(env, 0, "Wrong arguments"); \
            return 0; \
        } \
    }

/*
    Get the version of the libiio library.

    Parameters
        major:  A pointer to an unsigned integer (NULL accepted)
        minor:  A pointer to an unsigned integer (NULL accepted)
        git_tag:A pointer to a 8-characters buffer (NULL accepted)
*/
METHOD(library_get_version) {
    // ASSERT_ARGC(0)

    unsigned int major, minor;
    char git_tag[8];
    iio_library_get_version(&major, &minor, git_tag);

    napi_value res, maj, min, tag;
    ASSERT(res, napi_create_uint32(env, major, &maj))
    ASSERT(res, napi_create_uint32(env, minor, &min))
    ASSERT(res, napi_create_string_utf8(env, git_tag, NAPI_AUTO_LENGTH, &tag))
    ASSERT(res, napi_create_object(env, &res))
    ASSERT(res, napi_set_named_property(env, res, "major", maj))
    ASSERT(res, napi_set_named_property(env, res, "minor", min))
    ASSERT(res, napi_set_named_property(env, res, "git_tag", tag))
    return res;
}


/*
    Get the number of available backends.

    Returns:
        The number of available backends
*/
METHOD(get_backends_count) {
    uint32_t value = iio_get_backends_count();

    napi_value result;
    ASSERT(result, napi_create_uint32(env, value, &result))
    return result;
}


/*
    Retrieve the name of a given backend.

    Parameters:
        index: The index corresponding to the attribute
    Returns:
        On success, a pointer to a static NULL-terminated string
        If the index is invalid, NULL is returned
*/
METHOD(get_backend) {
    ASSERT_ARGC(1)
    ASSERT_UINT(args[0], index)

    const char *str = iio_get_backend(index);

    napi_value result;
    ASSERT(result, napi_create_string_utf8(env, str, NAPI_AUTO_LENGTH, &result))
    return result;
}

/*
    Create a scan context.

    Parameters:
        backend:    A NULL-terminated string containing the backend to use
                    for scanning. If NULL, all the available backends are used.
        flags:      Unused for now. Set to 0.
    Returns:
        on success, a pointer to a iio_scan_context structure
        On failure, NULL is returned and errno is set appropriately
*/
METHOD(create_scan_context) {
    ASSERT_ARGC(2)
    ASSERT_STRING(args[0], uri)
    ASSERT_UINT(args[1], flags)

    struct iio_scan_context *cxt;
    cxt = iio_create_scan_context(uri, flags);

    napi_value res;
    if (cxt) {
        ASSERT(res, napi_create_external(env, cxt, 0, 0, &res))
    } else {
        ASSERT(res, napi_get_undefined(env, &res))
    }
    return res;
}

/*
    Enumerate available contexts.

    Parameters:
        ctx:    A pointer to an iio_scan_context structure
        info:   A pointer to a 'const struct iio_context_info **' typed variable.
                The pointed variable will be initialized on success.
    Returns:
        On success, the number of contexts found.
        On failure, a negative error number.
*/
METHOD(scan_context_get_info_list) {
    ASSERT_ARGC(1)
    struct iio_scan_context *cxt;
    ASSERT_EXTERNAL(args[0], cxt)

    struct iio_context_info **icxt;
    uint32_t count = iio_scan_context_get_info_list(cxt, &icxt);

    napi_value res, length, ref;
    ASSERT(res, napi_create_uint32(env, count, &length))
    ASSERT(res, napi_create_external(env, icxt, 0, 0, &ref))
    ASSERT(res, napi_create_object(env, &res))
    ASSERT(res, napi_set_named_property(env, res, "length", length))
    ASSERT(res, napi_set_named_property(env, res, "ref", ref))
    return res;
}

/*
    Get the URI of a discovered context.

    Parameters
        info:   A pointer to an iio_context_info structure
    Returns
        A pointer to a static NULL-terminated string
*/
METHOD(context_info_get_uri) {
    ASSERT_ARGC(1)
    struct iio_context_info **icxt;
    ASSERT_EXTERNAL(args[0], icxt)

    const char* str = iio_context_info_get_uri(*icxt);

    napi_value result;
    ASSERT(result, napi_create_string_utf8(env, str, NAPI_AUTO_LENGTH, &result));
    return result;
}

/*
    Get a description of a discovered context.

    Parameters
        info:   A pointer to an iio_context_info structure
    Returns
        A pointer to a static NULL-terminated string
*/
METHOD(context_info_get_description) {
    ASSERT_ARGC(1)
    struct iio_context_info **icxt;
    ASSERT_EXTERNAL(args[0], icxt)

    const char* str = iio_context_info_get_description(*icxt);

    napi_value result;
    ASSERT(result, napi_create_string_utf8(env, str, NAPI_AUTO_LENGTH, &result));
    return result;
}

/*
    Create a context from a URI description.

    Parameters:
        uri: A URI describing the context location
    Returns:
        On success, a pointer to a iio_context structure
        On failure, NULL is returned and errno is set appropriately
*/
METHOD(create_context_from_uri) {
    ASSERT_ARGC(1)
    ASSERT_STRING(args[0], uri)

    struct iio_context *cxt;
    cxt = iio_create_context_from_uri(uri);

    napi_value res;
    if (cxt) {
        ASSERT(res, napi_create_external(env, cxt, 0, 0, &res))
    } else {
        ASSERT(res, napi_get_undefined(env, &res))
    }
    return res;
}

/*
    Enumerate the devices found in the given context.

    Parameters:
        ctx: A pointer to an iio_context structure
    Returns
        The number of devices found
*/
METHOD(context_get_devices_count) {
    ASSERT_ARGC(1)
    struct iio_context* cxt;
    ASSERT_EXTERNAL(args[0], cxt)

    uint32_t value = iio_context_get_devices_count(cxt);

    napi_value res;
    ASSERT(res, napi_create_uint32(env, value, &res))
    return res;
}

//// context_get_device
/*
    Get the device present at the given index.

    Parameters
        ctx: A pointer to an iio_context structure
        index:The index corresponding to the device
    Returns
        On success, a pointer to an iio_device structure
        If the index is invalid, NULL is returned
*/
METHOD(context_get_device) {
    ASSERT_ARGC(2)
    struct iio_context* cxt;
    ASSERT_EXTERNAL(args[0], cxt)
    ASSERT_UINT(args[1], index)

    struct iio_device *dev;
    dev = iio_context_get_device(cxt, index);

    napi_value res;
    if (dev) {
        ASSERT(res, napi_create_external(env, dev, 0, 0, &res))
    } else {
        ASSERT(res, napi_get_undefined(env, &res))
    }
    return res;
}

//// context_find_device

//// device_get_id

/*
    Retrieve the device ID (e.g. iio:device0)

    Parameters
        dev:    A pointer to an iio_device structure
    Returns
        A pointer to a static NULL-terminated string
*/
METHOD(device_get_id) {
    ASSERT_ARGC(1)
    struct iio_device *dev;
    ASSERT_EXTERNAL(args[0], dev)

    const char *str = iio_device_get_id(dev);

    napi_value result;
    ASSERT(result, napi_create_string_utf8(env, str, NAPI_AUTO_LENGTH, &result))
    return result;
}

/*
    Retrieve the device name (e.g. xadc)

    Parameters
        dev:    A pointer to an iio_device structure
    Returns
        A pointer to a static NULL-terminated string
        NOTE: if the device has no name, NULL is returned.
*/
METHOD(device_get_name) {
    ASSERT_ARGC(1)
    struct iio_device *dev;
    ASSERT_EXTERNAL(args[0], dev)

    const char *str = iio_device_get_name(dev);

    napi_value result;
    ASSERT(result, napi_create_string_utf8(env, str, NAPI_AUTO_LENGTH, &result))
    return result;
}

/*
    Enumerate the device-specific attributes of the given device.

    Parameters
        dev:  A pointer to an iio_device structure
    Returns
        The number of device-specific attributes found
*/
METHOD(device_get_attrs_count) {
    ASSERT_ARGC(1)
    struct iio_device *dev;
    ASSERT_EXTERNAL(args[0], dev)

    uint32_t value = iio_device_get_attrs_count(dev);

    napi_value res;
    ASSERT(res, napi_create_uint32(env, value, &res))
    return res;
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
METHOD(device_get_attr) {
    ASSERT_ARGC(2)
    struct iio_device *dev;
    ASSERT_EXTERNAL(args[0], dev)
    ASSERT_UINT(args[1], index)

    const char* str = iio_device_get_attr(dev, index);

    napi_value result;
    ASSERT(result, napi_create_string_utf8(env, str, NAPI_AUTO_LENGTH, &result))
    return result;
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
METHOD(device_attr_read) {
    ASSERT_ARGC(2)
    struct iio_device *dev;
    ASSERT_EXTERNAL(args[0], dev)
    ASSERT_STRING(args[1], attr)

    uint32_t len1 = 1024;
    char* dst = malloc(len1);
    int32_t len2 = iio_device_attr_read(dev, attr, dst, len1);

    napi_value res;
    if (len2 > 0) {
        ASSERT(res, napi_create_string_utf8(env, dst, NAPI_AUTO_LENGTH, &res))
    } else {
        ASSERT(res, napi_get_undefined(env, &res))
    }
    return res;
}

/*
    Enumerate the channels of the given device.

    Parameters
        dev: A pointer to an iio_device structure
    Returns
        The number of channels found
*/
METHOD(device_get_channels_count) {
    ASSERT_ARGC(1)
    struct iio_device *dev;
    ASSERT_EXTERNAL(args[0], dev)

    uint32_t count = iio_device_get_channels_count(dev);

    napi_value res;
    ASSERT(res, napi_create_uint32(env, count, &res))
    return res;
}

/*
    Get the channel present at the given index.

    Parameters
         dev: A pointer to an iio_device structure
         index: The index corresponding to the channel
    Returns
         On success, a pointer to an iio_channel structure
         If the index is invalid, NULL is returned
*/
METHOD(device_get_channel) {
    ASSERT_ARGC(2)
    struct iio_device *dev;
    ASSERT_EXTERNAL(args[0], dev)
    ASSERT_UINT(args[1], index)

    struct iio_channel *cha;
    cha = iio_device_get_channel(dev, index);

    napi_value res;
    if (cha) {
        ASSERT(res, napi_create_external(env, cha, 0, 0, &res))
    } else {
        ASSERT(res, napi_get_undefined(env, &res))
    }
    return res;
}

/*
    Try to find a channel structure by its name of ID.

    Parameters
        dev:    A pointer to an iio_device structure
        name:   A NULL-terminated string corresponding to the name or the ID
                of the channel to search for
        output: True if the searched channel is output, False otherwise
    Returns
        On success, a pointer to an iio_channel structure
        If the name or ID does not correspond to any known channel of the
        given device, NULL is returned
*/
METHOD(device_find_channel) {
    ASSERT_ARGC(3)
    struct iio_device *dev;
    ASSERT_EXTERNAL(args[0], dev)
    ASSERT_STRING(args[1], name)
    ASSERT_BOOL(args[2], is_output)

    struct iio_channel *cha;
    cha = iio_device_find_channel(dev, name, is_output);

    napi_value res;
    if (cha) {
        ASSERT(res, napi_create_external(env, cha, 0, 0, &res))
    } else {
        ASSERT(res, napi_get_undefined(env, &res))
    }
    return res;
}

/*
    Get the current sample size.

    Parameters
        dev:  A pointer to an iio_device structure
    Returns
        On success, the sample size in bytes
        On error, a negative errno code is returned
    NOTE: The sample size is not constant and will change when channels get
    enabled or disabled.
*/
METHOD(device_get_sample_size) {
    ASSERT_ARGC(1)
    struct iio_device *dev;
    ASSERT_EXTERNAL(args[0], dev)

    const int32_t size = iio_device_get_sample_size(dev);

    napi_value res;
    ASSERT(res, napi_create_int32(env, size, &res))
    return res;
}

/*
    Return True if the given channel is an output channel.

    Parameters
        chn:  A pointer to an iio_channel structure
    Returns
        True if the channel is an output channel, False otherwise
*/
METHOD(channel_is_output) {
    ASSERT_ARGC(1)
    struct iio_channel *cha;
    ASSERT_EXTERNAL(args[0], cha)

    bool flag = iio_channel_is_output(cha);

    napi_value result;
    ASSERT(result, napi_get_boolean(env, flag, &result))
    return result;
}

//// channel_get_id

/*
    Retrieve the channel ID (e.g. voltage0)

    Parameters
        chn:  A pointer to an iio_channel structure
    Returns
        A pointer to a static NULL-terminated string
*/
METHOD(channel_get_id) {
    ASSERT_ARGC(1)
    struct iio_channel *cha;
    ASSERT_EXTERNAL(args[0], cha)

    const char *res = iio_channel_get_id(cha);

    napi_value result;
    ASSERT(result, napi_create_string_utf8(env, res, NAPI_AUTO_LENGTH, &result))
    return result;
}

/*
    Retrieve the channel name (e.g. vccint)

    Parameters
        chn:    A pointer to an iio_channel structure
    Returns
        A pointer to a static NULL-terminated string
        NOTE: if the channel has no name, NULL is returned.
*/
METHOD(channel_get_name) {
    ASSERT_ARGC(1)
    struct iio_channel *cha;
    ASSERT_EXTERNAL(args[0], cha)

    const char *str = iio_channel_get_name(cha);

    napi_value res;
    if (str) {
        ASSERT(res, napi_create_string_utf8(env, str, NAPI_AUTO_LENGTH, &res))
    } else {
        ASSERT(res, napi_get_undefined(env, &res))
    }
    return res;
}

/*
    Returns True if the channel is enabled.

    Parameters
        chn:  A pointer to an iio_channel structure
    Returns
        True if the channel is enabled, False otherwise
*/
METHOD(channel_is_enabled) {
    ASSERT_ARGC(1)
    struct iio_channel *cha;
    ASSERT_EXTERNAL(args[0], cha)

    bool flag = iio_channel_is_enabled(cha);

    napi_value result;
    ASSERT(result, napi_get_boolean(env, flag, &result))
    return result;
}

/*
    Get the type of the given channel.

    Parameters
        chn:  A pointer to an iio_channel structure
    Returns
        The type of the channel
*/
METHOD(channel_get_type) {
    ASSERT_ARGC(1)
    struct iio_channel *cha;
    ASSERT_EXTERNAL(args[0], cha)

    uint32_t type = iio_channel_get_type(cha);

    napi_value res;
    ASSERT(res, napi_create_uint32(env, type, &res))
    return res;
}

/*
    Disable the given channel.

    Parameters
        chn:  A pointer to an iio_channel structure
*/
METHOD(channel_disable) {
    ASSERT_ARGC(1)
    struct iio_channel *cha;
    ASSERT_EXTERNAL(args[0], cha)

    iio_channel_disable(cha);

    napi_value res;
    ASSERT(res, napi_get_undefined(env, &res))
    return res;
}

/*
    Enable the given channel.

    Parameters
        chn:  A pointer to an iio_channel structure

    NOTE:Before creating an iio_buffer structure with iio_device_create_buffer,
    it is required to enable at least one channel of the device to read from.
*/
METHOD(channel_enable) {
    ASSERT_ARGC(1)
    struct iio_channel *cha;
    ASSERT_EXTERNAL(args[0], cha)

    iio_channel_enable(cha);

    napi_value res;
    ASSERT(res, napi_get_undefined(env, &res))
    return res;
}

/*
    Enumerate the channel-specific attributes of the given channel.

    Parameters
        chn:  A pointer to an iio_channel structure
    Returns
        The number of channel-specific attributes found
*/
METHOD(channel_get_attrs_count) {
    ASSERT_ARGC(1)
    struct iio_channel *cha;
    ASSERT_EXTERNAL(args[0], cha)

    const uint32_t count = iio_channel_get_attrs_count(cha);

    napi_value res;
    ASSERT(res, napi_create_uint32(env, count, &res))
    return res;
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
METHOD(channel_get_attr) {
    ASSERT_ARGC(2)
    struct iio_channel *cha;
    ASSERT_EXTERNAL(args[0], cha)
    ASSERT_UINT(args[1], index)

    const char *str = iio_channel_get_attr(cha, index);

    napi_value res;
    if (str) {
        ASSERT(res, napi_create_string_utf8(env, str, NAPI_AUTO_LENGTH, &res))
    } else {
        ASSERT(res, napi_get_undefined(env, &res))
    }
    return res;
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

METHOD(channel_attr_read) {
    ASSERT_ARGC(2)
    struct iio_channel *cha;
    ASSERT_EXTERNAL(args[0], cha)
    ASSERT_STRING(args[1], attr)

    uint32_t len1 = 1024;
    char* dst = malloc(len1);
    int32_t len2 = iio_channel_attr_read(cha, attr, dst, len1);

    napi_value res;
    if (len2 > 0) {
        ASSERT(res, napi_create_string_utf8(env, dst, NAPI_AUTO_LENGTH, &res))
    } else {
        ASSERT(res, napi_get_undefined(env, &res))
    }
    return res;
}

//// channel_attr_write_raw

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
METHOD(channel_attr_write) {
    ASSERT_ARGC(3)
    struct iio_channel *cha;
    ASSERT_EXTERNAL(args[0], cha)
    ASSERT_STRING(args[1], attr)
    ASSERT_STRING(args[2], src)

    const int32_t len = iio_channel_attr_write(cha, attr, src);

    napi_value res;
    ASSERT(res, napi_create_int32(env, len, &res))
    return res;
}

/*
    Create an input or output buffer associated to the given device.

    Parameters
        dev:        A pointer to an iio_device structure
        samples_count:  The number of samples that the buffer should contain
        cyclic:     If True, enable cyclic mode
    Returns
        On success, a pointer to an iio_buffer structure
        On error, NULL is returned, and errno is set to the error code

    NOTE: Channels that have to be written to / read from must be enabled
    before creating the buffer.
*/
METHOD(device_create_buffer) {
    ASSERT_ARGC(3)
    struct iio_device *dev;
    ASSERT_EXTERNAL(args[0], dev)
    ASSERT_UINT(args[1], samples_count)
    ASSERT_BOOL(args[2], cyclic)

    struct iio_buffer *buf;
    buf = iio_device_create_buffer(dev, samples_count, cyclic);

    napi_value res;
    if (buf) {
        ASSERT(res, napi_create_external(env, buf, 0, 0, &res))
    } else {
        ASSERT(res, napi_get_undefined(env, &res))
    }
    return res;
}

//// buffer_refill
//// buffer_first
//// buffer_step
//// buffer_start
//// buffer_end
//// buffer_foreach_sample

struct dev_read_cxt {
    void *data;
    uint32_t max_len;
    uint32_t len;
    napi_env *env;
    napi_async_context *async_context;
    napi_value *resource_object;
    napi_value *fn;
    struct iio_buffer *buf;
};

ssize_t dev_read_cb(const struct iio_channel *chn, void *src, size_t bytes, void *cxt) {
    struct dev_read_cxt *cxtt = (struct dev_read_cxt *)cxt;
    uint32_t len = cxtt->len;
    if (len == 0) {
        cxtt->data = src;
    }
    len += bytes;
    if (len < cxtt->max_len) {
        cxtt->len = len;
        return 0;
    }
    /* Use "src" to read or write a sample for this channel */
    napi_env env = *(cxtt->env);
    napi_async_context async_context = *(cxtt->async_context);
    napi_value resource_object = *(cxtt->resource_object);
    napi_value fn = *(cxtt->fn);
    napi_value res;
    ASSERT(fn, napi_make_callback(env,
        async_context,
        resource_object,
        fn,
        0,
        NULL,
        &res
    ))
    // printf("%ld\n", iio_buffer_refill(cxtt->buf));
    cxtt->len = 0;
    return 0;
}

METHOD(buf_read) {
    ASSERT_ARGC(3)
    struct iio_buffer *buf;
    ASSERT_EXTERNAL(args[0], buf)
    ASSERT_UINT(args[1], max_len)
    ASSERT_FUNCTION(args[2], fn)

    struct dev_read_cxt *cxt = malloc(sizeof *cxt);
    cxt->max_len = max_len;
    cxt->fn = &fn;
    cxt->env = &env;
    cxt->buf = buf;

    napi_value async_resource_name;
    ASSERT(async_resource_name, napi_create_string_utf8(env,
        "async_resource_name",
        NAPI_AUTO_LENGTH,
        &async_resource_name
    ))

    napi_async_context async_context;
    ASSERT(async_resource_name, napi_async_init(env,
        NULL,
        async_resource_name,
        &async_context
    ))
    cxt->async_context = &async_context;

    napi_value resource_object;
    ASSERT(resource_object, napi_create_object(env, &resource_object))
    cxt->resource_object = &resource_object;

    printf("A\n");

    const ssize_t count = iio_buffer_foreach_sample(buf, dev_read_cb, (void *)cxt);

    iio_buffer_refill(buf);

    napi_value res;
    ASSERT(res, napi_create_uint32(env, count, &res))
    return res;
}

napi_value Init(napi_env env, napi_value exports) {
    DECLARE_NAPI_METHOD("library_get_version", library_get_version)

    DECLARE_NAPI_METHOD("get_backends_count", get_backends_count)
    DECLARE_NAPI_METHOD("get_backend", get_backend)

    DECLARE_NAPI_METHOD("create_scan_context", create_scan_context)
    DECLARE_NAPI_METHOD("scan_context_get_info_list", scan_context_get_info_list)
    DECLARE_NAPI_METHOD("context_info_get_description", context_info_get_description)
    DECLARE_NAPI_METHOD("context_info_get_uri", context_info_get_uri)

    DECLARE_NAPI_METHOD("create_context_from_uri", create_context_from_uri)

    DECLARE_NAPI_METHOD("context_get_devices_count", context_get_devices_count)
    DECLARE_NAPI_METHOD("context_get_device", context_get_device)

    DECLARE_NAPI_METHOD("device_get_channels_count", device_get_channels_count)
    DECLARE_NAPI_METHOD("device_get_id", device_get_id)
    DECLARE_NAPI_METHOD("device_get_name", device_get_name)
    DECLARE_NAPI_METHOD("device_get_attrs_count", device_get_attrs_count)
    DECLARE_NAPI_METHOD("device_get_attr", device_get_attr)
    DECLARE_NAPI_METHOD("device_attr_read", device_attr_read)

    DECLARE_NAPI_METHOD("device_get_channel", device_get_channel)

    DECLARE_NAPI_METHOD("device_find_channel", device_find_channel)
    DECLARE_NAPI_METHOD("device_get_sample_size", device_get_sample_size)
    DECLARE_NAPI_METHOD("channel_is_output", channel_is_output)
    DECLARE_NAPI_METHOD("channel_get_id", channel_get_id)
    DECLARE_NAPI_METHOD("channel_get_name", channel_get_name)
    DECLARE_NAPI_METHOD("channel_is_enabled", channel_is_enabled)
    DECLARE_NAPI_METHOD("channel_get_type", channel_get_type)
    DECLARE_NAPI_METHOD("channel_disable", channel_disable)
    DECLARE_NAPI_METHOD("channel_enable", channel_enable)
    DECLARE_NAPI_METHOD("channel_get_attrs_count", channel_get_attrs_count)
    DECLARE_NAPI_METHOD("channel_get_attr", channel_get_attr)
    DECLARE_NAPI_METHOD("channel_attr_read", channel_attr_read)
    // DECLARE_NAPI_METHOD("channel_attr_write_raw", channel_attr_write_raw)
    DECLARE_NAPI_METHOD("channel_attr_write", channel_attr_write)

    DECLARE_NAPI_METHOD("device_create_buffer", device_create_buffer)
    // DECLARE_NAPI_METHOD("buffer_refill", buffer_refill)
    // DECLARE_NAPI_METHOD("buffer_first", buffer_first)
    // DECLARE_NAPI_METHOD("buffer_step", buffer_step)
    // DECLARE_NAPI_METHOD("buffer_start", buffer_start)
    // DECLARE_NAPI_METHOD("buffer_end", buffer_end)
    // DECLARE_NAPI_METHOD("buffer_foreach_sample", buffer_foreach_sample)

    DECLARE_NAPI_METHOD("buf_read", buf_read)

    return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)
