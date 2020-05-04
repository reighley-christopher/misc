test.blend : blender_manifest.py interpocket.py operator_modal_interpocket_timer.py
	if [ -f "test.blend" ]; then rm test.blend; fi
	blender -b -P blender_manifest.py -y

clean :
	rm test.blend
