# Getting Started with WebSG

The WebSG scripting API allows you to build 3D scenes and UI elements in a browser environment. This guide provides a quick introduction to working with various types of objects and interfaces available in the library, along with usage examples.

## World

The `world` global is the central hub for managing all objects and interactions in a WebSG scene. It is similar to the `document` global in a typical browser context.

Existing objects can be queried for in a loaded scene by calling their relatively named find functions:

```typescript
const foundNode = world.findNodeByName("nodeName");
const foundLight = world.findLightByName("lightName");
```

Create new objects like so:

```typescript
const newNode = world.createNode({ name: "nodeName" });
const newLight = world.createLight({ name: "lightName" });
```

## Global Event Handlers

WebSG provides global event handlers to manage key events in the lifecycle of a 3D world. This guide provides an overview of these event handlers and how to use them in your applications.

### onloadworld

The `onloadworld` event handler is called when the world is loaded.

::: warning
One important concept to understand in WebSG is that objects cannot be created, queried for, or otherwise interfaced with until the world is loaded.
:::

```typescript
let node;

onloadworld = () => {
  // Perform actions when the world is loaded
  node = world.findNodeByName("myNode");
};
```

### onenterworld

The `onenterworld` event handler is called when the user enters the world.

```typescript
onenterworld = () => {
  // Perform actions when the user enters the world
  node.translation.y = 1;
};
```

### onupdateworld

The `onupdateworld` event handler is called when the world is updated. It receives two parameters: `dt`, the time since the last update in seconds, and `time`, the total time since the start of the world in seconds.

```typescript
onupdateworld = (dt: number, time: number) => {
  // Perform actions on each world update
  // dt: time since the last update in seconds
  // time: total time since the start of the world in seconds

  node.translation.x += Math.sin(time) * 1 + 2;
};
```

## Basic Usage

### Creating a Node

To create a node, use the `createNode` method of the `world` object and provide an optional name for the node.

```typescript
const node = world.createNode({ name: "MyNode" });
```

### Adding a Mesh to a Node

Create a mesh using the `createBoxMesh` method and add it to the node by including it in the node properties.

```typescript
const mesh = world.createBoxMesh({ size: [1, 1, 1] });

const node = world.createNode({
  name: "MyNode",
  mesh: mesh,
});
```

### Adding a Light to a Node

Define light properties and create a light object using the `createLight` method. Add the light to the node by including it in the node properties.

```typescript
const light = world.createLight({
  type: WebSG.LightType.Point,
  intensity: 1.0,
});

const node = world.createNode({
  name: "MyNode",
  light: light,
});
```

### Adding Interactable to a Node

To make a node interactable, use the `addInteractable` method of the `Node` class.

```typescript
const node = world.createNode({ name: "MyInteractableNode" });
const interactable = node.addInteractable();
```

You can access the `Interactable` instance associated with a node using the `interactable` property:

```typescript
if (node.interactable.pressed) {
  console.log("Node pressed");
}
if (node.interactable.held) {
  console.log("Node held");
}
if (node.interactable.released) {
  console.log("Node released");
}
```

To remove the interactable feature from a node, call the `removeInteractable` method of the `Node` class:

```typescript
node.removeInteractable();
```

After calling `removeInteractable`, the `interactable` property of the node will return `undefined`.

## User Interfaces

UI in WebSG is designed around the [flexbox layout system](https://css-tricks.com/snippets/css/a-guide-to-flexbox/).

### Creating a UI Canvas and Adding Elements

Create a flat-planed canvas in 3D space and add UI elements to it.

```typescript
const canvas = world.createUICanvas({
  // define dimensions
  width: 800, // width in pixels
  height: 600, // height in pixels
  size: [8, 6], // size in meters

  // create a root element
  root: world.createUIElement({
    width: 800,
    height: 600,
    backgroundColor: [0, 0, 0, 0.5], // rgba format
    flexDirection: "column",
  }),
});

const elA = world.createUIElement({
  width: 100,
  height: 100,
  backgroundColor: [1, 1, 1, 0.5],
});

const elB = world.createUIElement({
  width: 50,
  height: 50,
});

canvas.root.addChild(elA);
canvas.root.addChild(elB);
```

### Working with Text

Text can be added to elements, too. Add text by appending it as a child of any element.

```typescript
const canvas = world.createUICanvas({
  width: 800,
  height: 600,
  size: [8, 6],
});

const text = world.createUIText({
  value: "Hello, world!",
  fontSize: 20,
  padding: [5, 5, 5, 5],
  color: [0, 0, 0, 1],
});

canvas.root.addChild(text);
```

### Working with Buttons

Buttons are interactable text elements.

```typescript
const canvas = world.createUICanvas({
  width: 800,
  height: 600,
  size: [8, 6],
});

const button = world.createUIButton({
  label: "Click me!", // shows as a cursor tooltip
  text: "Hello World", // shows as text on the button
  width: 100,
  height: 100,
});

canvas.root.addChild(button);
```

### Handling Button States

Check if the button is pressed, held, or released by accessing the corresponding properties of the button object.

```typescript
if (button.pressed) {
  console.log("Button pressed");
}
if (button.held) {
  console.log("Button held");
}
if (button.released) {
  console.log("Button released");
}
```

## Physics

WebSG provides functionality to create physics-based interactions in your 3D world. You can create different types of physics bodies and colliders to simulate realistic behavior of objects.

### Creating a Physics Body

To create a physics body, use the `addPhysicsBody` method of the `Node` class and provide a `PhysicsBodyProps` object.

```typescript
const boxMesh = world.createBoxMesh({ size: [1, 1, 1] });
const boxNode = world.createNode({ mesh: boxMesh });
boxNode.addPhysicsBody({
  type: WebSG.PhysicsBodyType.Rigid,
  linearVelocity: [0, 0, 0],
  angularVelocity: [0, 0, 0],
  inertiaTensor: [1, 1, 1],
});
```

### Creating a Collider

Create a collider using the `createCollider` method of the `World` class and provide a `ColliderProps` object. Assign the collider to the node by setting the node's `collider` property.

```typescript
const boxCollider = world.createCollider({
  type: WebSG.ColliderType.Box,
  isTrigger: false,
  size: [1, 1, 1],
});
boxNode.collider = boxCollider;
```

### Interacting with Physics Bodies

You can interact with physics bodies by updating their properties.

For dynamic rigid bodies, the linear and angular velocities of the `physicsBody` can be updated.

::: warning
If a body type is `Rigid` then the body is dynamic. This means that the node's translation will be continuously updated by the physics engine, and therefor cannot be updated by any set operations.
:::

```typescript
boxNode.physicsBody.linearVelocity.set([1, 0, 0]);
boxNode.physicsBody.angularVelocity.set([0, 1, 0]);
```

For `Kinematic` and `Static` bodies you can set the node's translation directly and the physics body will update accordingly.

```typescript
boxNode.translation.set([1, 1, 1]);
```

### Removing Physics Bodies and Colliders

You can remove a physics body or collider from a node by calling the `removePhysicsBody` method on a node with a physics body, or by setting the `collider` property of a node to `undefined`.

```typescript
boxNode.removePhysicsBody(); // Remove the physics body
boxNode.collider = undefined; // Remove the collider
```