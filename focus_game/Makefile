jars : controlserver.jar chaintools.jar reighley.jar focus_game.jar

controlserver.jar : ../scala/controlserver.jar
	cp ../scala/controlserver.jar .

focus_game.jar : ../scala/focus_game.jar
	cp ../scala/focus_game.jar .

../scala/controlserver.jar : 
	cd ../scala
	make controlserver.jar
