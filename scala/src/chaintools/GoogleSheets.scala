package chaintools

import reighley.christopher.GoogleSheet
import reighley.christopher.Properties.properties
import scala.util.parsing.json.JSON

object GoogleSheets {

  var testid = "12SPSNnOWyQsHRxDp3JCiQPsXRnitJPJK4Z63svssK-E";

  class SheetsInterface( sheet:GoogleSheet ) extends ChainSink[String]( (data) => {
      sheet.update_record( JSON.parseFull(data).get.asInstanceOf[Map[String,String]] )
      }  )

  def google_sheet(sheet_id:String, header_row:Int, key_column:String) = {
    new SheetsInterface(new GoogleSheet(sheet_id, header_row, key_column, 
          properties.get("google.account_id").asInstanceOf[String], 
          properties.get("google.key_path" ).asInstanceOf[String] ) ) 
    }
  } 
