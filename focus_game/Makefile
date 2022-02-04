jars : controlserver.jar focus_game.jar kafkainterface.jar

kafkainterface.jar : ../scala/kafkainterface.jar
	cp ../scala/kafkainterface.jar .

controlserver.jar : ../scala/controlserver.jar
	cp ../scala/controlserver.jar .

focus_game.jar : ../scala/focus_game.jar
	cp ../scala/focus_game.jar .

../scala/controlserver.jar : 
	cd ../scala
	make controlserver.jar

../scala/focus_game.jar : 
	cd ../scala
	make focus_game.jar
