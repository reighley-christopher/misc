package kafkainterface

import java.io.File

import reighley.christopher.Interactive
import reighley.christopher.CommandDelegate

class SendDelegate(threadm:ConsumerProducerAdmin, topic:String , key:String ) extends CommandDelegate {
  def process(line:String, contents:String):String = {
    line match {
      case "send" => threadm.send(topic, key, contents) 
      case "clear" => return ""
      case _ => {}
      }
    return contents  
    }
  def close():String = "" 
  }

class Parser(threadm:ConsumerProducerAdmin) {
  def files() = (new File("shunts")).list()  

  def list() = threadm.topics().toArray().mkString("\n") + "\n----\n" + files().mkString("\n")
  def tableize(topic:String, filename:String) = threadm.tableize(topic, filename) 
  def untableize(topic:String) = "UNIMPLEMENTED\n"
  def listen(topic:String) = threadm.listen(topic) 
  def unlisten(topic:String) = "UNIMPLEMENTED\n"
  def send(topic:String, key:String) = Interactive.start(new SendDelegate(threadm, topic, key), 27) 
  def shunt(inpu:String, outpu:String) = {
    if( files().indexOf(outpu) > 0 ) {
      threadm.create_kafka_to_file_shunt(inpu, "shunts/" + outpu)  
      } else {
      threadm.create_kafka_to_kafka_shunt(inpu, outpu) 
      }
    } //TODO other kinds of shunts 
  def unshunt(endpoint:String) = "UNIMPLEMENTED\n"
  def create(topic:String) = threadm.addTopic(topic)
  def delete(topic:String) = threadm.removeTopic(topic)  
  def info() = threadm.info()

  def run( command:String ):String = {
    val cmd = command.split(" ")
    cmd(0) match {
      case "list" => list() 
      case "tableize" => tableize(cmd(1), cmd(2)) ; ""
      case "untableize" => untableize(cmd(1))
      case "listen" => listen(cmd(1)) ; ""
      case "unlisten" => unlisten(cmd(1))
      case "dump" => threadm.dump(cmd(1))
      case "send" => send(cmd(1), cmd(2))
      case "shunt" => shunt(cmd(1), cmd(2)) ; ""
      case "unshunt" => unshunt(cmd(1))
      case "info" => info()
      case "create" => create(cmd(1)) ; "" //TODO format the output instead of ignoring it
      case "delete" => delete(cmd(1)) ; ""
      case _ => """commands :
                  |list
                  |tableize
                  |untableize
                  |listen
                  |unlisten
                  |dump
                  |send
                  |shunt
                  |unshunt
                  |info
                  |create
                  |delete""".stripMargin
      } 
    }  
  } 


