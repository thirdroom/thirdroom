#include <emscripten.h>
#include <emscripten/console.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>

#include "../../include/quickjs/cutils.h"
#include "../../include/quickjs/quickjs.h"

#include "../jsutils.h"
#include "../websg-utils.h"
#include "../script-context.h"
#include "websg.h"
#include "buffer-view.h"
#include "buffer.h"

/**
 * WebSG.BufferView
 */

static JSValue js_buffer_view_constructor(JSContext *ctx, JSValueConst new_target, int argc, JSValueConst *argv) {
  BufferView *buffer_view = js_mallocz(ctx, sizeof(BufferView));

  

  if (websg_create_resource(ResourceType_BufferView, buffer_view)) {
    return JS_EXCEPTION;
  }

  JSValue proto = JS_GetPropertyStr(ctx, new_target, "prototype");

  if (JS_IsException(proto)) {
    websg_dispose_resource(buffer_view);
    JS_FreeValue(ctx, proto);
    return JS_EXCEPTION;
  }

  JSValue val = JS_NewObjectProtoClass(ctx, proto, js_buffer_view_class_id);
  JS_FreeValue(ctx, proto);

  if (JS_IsException(val)) {
    websg_dispose_resource(buffer_view);
    JS_FreeValue(ctx, val);
    return JS_EXCEPTION;
  }

  

  JS_SetOpaque(val, buffer_view);
  set_js_val_from_ptr(ctx, buffer_view, val);

  return val;
}


static JSValue js_buffer_view_get_name(JSContext *ctx, JSValueConst this_val) {
  BufferView *buffer_view = JS_GetOpaque2(ctx, this_val, js_buffer_view_class_id);

  if (!buffer_view) {
    return JS_EXCEPTION;
  } else {
    JSValue val;
    val = JS_NewString(ctx, buffer_view->name);
    return val;
  }
}


static JSValue js_buffer_view_set_name(JSContext *ctx, JSValueConst this_val, JSValue val) {
  BufferView *buffer_view = JS_GetOpaque2(ctx, this_val, js_buffer_view_class_id);

  if (!buffer_view) {
    return JS_EXCEPTION;
  } else {
    buffer_view->name = JS_ToCString(ctx, val);
    return JS_UNDEFINED;
  }
}


static JSValue js_buffer_view_get_buffer(JSContext *ctx, JSValueConst this_val) {
  BufferView *buffer_view = JS_GetOpaque2(ctx, this_val, js_buffer_view_class_id);

  if (!buffer_view) {
    return JS_EXCEPTION;
  } else {
    JSValue val;
    val = create_buffer_from_ptr(ctx, buffer_view->buffer);
    return val;
  }
}


static JSValue js_buffer_view_get_byte_offset(JSContext *ctx, JSValueConst this_val) {
  BufferView *buffer_view = JS_GetOpaque2(ctx, this_val, js_buffer_view_class_id);

  if (!buffer_view) {
    return JS_EXCEPTION;
  } else {
    JSValue val;
    val = JS_NewUint32(ctx, buffer_view->byte_offset);
    return val;
  }
}


static JSValue js_buffer_view_get_byte_length(JSContext *ctx, JSValueConst this_val) {
  BufferView *buffer_view = JS_GetOpaque2(ctx, this_val, js_buffer_view_class_id);

  if (!buffer_view) {
    return JS_EXCEPTION;
  } else {
    JSValue val;
    val = JS_NewUint32(ctx, buffer_view->byte_length);
    return val;
  }
}


static JSValue js_buffer_view_get_byte_stride(JSContext *ctx, JSValueConst this_val) {
  BufferView *buffer_view = JS_GetOpaque2(ctx, this_val, js_buffer_view_class_id);

  if (!buffer_view) {
    return JS_EXCEPTION;
  } else {
    JSValue val;
    val = JS_NewUint32(ctx, buffer_view->byte_stride);
    return val;
  }
}


static JSValue js_buffer_view_get_target(JSContext *ctx, JSValueConst this_val) {
  BufferView *buffer_view = JS_GetOpaque2(ctx, this_val, js_buffer_view_class_id);

  if (!buffer_view) {
    return JS_EXCEPTION;
  } else {
    JSValue val;
    val = JS_NewUint32(ctx, buffer_view->target);
    return val;
  }
}




static void js_buffer_view_finalizer(JSRuntime *rt, JSValue val) {
  BufferView *buffer_view = JS_GetOpaque(val, js_buffer_view_class_id);
  websg_dispose_resource(buffer_view);
  js_free_rt(rt, buffer_view);
}

static JSClassDef js_buffer_view_class = {
  "BufferView",
  .finalizer = js_buffer_view_finalizer
};

static const JSCFunctionListEntry js_buffer_view_proto_funcs[] = {
  JS_CGETSET_DEF("name", js_buffer_view_get_name, js_buffer_view_set_name),
  JS_CGETSET_DEF("buffer", js_buffer_view_get_buffer, NULL),
  JS_CGETSET_DEF("byteOffset", js_buffer_view_get_byte_offset, NULL),
  JS_CGETSET_DEF("byteLength", js_buffer_view_get_byte_length, NULL),
  JS_CGETSET_DEF("byteStride", js_buffer_view_get_byte_stride, NULL),
  JS_CGETSET_DEF("target", js_buffer_view_get_target, NULL),
  JS_PROP_STRING_DEF("[Symbol.toStringTag]", "BufferView", JS_PROP_CONFIGURABLE),
};

static JSValue js_define_buffer_view_class(JSContext *ctx) {
  JSRuntime *rt = JS_GetRuntime(ctx);

  JS_NewClassID(&js_buffer_view_class_id);
  JS_NewClass(rt, js_buffer_view_class_id, &js_buffer_view_class);

  JSValue buffer_view_proto = JS_NewObject(ctx);
  JS_SetPropertyFunctionList(ctx, buffer_view_proto, js_buffer_view_proto_funcs, countof(js_buffer_view_proto_funcs));
  
  JSValue buffer_view_class = JS_NewCFunction2(ctx, js_buffer_view_constructor, "BufferView", 1, JS_CFUNC_constructor, 1);
  JS_SetConstructor(ctx, buffer_view_class, buffer_view_proto);
  JS_SetClassProto(ctx, js_buffer_view_class_id, buffer_view_proto);

  return buffer_view_class;
}

/**
 * WebSG.BufferView related functions
*/

static JSValue js_get_buffer_view_by_name(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  const char *name = JS_ToCString(ctx, argv[0]);
  BufferView *buffer_view = websg_get_resource_by_name(ResourceType_BufferView, name);
  JS_FreeCString(ctx, name);
  return create_buffer_view_from_ptr(ctx, buffer_view);
}

JSValue create_buffer_view_from_ptr(JSContext *ctx, BufferView *buffer_view) {
  if (!buffer_view) {
    return JS_UNDEFINED;
  }

  JSValue val = get_js_val_from_ptr(ctx, buffer_view);

  if (JS_IsUndefined(val)) {
    val = JS_NewObjectClass(ctx, js_buffer_view_class_id);
    
    JS_SetOpaque(val, buffer_view);
    set_js_val_from_ptr(ctx, buffer_view, val);
  }

  return val;
}

void js_define_buffer_view_api(JSContext *ctx, JSValue *target) {
  JS_SetPropertyStr(ctx, *target, "BufferView", js_define_buffer_view_class(ctx));
  JS_SetPropertyStr(
    ctx,
    *target,
    "getBufferViewByName",
    JS_NewCFunction(ctx, js_get_buffer_view_by_name, "getBufferViewByName", 1)
  );
}