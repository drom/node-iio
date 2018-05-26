#ifndef __LIBIIO_MACROS_H
#define __LIBIIO_MACROS_H

#define CDATA(buf) (char *) node::Buffer::Data(buf)
#define LOCAL_STRING(str) Nan::New<String>(str).ToLocalChecked()
#define LOCAL_FUNCTION(fn) Nan::GetFunction(Nan::New<FunctionTemplate>(fn)).ToLocalChecked()
#define EXPORT_NUMBER(name) Nan::Set(target, LOCAL_STRING(#name), Nan::New<Number>(name));
#define EXPORT_FUNCTION(name) Nan::Set(target, LOCAL_STRING(#name), LOCAL_FUNCTION(name));

#define CALL_LIBIIO_INT(fn) \
    int ret = fn; \
    info.GetReturnValue().Set(Nan::New(ret));

#define CALL_LIBIIO_UINT(fn) \
    uint32_t ret = fn; \
    info.GetReturnValue().Set(Nan::New(ret));

#define CALL_LIBIIO_BOOL(fn) \
    bool ret = fn; \
    info.GetReturnValue().Set(Nan::New(ret));

#define CALL_LIBIIO_CONST_CHAR(fn) \
    const char *ret = fn; \
    info.GetReturnValue().Set(Nan::New(ret).ToLocalChecked());

#define CALL_LIBIIO_CONST_CHAR_OR_NULL(fn) \
    const char *ret = fn; \
    if (ret == NULL) { \
        return; \
    } \
    info.GetReturnValue().Set(Nan::New(ret).ToLocalChecked());

#define ASSERT_UINT(name, var) \
    if (!name->IsNumber()) { \
        Nan::ThrowError(#var " must be a number"); \
        return; \
    } \
    uint32_t var = name->IntegerValue();

#define ASSERT_BUFFER(name, var) \
    if (!name->IsObject()) { \
        Nan::ThrowError(#var " must be a buffer"); \
        return; \
    } \
    Local<Object> var = name->ToObject();

#define ASSERT_OBJECT(name, var) \
    if (!name->IsObject()) { \
        Nan::ThrowError(#var " must be an object"); \
        return; \
    } \
    Local<Object> var = name->ToObject();

#define ASSERT_SCAN_CXT(name, var) \
    if (!name->IsObject()) { \
        Nan::ThrowError(#var " must be an object"); \
        return; \
    } \
    Local<Object> jscxt = name->ToObject(); \
    iio_scan_context *var = (struct iio_scan_context)iioScanContext::Resolve(jscxt);

#define ASSERT_CXT(name, var) \
    if (!name->IsObject()) { \
        Nan::ThrowError(#var " must be an object"); \
        return; \
    } \
    Local<Object> jscxt = name->ToObject(); \
    iio_context *var = (struct iio_context)iioContext::Resolve(jscxt);

#define ASSERT_CXT_INFO(name, var) \
    if (!name->IsObject()) { \
        Nan::ThrowError(#var " must be an object"); \
        return; \
    } \
    Local<Object> jscxt = name->ToObject(); \
    iio_context_info *var = (struct iio_context_info)iioContextInfo::Resolve(jscxt);

#endif
