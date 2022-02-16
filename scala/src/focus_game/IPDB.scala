package focus_game

import controlserver.FanInOut
import chaintools._
import chaintools.File._
import chaintools.HTTPService._
import chaintools.ChainTools._

class IPDB(host:String, port:Int, root:String) {
  val faninout = FanInOut(root)
  val service_read = loiteringHttp(host, port , "/ipread/" )
  val service_write = http(host, port, "/ipwrite/")
  def start() = {
    faninout.input("fifo_in")
    faninout.output("fifo_out")
    faninout.attribute("add_input_separator", "10")
    faninout.execute("/usr/bin/sqlite3 " + root + " ipdb.sqlite")
    faninout.noisy

    detach( service_read > json_annotations > json_to_map > tweek("X-real-ip", x => x.split(':')(0) )  > map_to_json > 
            templated("select * from ( select '<%= key %>' ) left outer join (select name from ips where ip = '<%= X-real-ip %>') ;") > 
            throughprint[String]) > write_fifo(root + "/fifo_in")
    
    detach( service_write > json_annotations > json_to_map > tweek("X-real-ip", x => x.split(':')(0)) > map_to_json > 
            templated("insert into ips values ('<%= name %>', '<%= X-real-ip %>');") > 
            throughprint[String] ) > write_fifo(root + "/fifo_in") 

    detach(read_fifo(root + "/fifo_out") > throughprint[String] > split('|') > label(Array("key", "value")) > 
             map_to_json > extract_annotation("key"))  > service_read

    }
  }

