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
#include "mesh.h"
#include "mesh-primitive.h"

/**
 * WebSG.Mesh
 */

static JSValue js_mesh_constructor(JSContext *ctx, JSValueConst new_target, int argc, JSValueConst *argv) {
  Mesh *mesh = js_mallocz(ctx, sizeof(Mesh));

  

  if (websg_create_resource(ResourceType_Mesh, mesh)) {
    return JS_EXCEPTION;
  }

  JSValue proto = JS_GetPropertyStr(ctx, new_target, "prototype");

  if (JS_IsException(proto)) {
    websg_dispose_resource(mesh);
    JS_FreeValue(ctx, proto);
    return JS_EXCEPTION;
  }

  JSValue val = JS_NewObjectProtoClass(ctx, proto, js_mesh_class_id);
  JS_FreeValue(ctx, proto);

  if (JS_IsException(val)) {
    websg_dispose_resource(mesh);
    JS_FreeValue(ctx, val);
    return JS_EXCEPTION;
  }

  

  JS_SetOpaque(val, mesh);
  set_js_val_from_ptr(ctx, mesh, val);

  return val;
}


static JSValue js_mesh_get_name(JSContext *ctx, JSValueConst this_val) {
  Mesh *mesh = JS_GetOpaque2(ctx, this_val, js_mesh_class_id);

  if (!mesh) {
    return JS_EXCEPTION;
  } else {
    JSValue val;
    val = JS_NewString(ctx, mesh->name);
    return val;
  }
}


static JSValue js_mesh_set_name(JSContext *ctx, JSValueConst this_val, JSValue val) {
  Mesh *mesh = JS_GetOpaque2(ctx, this_val, js_mesh_class_id);

  if (!mesh) {
    return JS_EXCEPTION;
  } else {
    mesh->name = JS_ToCString(ctx, val);
    return JS_UNDEFINED;
  }
}

static JSValue js_mesh_primitives(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  Mesh *mesh = JS_GetOpaque2(ctx, this_val, js_mesh_class_id);

  if (!mesh) {
    return JS_EXCEPTION;
  } else {
    return JS_NewRefArrayIterator(ctx, (JSValue (*)(JSContext *ctx, void *res))&create_mesh_primitive_from_ptr, (void **)mesh->primitives, countof(mesh->primitives));
  }
}

static JSValue js_mesh_add_primitive(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  Mesh *mesh = JS_GetOpaque2(ctx, this_val, js_mesh_class_id);

  if (!mesh) {
    return JS_EXCEPTION;
  } else {
    return JS_AddRefArrayItem(ctx, js_mesh_primitive_class_id, (void **)mesh->primitives, countof(mesh->primitives), argv[0]);
  }
}

static JSValue js_mesh_remove_primitive(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  Mesh *mesh = JS_GetOpaque2(ctx, this_val, js_mesh_class_id);

  if (!mesh) {
    return JS_EXCEPTION;
  } else {
    return JS_RemoveRefArrayItem(ctx, js_mesh_primitive_class_id, (void **)mesh->primitives, countof(mesh->primitives), argv[0]);
  }
}



static void js_mesh_finalizer(JSRuntime *rt, JSValue val) {
  Mesh *mesh = JS_GetOpaque(val, js_mesh_class_id);
  websg_dispose_resource(mesh);
  js_free_rt(rt, mesh);
}

static JSClassDef js_mesh_class = {
  "Mesh",
  .finalizer = js_mesh_finalizer
};

static const JSCFunctionListEntry js_mesh_proto_funcs[] = {
  JS_CGETSET_DEF("name", js_mesh_get_name, js_mesh_set_name),
  JS_CFUNC_DEF("primitives", 0, js_mesh_primitives),
  JS_CFUNC_DEF("addPrimitive", 1, js_mesh_add_primitive),
  JS_CFUNC_DEF("removePrimitive", 1, js_mesh_remove_primitive),
  JS_PROP_STRING_DEF("[Symbol.toStringTag]", "Mesh", JS_PROP_CONFIGURABLE),
};

static JSValue js_define_mesh_class(JSContext *ctx) {
  JSRuntime *rt = JS_GetRuntime(ctx);

  JS_NewClassID(&js_mesh_class_id);
  JS_NewClass(rt, js_mesh_class_id, &js_mesh_class);

  JSValue mesh_proto = JS_NewObject(ctx);
  JS_SetPropertyFunctionList(ctx, mesh_proto, js_mesh_proto_funcs, countof(js_mesh_proto_funcs));
  
  JSValue mesh_class = JS_NewCFunction2(ctx, js_mesh_constructor, "Mesh", 1, JS_CFUNC_constructor, 1);
  JS_SetConstructor(ctx, mesh_class, mesh_proto);
  JS_SetClassProto(ctx, js_mesh_class_id, mesh_proto);

  return mesh_class;
}

/**
 * WebSG.Mesh related functions
*/

static JSValue js_get_mesh_by_name(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  const char *name = JS_ToCString(ctx, argv[0]);
  Mesh *mesh = websg_get_resource_by_name(ResourceType_Mesh, name);
  JS_FreeCString(ctx, name);
  return create_mesh_from_ptr(ctx, mesh);
}

JSValue create_mesh_from_ptr(JSContext *ctx, Mesh *mesh) {
  if (!mesh) {
    return JS_UNDEFINED;
  }

  JSValue val = get_js_val_from_ptr(ctx, mesh);

  if (JS_IsUndefined(val)) {
    val = JS_NewObjectClass(ctx, js_mesh_class_id);
    
    JS_SetOpaque(val, mesh);
    set_js_val_from_ptr(ctx, mesh, val);
  }

  return val;
}

void js_define_mesh_api(JSContext *ctx, JSValue *target) {
  JS_SetPropertyStr(ctx, *target, "Mesh", js_define_mesh_class(ctx));
  JS_SetPropertyStr(
    ctx,
    *target,
    "getMeshByName",
    JS_NewCFunction(ctx, js_get_mesh_by_name, "getMeshByName", 1)
  );
}