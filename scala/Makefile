chaintools : chaintools/ChainTools.class chaintools/HttpService.class

chaintools/ChainTools.class : src/chaintools/ChainTools.scala
	scalac ChainTools.scala

chaintools/HttpService.class : src/chaintools/HttpService.scala
	scalac src/chaintools/HttpService.scala

chaintools/Riak.class : src/chaintools/Riak.scala
	scalac src/chaintools/Riak.scala

controlserver/Server.class : controlserver/Server.scala
	scalac controlserver/Server.scala

clean :
	rm controlserver/*.class

run :
	scala controlserver/Server
