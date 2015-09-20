import bpy
import interpocket

class ModalTimerOperator(bpy.types.Operator):
    """Operator which runs its self from a timer"""
    bl_idname = "wm.modal_timer_operator"
    bl_label = "Modal Timer Operator"

    _timer = None
    _intpock = None
    
    def modal(self, context, event):
        if event.type == 'TIMER':
            self._intpock.poll()
        return {'PASS_THROUGH'}

    def execute(self, context):
        wm = context.window_manager
        self._timer = wm.event_timer_add(0.1, context.window)
        self._intpock = interpocket.Interpocket("/home/reighley/test_socket", {"C": bpy.context, "D": bpy.data, "bpy":bpy})
        self._intpock.start()
        wm.modal_handler_add(self)
        return {'RUNNING_MODAL'}

    def cancel(self, context):
        self._intpock.end()
        wm = context.window_manager
        wm.event_timer_remove(self._timer)

def register():
    bpy.utils.register_class(ModalTimerOperator)

def unregister():
    bpy.utils.unregister_class(ModalTimerOperator)

if __name__ == "__main__":
    register()

    # test call
    bpy.ops.wm.modal_timer_operator()
