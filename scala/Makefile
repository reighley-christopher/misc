chaintools : chaintools/ChainTools.class chaintools/HTTPService.class chaintools/Riak.class chaintools/Kafka.class chaintools/Sql.class

chaintools/AnnotatedString.class : src/chaintools/AnnotatedString.scala
	scalac src/chaintools/AnnotatedString.scala

chaintools/ChainTools.class : src/chaintools/ChainTools.scala
	scalac src/chaintools/ChainTools.scala

chaintools/HTTPService.class : src/chaintools/HttpService.scala
	scalac src/chaintools/HttpService.scala

chaintools/Riak.class : src/chaintools/Riak.scala chaintools/AnnotatedString.class 
	scalac src/chaintools/Riak.scala

chaintools/Sql.class : src/chaintools/Sql.scala
	scalac src/chaintools/Sql.scala

chaintools/Kafka.class : src/chaintools/Kafka.scala 
	scalac src/chaintools/Kafka.scala

focus_game/FocusLogic.class : src/focus_game/FocusGame.scala reighley/christopher/EnumerationModule.class reighley/christopher/ZonedDateTimeModule.class
	scalac src/focus_game/FocusGame.scala

reighley/christopher/Properties.class : src/reighley/christopher/Properties.scala
	scalac src/reighley/christopher/Properties.scala

reighley/christopher/ZonedDateTimeModule.class : src/reighley/christopher/ZonedDateTimeModule.scala
	scalac src/reighley/christopher/ZonedDateTimeModule.scala

reighley/christopher/EnumerationModule.class : src/reighley/christopher/EnumerationModule.scala
	scalac src/reighley/christopher/EnumerationModule.scala

controlserver/Server.class : src/controlserver/Server.scala
	scalac src/controlserver/Server.scala

controlserver/Socket2.class : src/controlserver/Socket2.scala
	scalac src/controlserver/Socket2.scala

controlserver.jar : controlserver/Server.class controlserver/Socket2.class
	jar cfm controlserver.jar src/META-INF/controlserver.MANIFEST.MF controlserver/*

focus_game.jar : focus_game/FocusLogic.class reighley/christopher/Properties.class chaintools/Kafka.class chaintools/Riak.class 
	jar cf focus_game.jar focus_game/* reighley/* chaintools/* 

test : src/focus_game/tests.scala focus_game/FocusLogic.class  
	scalac src/focus_game/tests.scala
	scala org.scalatest.run focus_game.tests.RecordSpec focus_game.tests.InputSpec focus_game.tests.MigrationSpec 

clean :
	rm controlserver/*.class

run :
	scala controlserver/Server
