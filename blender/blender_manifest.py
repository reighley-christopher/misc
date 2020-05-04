#create and save a blender file out of certain assets

import bpy

#remove the default light, camera, and box
for i in bpy.data.objects :
  bpy.data.objects.remove(i)

#add the socket server
bpy.data.texts.load("interpocket.py")
bpy.data.texts.load("operator_modal_interpocket_timer.py")
for i in bpy.data.texts :
  i.use_module = True 

#save
bpy.ops.wm.save_mainfile(filepath="/home/reighley/Code/misc/blender/test.blend") 

