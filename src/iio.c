#include <iio.h>
#include <node_api.h>
#include <assert.h>

#define DECLARE_NAPI_METHOD(name, func) \
    napi_status func ## _status; \
    napi_property_descriptor func ## _desc = { name, 0, func, 0, 0, 0, napi_default, 0 }; \
    func ## _status = napi_define_properties(env, exports, 1, &func ## _desc); \
    assert(func ## _status == napi_ok); \

#define METHOD(name) \
    napi_value name(napi_env env, napi_callback_info info)

#define ASSERT_ARGC(count) \
    size_t argc = count; \
    napi_value args[count]; \
    napi_status argc_status; \
    argc_status = napi_get_cb_info(env, info, &argc, args, 0, 0); \
    assert(argc_status == napi_ok); \
    if (argc < count) { \
        napi_throw_type_error(env, 0, "Wrong number of arguments"); \
        return 0; \
    }

#define ASSERT_UINT(name, var) \
    napi_status var ## _status; \
    napi_valuetype var ## _valuetype; \
    var ## _status = napi_typeof(env, name, &var ## _valuetype); \
    assert(var ## _status == napi_ok); \
    if (var ## _valuetype != napi_number) { \
        napi_throw_type_error(env, 0, "Wrong arguments"); \
        return 0; \
    } \
    uint32_t var; \
    var ## _status = napi_get_value_uint32(env, name, &var); \
    assert(var ## _status == napi_ok);


#define ASSERT_STRING(name, var) \
    napi_status var ## _status; \
    napi_valuetype var ## _valuetype; \
    var ## _status = napi_typeof(env, name, &var ## _valuetype); \
    assert(var ## _status == napi_ok); \
    if (var ## _valuetype != napi_string) { \
        napi_throw_type_error(env, 0, "Wrong arguments"); \
        return 0; \
    } \
    char var[256]; \
    size_t var ## _result; \
    var ## _status = napi_get_value_string_latin1(env, name, var, 256, &var ## _result); \
    assert(var ## _status == napi_ok);

#define ASSERT_OBJECT(name, var) \
    napi_value var; \
    napi_valuetype var ## _valuetype; \
    \
    napi_status var ## _status; \
    var ## _status = napi_typeof(env, name, &var ## _valuetype); \
    assert(var ## _status == napi_ok); \
    \
    if (var ## _valuetype != napi_object) { \
        napi_throw_type_error(env, 0, "Wrong arguments"); \
        return 0; \
    } \


#define ASSERT_EXTERNAL(name, var) \
    napi_valuetype var ## _valuetype; \
    \
    napi_status var ## _status; \
    var ## _status = napi_typeof(env, name, &var ## _valuetype); \
    assert(var ## _status == napi_ok); \
    \
    if (var ## _valuetype != napi_external) { \
        napi_throw_type_error(env, 0, "Wrong arguments"); \
        return 0; \
    } \
    var ## _status = napi_get_value_external(env, name, &cxt); \
    assert(var ## _status == napi_ok);


//// library_get_version

/*
    Get the number of available backends.
        Returns:
            The number of available backends
*/
METHOD(get_backends_count) {
    uint32_t value = iio_get_backends_count();

    napi_value result;
    assert(napi_create_uint32(env, value, &result) == napi_ok);
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

    const char *res = iio_get_backend(index);

    napi_value result;
    assert(napi_create_string_utf8(env, res, NAPI_AUTO_LENGTH, &result) == napi_ok);
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

    napi_value obj;
    assert(napi_create_object(env, &obj) == napi_ok);
    assert(napi_wrap(env, obj, (void *)cxt, 0, 0, 0) == napi_ok);
    return obj;
}


//// scan_context_get_info_list
//// context_info_get_description_index
//// context_info_get_uri_index
//// context_info_count

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

    struct iio_context *cxt = malloc(1024);
    cxt = iio_create_context_from_uri(uri);

    napi_value res;
    if (cxt) {
        assert(napi_create_external(env, cxt, 0, 0, &res) == napi_ok);
    } else {
        assert(napi_get_undefined(env, &res) == napi_ok);
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
    assert(napi_create_uint32(env, value, &res) == napi_ok);
    return res;
}


//// context_get_device
//// context_find_device
//// device_get_id
//// device_get_name
//// device_get_attrs_count
//// device_get_attr
//// device_attr_read
//// device_get_channels_count
//// device_get_channel
//// device_find_channel
//// device_get_sample_size
//// channel_is_output
//// channel_get_id
//// channel_get_name
//// channel_is_enabled
//// channel_get_type
//// channel_disable
//// channel_enable
//// channel_get_attrs_count
//// channel_get_attr
//// channel_attr_read
//// channel_attr_write_raw
//// channel_attr_write
//// device_create_buffer
//// buffer_refill
//// buffer_first
//// buffer_step
//// buffer_start
//// buffer_end
//// buffer_foreach_sample

napi_value Init(napi_env env, napi_value exports) {
  DECLARE_NAPI_METHOD("get_backends_count", get_backends_count)
  DECLARE_NAPI_METHOD("get_backend", get_backend)
  DECLARE_NAPI_METHOD("create_scan_context", create_scan_context)
  DECLARE_NAPI_METHOD("create_context_from_uri", create_context_from_uri)
  DECLARE_NAPI_METHOD("context_get_devices_count", context_get_devices_count)
  return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)
