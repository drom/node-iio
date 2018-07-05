#include <iio.h>
#include <node_api.h>
#include <assert.h>

#define DECLARE_NAPI_METHOD(name, func) \
  { name, 0, func, 0, 0, 0, napi_default, 0 }

//// library_get_version

// Get the number of available backends.
// Returns:
//      The number of available backends
napi_value get_backends_count(napi_env env, napi_callback_info info) {
  napi_status status;
  napi_value result;
  uint32_t value = iio_get_backends_count();
  status = napi_create_uint32(env, value, &result);
  assert(status == napi_ok);
  return result;
}


// Retrieve the name of a given backend.
// Parameters:
//      index: The index corresponding to the attribute
// Returns:
//      On success, a pointer to a static NULL-terminated string
//      If the index is invalid, NULL is returned
napi_value get_backend(napi_env env, napi_callback_info info) {
  napi_status status;

  size_t argc = 1;

  napi_value args[1];
  status = napi_get_cb_info(env, info, &argc, args, 0, 0);
  assert(status == napi_ok);

  if (argc < 1) {
    napi_throw_type_error(env, 0, "Wrong number of arguments");
    return 0;
  }

  napi_valuetype valuetype0;
  status = napi_typeof(env, args[0], &valuetype0);
  assert(status == napi_ok);

  if (valuetype0 != napi_number) {
    napi_throw_type_error(env, 0, "Wrong arguments");
    return 0;
  }

  uint32_t value0;
  status = napi_get_value_uint32(env, args[0], &value0);
  assert(status == napi_ok);

  napi_value result;
  status = napi_create_string_utf8(env, iio_get_backend(value0), NAPI_AUTO_LENGTH, &result);
  assert(status == napi_ok);
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
// NAN_METHOD(create_scan_context) {
//     CDATA_OR_NULL(info[0], uri)
//     ASSERT_UINT(info[1], flags)
//     iio_scan_context *cxt;
//     cxt = iio_create_scan_context(uri, flags);
//     Local<Object> jscxt = iioScanContext::New(cxt);
//     info.GetReturnValue().Set(jscxt);
// }


//// scan_context_get_info_list
//// context_info_get_description_index
//// context_info_get_uri_index
//// context_info_count
//// create_context_from_uri
//// context_get_devices_count
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
  napi_status status;

  napi_property_descriptor desc0 = DECLARE_NAPI_METHOD("get_backends_count", get_backends_count);
  status = napi_define_properties(env, exports, 1, &desc0);
  assert(status == napi_ok);

  napi_property_descriptor desc1 = DECLARE_NAPI_METHOD("get_backend", get_backend);
  status = napi_define_properties(env, exports, 1, &desc1);
  assert(status == napi_ok);

  return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)
