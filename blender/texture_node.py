import bpy
import numpy
from bpy.types import NodeSocket, TextureNodeImage, TextureNode

# Custom socket type
class HookSocketType(NodeSocket):
    # Description string
    '''Custom node socket type'''
    # Optional identifier string. If not explicitly defined, the python class name is used.
    bl_idname = 'HookSocketType'
    # Label for nice name display
    bl_label = 'Hook Socket'
    def draw(self, context, layout, node, text):
       pass
    # Socket color
    def draw_color(self, context, node):
        return (0.0, 0.0, 0.0, 1.0)

class HookNode(TextureNode):
    '''Hook Into Nodes Interface'''
    bl_idname = 'HookNode'
    bl_label = 'Hook Node'
    bl_icon = 'SOUND'

    selfpath = bpy.props.StringProperty()
    
    def init(self, context):
        self.inputs.new('HookSocketType', "Data In")
        self.outputs.new('HookSocketType', "Data Out")

    # Copy function to initialize a copied node from an existing one.
    def copy(self, node):
        print("Copying from node ", node)

    # Free function to clean up on removal.
    def free(self):
        print("Removing node ", self, ", Goodbye!")

    # Additional buttons displayed on the node.
    def draw_buttons(self, context, layout):
        layout.prop(self, "selfpath")

    # Optional: custom label
    # Explicit user label overrides this, but here we can define a label dynamically
    def draw_label(self):
        return "Hook"

    def update(self):
        print("update reighley node")
        how = self.inputs["Data In"]
        if how.is_linked : 
            print(dir(how.links[0].from_node))

class StrangeOutput(TextureNode):
    # === Basics ===
    # Description string
    '''A custom node'''
    # Optional identifier string. If not explicitly defined, the python class name is used.
    bl_idname = 'StrangeOutput'
    # Label for nice name display
    bl_label = 'Strange Output'
    # Icon identifier
    bl_icon = 'SOUND'

    # === Custom Properties ===
    # These work just like custom properties in ID data blocks
    # Extensive information can be found under
    # http://wiki.blender.org/index.php/Doc:2.6/Manual/Extensions/Python/Properties
    myStringProperty = bpy.props.StringProperty()
    myFloatProperty = bpy.props.FloatProperty(default=3.1415926)

    # === Optional Functions ===
    # Initialization function, called when a new node is created.
    # This is the most common place to create the sockets for a node, as shown below.
    # NOTE: this is not the same as the standard __init__ function in Python, which is
    #       a purely internal Python method and unknown to the node system!
    def init(self, context):
        
        self.inputs.new('NodeSocketColor', "Data In")
        self.inputs.new('NodeSocketVector', "!")

        self.outputs.new('NodeSocketColor', "How")
        self.outputs.new('NodeSocketColor', "are")
        self.outputs.new('NodeSocketFloat', "you")

    # Copy function to initialize a copied node from an existing one.
    def copy(self, node):
        print("Copying from node ", node)

    # Free function to clean up on removal.
    def free(self):
        print("Removing node ", self, ", Goodbye!")

    # Additional buttons displayed on the node.
    def draw_buttons(self, context, layout):
        layout.label("Node settings")
        layout.prop(self, "myFloatProperty")

    # Detail buttons in the sidebar.
    # If this function is not defined, the draw_buttons function is used instead
    def draw_buttons_ext(self, context, layout):
        layout.prop(self, "myFloatProperty")
        # myStringProperty button will only be visible in the sidebar
        layout.prop(self, "myStringProperty")

    # Optional: custom label
    # Explicit user label overrides this, but here we can define a label dynamically
    def draw_label(self):
        return "I am a custom node"

    def update(self):
        print("update reighley node")
        how = self.inputs["Data In"]
        if how.is_linked : 
            print(dir(how.links[0].from_node))

class StrangeInput(TextureNodeImage):
    # === Basics ===
    # Description string
    '''A custom node'''
    # Optional identifier string. If not explicitly defined, the python class name is used.
    bl_idname = 'StrangeInput'
    # Label for nice name display
    bl_label = 'Strange Input'
    # Icon identifier
    bl_icon = 'SOUND'

    # === Custom Properties ===
    # These work just like custom properties in ID data blocks
    # Extensive information can be found under
    # http://wiki.blender.org/index.php/Doc:2.6/Manual/Extensions/Python/Properties
    myStringProperty = bpy.props.StringProperty()
    myFloatProperty = bpy.props.FloatProperty(default=3.1415926)

    # === Optional Functions ===
    # Initialization function, called when a new node is created.
    # This is the most common place to create the sockets for a node, as shown below.
    # NOTE: this is not the same as the standard __init__ function in Python, which is
    #       a purely internal Python method and unknown to the node system!
    def init(self, context):
        
        self.inputs.new('NodeSocketVector', "!")

        self.outputs.new('NodeSocketColor', "How")
        self.outputs.new('NodeSocketColor', "are")
        self.outputs.new('NodeSocketFloat', "you")

    # Copy function to initialize a copied node from an existing one.
    def copy(self, node):
        print("Copying from node ", node)

    # Free function to clean up on removal.
    def free(self):
        print("Removing node ", self, ", Goodbye!")

    # Additional buttons displayed on the node.
    def draw_buttons(self, context, layout):
        layout.label("Node settings")
        layout.prop(self, "myFloatProperty")

    # Detail buttons in the sidebar.
    # If this function is not defined, the draw_buttons function is used instead
    def draw_buttons_ext(self, context, layout):
        layout.prop(self, "myFloatProperty")
        # myStringProperty button will only be visible in the sidebar
        layout.prop(self, "myStringProperty")

    # Optional: custom label
    # Explicit user label overrides this, but here we can define a label dynamically
    def draw_label(self):
        return "I am a custom node"

    def update(self):
        print("update reighley node")
        how = self.outputs["How"]
        if how.is_linked : 
            print(how.links)
            #how.links[0].to_socket.default_value = self.image

# Derived from the Node base type.
class MyCustomNode(TextureNode):
    # === Basics ===
    # Description string
    '''A custom node'''
    # Optional identifier string. If not explicitly defined, the python class name is used.
    bl_idname = 'CustomNodeType'
    # Label for nice name display
    bl_label = 'Custom Node'
    # Icon identifier
    bl_icon = 'SOUND'

    # === Custom Properties ===
    # These work just like custom properties in ID data blocks
    # Extensive information can be found under
    # http://wiki.blender.org/index.php/Doc:2.6/Manual/Extensions/Python/Properties
    myStringProperty = bpy.props.StringProperty()
    myFloatProperty = bpy.props.FloatProperty(default=3.1415926)

    # === Optional Functions ===
    # Initialization function, called when a new node is created.
    # This is the most common place to create the sockets for a node, as shown below.
    # NOTE: this is not the same as the standard __init__ function in Python, which is
    #       a purely internal Python method and unknown to the node system!
    def init(self, context):
        
        self.inputs.new('NodeSocketFloat', "World")
        self.inputs.new('NodeSocketVector', "!")

        self.outputs.new('NodeSocketColor', "How")
        self.outputs.new('NodeSocketColor', "are")
        self.outputs.new('NodeSocketFloat', "you")

    # Copy function to initialize a copied node from an existing one.
    def copy(self, node):
        print("Copying from node ", node)

    # Free function to clean up on removal.
    def free(self):
        print("Removing node ", self, ", Goodbye!")

    # Additional buttons displayed on the node.
    def draw_buttons(self, context, layout):
        layout.label("Node settings")
        layout.prop(self, "myFloatProperty")

    # Detail buttons in the sidebar.
    # If this function is not defined, the draw_buttons function is used instead
    def draw_buttons_ext(self, context, layout):
        layout.prop(self, "myFloatProperty")
        # myStringProperty button will only be visible in the sidebar
        layout.prop(self, "myStringProperty")

    # Optional: custom label
    # Explicit user label overrides this, but here we can define a label dynamically
    def draw_label(self):
        return "I am a custom node"


### Node Categories ###
# Node categories are a python system for automatically
# extending the Add menu, toolbar panels and search operator.
# For more examples see release/scripts/startup/nodeitems_builtins.py

import nodeitems_utils
from nodeitems_utils import NodeCategory, NodeItem


# our own base class with an appropriate poll function,
# so the categories only show up in our own tree type
class MyNodeCategory(NodeCategory):
    @classmethod
    def poll(cls, context):
        return context.space_data.tree_type == 'TextureNodeTree'

# all categories in a list
node_categories = [
    # identifier, label, items list
    MyNodeCategory("REIGHLEYNODES", "Reighley's Nodes", items=[
        # the node item can have additional settings,
        # which are applied to new nodes
        # NB: settings values are stored as string expressions,
        # for this reason they should be converted to strings using repr()
        NodeItem("HookNode", label="Hook Node"),
        NodeItem("StrangeOutput", label ="Strange Output"),
        NodeItem("StrangeInput", label="Strange Input", settings={
            "myStringProperty": repr("Lorem ipsum dolor sit amet"),
            "myFloatProperty": repr(1.0),
            }),
        NodeItem("CustomNodeType", label="Node B", settings={
            "myStringProperty": repr("consectetur adipisicing elit"),
            "myFloatProperty": repr(2.0),
            }),
        ]),
    ]


def register():
    bpy.utils.register_class(HookSocketType)
    bpy.utils.register_class(MyCustomNode)
    bpy.utils.register_class(HookNode)
    bpy.utils.register_class(StrangeInput)
    bpy.utils.register_class(StrangeOutput)
    nodeitems_utils.register_node_categories("REIGHLEYNODES", node_categories)


def unregister():
    nodeitems_utils.unregister_node_categories("REIGHLEYNODES")
    bpy.utils.unregister_class(StrangeOutput)
    bpy.utils.unregister_class(StrangeInput)
    bpy.utils.unregister_class(HookNode)
    bpy.utils.unregister_class(MyCustomNode)
    bpy.utils.unregister_class(HookSocketType)


if __name__ == "__main__":
    #unregister()
    register()
