chaintools : chaintools/ChainTools.class chaintools/HTTPService.class chaintools/Riak.class chaintools/Kafka.class chaintools/Sql.class

chaintools/ChainTools.class : src/chaintools/ChainTools.scala
	scalac src/chaintools/ChainTools.scala/ChainTools.scala

chaintools/HTTPService.class : src/chaintools/HttpService.scala
	scalac src/chaintools/HttpService.scala

chaintools/Riak.class : src/chaintools/Riak.scala
	scalac src/chaintools/Riak.scala

chaintools/Sql.class : src/chaintools/Sql.scala
	scalac src/chaintools/Sql.scala

chaintools/Kafka.class : src/chaintools/Kafka.scala 
	scalac src/chaintools/Kafka.scala

focus_game/FocusGame.class : src/focus_game/FocusGame.scala
	scalac src/focus_game/FocusGame.scala

reighley/christopher/Properties.class : src/reighley/christopher/Properties.scala
	scalac src/reighley/christopher/Properties.scala

controlserver/Server.class : src/controlserver/Server.scala
	scalac src/controlserver/Server.scala

controlserver/Socket2.class : src/controlserver/Socket2.scala
	scalac src/controlserver/Socket2.scala

controlserver.jar : controlserver/Server.class controlserver/Socket2.class
	jar cfm controlserver.jar src/META-INF/controlserver.MANIFEST.MF controlserver/*

focus_game.jar : focus_game/FocusGame.class reighley/christopher/Properties.class chaintools/Kafka.class 
	jar cf focus_game.jar focus_game/* reighley/* chaintools/* 

clean :
	rm controlserver/*.class

run :
	scala controlserver/Server
