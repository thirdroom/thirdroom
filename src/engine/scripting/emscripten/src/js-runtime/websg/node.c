#include <string.h>
#include "../quickjs/cutils.h"
#include "../quickjs/quickjs.h"
#include "../../websg.h"
#include "./node.h"
#include "./scene.h"
#include "./mesh.h"
#include "./light.h"
#include "./collider.h"
#include "./interactable.h"
#include "./physics-body.h"
#include "./node-iterator.h"
#include "./vector3.h"
#include "./quaternion.h"
#include "./matrix4.h"
#include "./ui-canvas.h"
#include "../utils/array.h"

JSClassID js_websg_node_class_id;

static void js_websg_node_finalizer(JSRuntime *rt, JSValue val) {
  WebSGNodeData *node_data = JS_GetOpaque(val, js_websg_node_class_id);

  if (node_data) {
    JS_FreeValueRT(rt, node_data->interactable);
    JS_FreeValueRT(rt, node_data->physics_body);
    js_free_rt(rt, node_data);
  }
}

static JSClassDef js_websg_node_class = {
  "Node",
  .finalizer = js_websg_node_finalizer
};

static JSValue js_websg_node_add_child(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  WebSGNodeData *node_data = JS_GetOpaque(this_val, js_websg_node_class_id);

  node_id_t node_id = node_data->node_id;

  WebSGNodeData *child_data = JS_GetOpaque2(ctx, argv[0], js_websg_node_class_id);

  if (child_data == NULL) {
    return JS_EXCEPTION;
  }

  if (websg_node_add_child(node_id, child_data->node_id) == -1) {
    JS_ThrowInternalError(ctx, "WebSG: Couldn't add child node.");
    return JS_EXCEPTION;
  }

  return this_val;
}

static JSValue js_websg_node_remove_child(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  WebSGNodeData *node_data = JS_GetOpaque(this_val, js_websg_node_class_id);

  node_id_t node_id = node_data->node_id;

  WebSGNodeData *child_data = JS_GetOpaque2(ctx, argv[0], js_websg_node_class_id);

  if (child_data == NULL) {
    return JS_EXCEPTION;
  }

  if (websg_node_remove_child(node_id, child_data->node_id) == -1) {
    JS_ThrowInternalError(ctx, "WebSG: Couldn't remove child node.");
    return JS_EXCEPTION;
  }

  return this_val;
}

static JSValue js_websg_node_get_child(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  WebSGNodeData *node_data = JS_GetOpaque(this_val, js_websg_node_class_id);

  uint32_t index;

  if (JS_ToUint32(ctx, &index, argv[0]) == -1) {
    return JS_EXCEPTION;
  }

  node_id_t child_id = websg_node_get_child(node_data->node_id, index);

  if (child_id == 0) {
    return JS_UNDEFINED;
  }

  return js_websg_get_node_by_id(ctx, node_data->world_data, child_id);
}


JSValue js_websg_node_children(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  WebSGNodeData *node_data = JS_GetOpaque(this_val, js_websg_node_class_id);

  int32_t count = websg_node_get_child_count(node_data->node_id);

  if (count == -1) {
    JS_ThrowInternalError(ctx, "WebSG: Error getting node child count.");
    return JS_EXCEPTION;
  }

  node_id_t *children = js_malloc(ctx, sizeof(node_id_t) * count);

  if (websg_node_get_children(node_data->node_id, children, count) == -1) {
    JS_ThrowInternalError(ctx, "WebSG: Error getting node children.");
    return JS_EXCEPTION;
  }

  return js_websg_create_node_iterator(ctx, node_data->world_data, children, count);
}

static JSValue js_websg_node_parent(JSContext *ctx, JSValueConst this_val) {
  WebSGNodeData *node_data = JS_GetOpaque(this_val, js_websg_node_class_id);

  node_id_t node_id = node_data->node_id;

  node_id_t parent_id = websg_node_get_parent(node_id);

  if (parent_id != 0) {
    return js_websg_get_node_by_id(ctx, node_data->world_data, parent_id);
  }

  scene_id_t parent_scene_id = websg_node_get_parent_scene(node_id);

  if (parent_scene_id != 0) {
    return js_websg_get_scene_by_id(ctx, node_data->world_data, parent_scene_id);
  }

  return JS_UNDEFINED;
}

static JSValue js_websg_node_get_is_static(JSContext *ctx, JSValueConst this_val) {
  WebSGNodeData *node_data = JS_GetOpaque(this_val, js_websg_node_class_id);
  uint32_t result = websg_node_get_is_static(node_data->node_id);
  return JS_NewBool(ctx, result);
}

static JSValue js_websg_node_set_is_static(JSContext *ctx, JSValueConst this_val, JSValueConst arg) {
  WebSGNodeData *node_data = JS_GetOpaque(this_val, js_websg_node_class_id);

  int32_t value = JS_ToBool(ctx, arg);

  if (value < 0) {
    return JS_EXCEPTION;
  }

  int32_t result = websg_node_set_is_static(node_data->node_id, value);

  if (result == -1) {
    JS_ThrowInternalError(ctx, "WebSG: Error setting isStatic.");
    return JS_EXCEPTION;
  }

  return JS_UNDEFINED;
}

static JSValue js_websg_node_get_visible(JSContext *ctx, JSValueConst this_val) {
  WebSGNodeData *node_data = JS_GetOpaque(this_val, js_websg_node_class_id);
  uint32_t result = websg_node_get_visible(node_data->node_id);
  return JS_NewBool(ctx, result);
}

static JSValue js_websg_node_set_visible(JSContext *ctx, JSValueConst this_val, JSValueConst arg) {
  WebSGNodeData *node_data = JS_GetOpaque(this_val, js_websg_node_class_id);

  int32_t value = JS_ToBool(ctx, arg);

  if (value < 0) {
    return JS_EXCEPTION;
  }

  int32_t result = websg_node_set_visible(node_data->node_id, value);

  if (result == -1) {
    JS_ThrowInternalError(ctx, "WebSG: Error setting visible.");
    return JS_EXCEPTION;
  }

  return JS_UNDEFINED;
}

static JSValue js_websg_node_get_mesh(JSContext *ctx, JSValueConst this_val) {
  WebSGNodeData *node_data = JS_GetOpaque(this_val, js_websg_node_class_id);
  mesh_id_t mesh_id = websg_node_get_mesh(node_data->node_id);
  return js_websg_get_mesh_by_id(ctx, node_data->world_data, mesh_id);
}

static JSValue js_websg_node_set_mesh(JSContext *ctx, JSValueConst this_val, JSValueConst arg) {
  WebSGNodeData *node_data = JS_GetOpaque(this_val, js_websg_node_class_id);

  WebSGMeshData *mesh_data = JS_GetOpaque2(ctx, arg, js_websg_mesh_class_id);

  if (mesh_data == NULL) {
    return JS_EXCEPTION;
  }

  if (websg_node_set_mesh(node_data->node_id, mesh_data->mesh_id) == -1) {
    JS_ThrowInternalError(ctx, "WebSG: Couldn't set mesh.");
    return JS_EXCEPTION;
  }

  return JS_UNDEFINED;
}

static JSValue js_websg_node_get_light(JSContext *ctx, JSValueConst this_val) {
  WebSGNodeData *node_data = JS_GetOpaque(this_val, js_websg_node_class_id);
  light_id_t light_id = websg_node_get_light(node_data->node_id);
  return js_websg_get_light_by_id(ctx, node_data->world_data, light_id);
}

static JSValue js_websg_node_set_light(JSContext *ctx, JSValueConst this_val, JSValueConst arg) {
  WebSGNodeData *node_data = JS_GetOpaque(this_val, js_websg_node_class_id);

  WebSGLightData *light_data = JS_GetOpaque2(ctx, arg, js_websg_light_class_id);

  if (light_data == NULL) {
    return JS_EXCEPTION;
  }

  if (websg_node_set_light(node_data->node_id, light_data->light_id) == -1) {
    JS_ThrowInternalError(ctx, "WebSG: Couldn't set light.");
    return JS_EXCEPTION;
  }

  return JS_UNDEFINED;
}

static JSValue js_websg_node_get_collider(JSContext *ctx, JSValueConst this_val) {
  WebSGNodeData *node_data = JS_GetOpaque(this_val, js_websg_node_class_id);
  collider_id_t collider_id = websg_node_get_collider(node_data->node_id);
  return js_websg_get_collider_by_id(ctx, node_data->world_data, collider_id);
}

static JSValue js_websg_node_set_collider(JSContext *ctx, JSValueConst this_val, JSValueConst arg) {
  WebSGNodeData *node_data = JS_GetOpaque(this_val, js_websg_node_class_id);

  WebSGColliderData *collider_data = JS_GetOpaque2(ctx, arg, js_websg_collider_class_id);

  if (collider_data == NULL) {
    return JS_EXCEPTION;
  }

  if (websg_node_set_collider(node_data->node_id, collider_data->collider_id) == -1) {
    JS_ThrowInternalError(ctx, "WebSG: Couldn't set collider.");
    return JS_EXCEPTION;
  }

  return JS_UNDEFINED;
}

static JSValue js_websg_node_get_ui_canvas(JSContext *ctx, JSValueConst this_val) {
  WebSGNodeData *node_data = JS_GetOpaque(this_val, js_websg_node_class_id);
  ui_canvas_id_t ui_canvas_id = websg_node_get_ui_canvas(node_data->node_id);
  return js_websg_get_ui_canvas_by_id(ctx, node_data->world_data, ui_canvas_id);
}


static JSValue js_websg_node_set_ui_canvas(JSContext *ctx, JSValueConst this_val, JSValueConst arg) {
  WebSGNodeData *node_data = JS_GetOpaque(this_val, js_websg_node_class_id);

  WebSGUICanvasData *ui_canvas_data = JS_GetOpaque2(ctx, arg, js_websg_ui_canvas_class_id);

  if (ui_canvas_data == NULL) {
    return JS_EXCEPTION;
  }

  if (websg_node_set_ui_canvas(node_data->node_id, ui_canvas_data->ui_canvas_id) == -1) {
    JS_ThrowInternalError(ctx, "WebSG: Couldn't set UICanvas.");
    return JS_EXCEPTION;
  }

  return JS_UNDEFINED;
}

static float_t js_websg_node_get_translation_element(uint32_t node_id, float_t *translation, int index) {
  return websg_node_get_translation_element(node_id, index);
}

static void js_websg_node_set_translation_element(uint32_t node_id, float_t *translation, int index, float_t value) {
  websg_node_set_translation_element(node_id, index, value);
}

static float_t js_websg_node_get_rotation_element(uint32_t node_id, float_t *rotation, int index) {
  return websg_node_get_rotation_element(node_id, index);
}

static void js_websg_node_set_rotation_element(uint32_t node_id, float_t *rotation, int index, float_t value) {
  websg_node_set_rotation_element(node_id, index, value);
}

static float_t js_websg_node_get_scale_element(uint32_t node_id, float_t *scale, int index) {
  return websg_node_get_scale_element(node_id, index);
}

static void js_websg_node_set_scale_element(uint32_t node_id, float_t *scale, int index, float_t value) {
  websg_node_set_scale_element(node_id, index, value);
}

static float_t js_websg_node_get_matrix_element(uint32_t node_id, float_t *matrix, int index) {
  return websg_node_get_matrix_element(node_id, index);
}

static void js_websg_node_set_matrix_element(uint32_t node_id, float_t *matrix, int index, float_t value) {
  websg_node_set_matrix_element(node_id, index, value);
}

static float_t js_websg_node_get_world_matrix_element(uint32_t node_id, float_t *world_matrix, int index) {
  return websg_node_get_world_matrix_element(node_id, index);
}

// Implement the addChild and removeChild methods
static const JSCFunctionListEntry js_websg_node_proto_funcs[] = {
  JS_CFUNC_DEF("addChild", 1, js_websg_node_add_child),
  JS_CFUNC_DEF("removeChild", 1, js_websg_node_remove_child),
  JS_CFUNC_DEF("getChild", 1, js_websg_node_get_child),
  JS_CFUNC_DEF("children", 0, js_websg_node_children),
  JS_CGETSET_DEF("parent", js_websg_node_parent, NULL),
  JS_CGETSET_DEF("isStatic", js_websg_node_get_is_static, js_websg_node_set_is_static),
  JS_CGETSET_DEF("visible", js_websg_node_get_is_static, js_websg_node_set_is_static),
  JS_CGETSET_DEF("mesh", js_websg_node_get_mesh, js_websg_node_set_mesh),
  JS_CGETSET_DEF("light", js_websg_node_get_light, js_websg_node_set_light),
  JS_CGETSET_DEF("collider", js_websg_node_get_collider, js_websg_node_set_collider),
  JS_CGETSET_DEF("uiCanvas", js_websg_node_get_ui_canvas, js_websg_node_set_ui_canvas),
  JS_CGETSET_DEF("interactable", js_websg_node_get_interactable, NULL),
  JS_CFUNC_DEF("addInteractable", 1, js_websg_node_add_interactable),
  JS_CFUNC_DEF("removeInteractable", 0, js_websg_node_remove_interactable),
  JS_CGETSET_DEF("physicsBody", js_websg_node_get_physics_body, NULL),
  JS_CFUNC_DEF("addPhysicsBody", 1, js_websg_node_add_physics_body),
  JS_CFUNC_DEF("removePhysicsBody", 0, js_websg_node_remove_physics_body),
  JS_PROP_STRING_DEF("[Symbol.toStringTag]", "WebSGNode", JS_PROP_CONFIGURABLE),
};

static JSValue js_websg_node_constructor(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  return JS_ThrowTypeError(ctx, "Illegal Constructor.");
}

void js_websg_define_node(JSContext *ctx, JSValue websg) {
  JS_NewClassID(&js_websg_node_class_id);
  JS_NewClass(JS_GetRuntime(ctx), js_websg_node_class_id, &js_websg_node_class);
  JSValue node_proto = JS_NewObject(ctx);
  JS_SetPropertyFunctionList(ctx, node_proto, js_websg_node_proto_funcs, countof(js_websg_node_proto_funcs));
  JS_SetClassProto(ctx, js_websg_node_class_id, node_proto);

  JSValue constructor = JS_NewCFunction2(
    ctx,
    js_websg_node_constructor,
    "Node",
    0,
    JS_CFUNC_constructor,
    0
  );
  JS_SetConstructor(ctx, constructor, node_proto);
  JS_SetPropertyStr(
    ctx,
    websg,
    "Node",
    constructor
  );
}

JSValue js_websg_new_node_instance(JSContext *ctx, WebSGWorldData *world_data, node_id_t node_id) {
  JSValue node = JS_NewObjectClass(ctx, js_websg_node_class_id);

  if (JS_IsException(node)) {
    return node;
  }

  js_websg_define_vector3_prop(
    ctx,
    node,
    "translation",
    node_id,
    &js_websg_node_get_translation_element,
    &js_websg_node_set_translation_element
  );

  js_websg_define_quaternion_prop(
    ctx,
    node,
    "rotation",
    node_id,
    &js_websg_node_get_rotation_element,
    &js_websg_node_set_rotation_element
  );

  js_websg_define_vector3_prop(
    ctx,
    node,
    "scale",
    node_id,
    &js_websg_node_get_scale_element,
    &js_websg_node_set_scale_element
  );

  js_websg_define_matrix4_prop(
    ctx,
    node,
    "matrix",
    node_id,
    &js_websg_node_get_matrix_element,
    &js_websg_node_set_matrix_element
  );

  js_websg_define_matrix4_prop_read_only(
    ctx,
    node,
    "worldMatrix",
    node_id,
    &js_websg_node_get_world_matrix_element
  );

  WebSGNodeData *node_data = js_mallocz(ctx, sizeof(WebSGNodeData));
  node_data->world_data = world_data;
  node_data->node_id = node_id;
  node_data->interactable = js_websg_init_node_interactable(ctx, node_id);
  node_data->physics_body = js_websg_init_node_physics_body(ctx, node_id);
  JS_SetOpaque(node, node_data);

  JS_SetPropertyUint32(ctx, world_data->nodes, node_id, JS_DupValue(ctx, node));

  return node;
}

/**
 * Public Methods
 **/

JSValue js_websg_get_node_by_id(JSContext *ctx, WebSGWorldData *world_data, node_id_t node_id) {
  JSValue node = JS_GetPropertyUint32(ctx, world_data->nodes, node_id);

  if (!JS_IsUndefined(node)) {
    return JS_DupValue(ctx, node);
  }

  return js_websg_new_node_instance(ctx, world_data, node_id);
}

/**
 * World Methods
 **/

JSValue js_websg_world_create_node(JSContext *ctx, JSValue this_val, int argc, JSValue *argv) {
  WebSGWorldData *world_data = JS_GetOpaque(this_val, js_websg_world_class_id);

  NodeProps *props = js_mallocz(ctx, sizeof(NodeProps));
  props->rotation[0] = 0.0f;
  props->rotation[1] = 0.0f;
  props->rotation[2] = 0.0f;
  props->rotation[3] = 1.0f;
  props->scale[0] = 1.0f;
  props->scale[1] = 1.0f;
  props->scale[2] = 1.0f;

  if (!JS_IsUndefined(argv[0])) {

    JSValue name_val = JS_GetPropertyStr(ctx, argv[0], "name");

    if (!JS_IsUndefined(name_val)) {
      props->name = JS_ToCString(ctx, name_val);

      if (props->name == NULL) {
        js_free(ctx, props);
        return JS_EXCEPTION;
      }
    }

    JSValue mesh_val = JS_GetPropertyStr(ctx, argv[0], "mesh");

    if (!JS_IsUndefined(mesh_val)) {
      WebSGMeshData *mesh_data = JS_GetOpaque2(ctx, mesh_val, js_websg_mesh_class_id);

      if (mesh_data == NULL) {
        js_free(ctx, props);
        return JS_EXCEPTION;
      }

      props->mesh = mesh_data->mesh_id;
    }

    JSValue translation_val = JS_GetPropertyStr(ctx, argv[0], "translation");

    if (!JS_IsUndefined(translation_val)) {
      if (js_get_float_array_like(ctx, translation_val, props->translation, 3) < 0) {
        js_free(ctx, props);
        return JS_EXCEPTION;
      }
    }

    JSValue rotation_val = JS_GetPropertyStr(ctx, argv[0], "rotation");

    if (!JS_IsUndefined(rotation_val)) {
      if (js_get_float_array_like(ctx, rotation_val, props->rotation, 4) < 0) {
        js_free(ctx, props);
        return JS_EXCEPTION;
      }
    }

    JSValue scale_val = JS_GetPropertyStr(ctx, argv[0], "scale");

    if (!JS_IsUndefined(scale_val)) {
      if (js_get_float_array_like(ctx, scale_val, props->scale, 3) < 0) {
        js_free(ctx, props);
        return JS_EXCEPTION;
      }
    }

  }

  node_id_t node_id = websg_world_create_node(props);

  js_free(ctx, props);

  if (node_id == 0) {
    JS_ThrowInternalError(ctx, "WebSG: Couldn't create node.");
    return JS_EXCEPTION;
  }

  return js_websg_new_node_instance(ctx, world_data, node_id);
}

JSValue js_websg_world_find_node_by_name(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  WebSGWorldData *world_data = JS_GetOpaque(this_val, js_websg_world_class_id);

  size_t length;
  const char* name = JS_ToCStringLen(ctx, &length, argv[0]);

  if (name == NULL) {
    return JS_EXCEPTION;
  }

  node_id_t node_id = websg_world_find_node_by_name(name, length);

  if (node_id == 0) {
    return JS_UNDEFINED;
  }

  return js_websg_get_node_by_id(ctx, world_data, node_id);
}