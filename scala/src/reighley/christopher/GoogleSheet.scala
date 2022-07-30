package reighley.christopher

import java.util.Base64._
import java.security._
//import java.security.interfaces._
import java.security.spec._
import com.fasterxml.jackson.core._
import com.fasterxml.jackson.databind._
import com.fasterxml.jackson.databind.node._
import java.util.Date
import java.io.File
//import java.io.OutputStreamWriter
//import java.io.DataOutputStream
//import javax.net.ssl.HttpsURLConnection
import java.net.URLEncoder
//import sys.process._
import scala.jdk.CollectionConverters._
import scala.collection.mutable.Buffer
import scala.collection.mutable.ListBuffer
import reighley.christopher.Util.simple_http
import scala.collection.Map

class JWTConnection(account_id:String, private_key_path:String) {
  //TODO keep track of token expiration time
  //TODO Use token to call refresh API
  //TODO this is specific to google but it is an open standard so others might use it (Twitter?)

  val header:Array[Byte] = """{ "alg":"RS256", "typ":"JWT" }""".getBytes

  def claimset(iat:Long, exp:Long ) :Array[Byte] = s"""
  {
  "iss":"$account_id",
  "scope": "https://www.googleapis.com/auth/spreadsheets",
  "aud": "https://oauth2.googleapis.com/token",
  "exp": $exp, 
  "iat": $iat 
  }
  """.getBytes
  
  val grant_type = "urn:ietf:params:oauth:grant-type:jwt-bearer" 
  
  def unsigned_token:Array[Byte] = {
    val enc = getUrlEncoder()
    val now = new Date().getTime()/1000
    (enc.encodeToString(header) + "." + enc.encodeToString(claimset(now-60*15, now+ 60*15) ) ).getBytes
    }

  def get_private_key(filename:String):PrivateKey = 
    {
    val f = new File(filename)
    val parser:JsonParser = new JsonFactory().createParser( f )
    val mapper = new ObjectMapper()
    val tree:JsonNode = mapper.readTree(f) 
    //val tree:TreeNode = parser.readValueAsTree()
    var key = tree.path("private_key").asText().
              replace("-----BEGIN PRIVATE KEY-----", "").
              replace("-----END PRIVATE KEY-----", "").
              replace("\n", "")
    var bytearray = getDecoder().decode(key)
    val keyfactory = KeyFactory.getInstance("RSA")
    val keyspec = new PKCS8EncodedKeySpec(bytearray)
    return keyfactory.generatePrivate(keyspec) 
    }

  def sign(data:Array[Byte]):String = {
    var sig = Signature.getInstance("SHA256withRSA")
    var private_key = get_private_key(private_key_path)
    var enc = getUrlEncoder()
    sig.initSign( private_key )
    sig.update(data)
    data.map(_.toChar).mkString +  "." + enc.encodeToString(sig.sign())
    }

  def post_body = "grant_type=" + URLEncoder.encode(grant_type,"UTF-8") + "&assertion=" + sign(unsigned_token) 

  def get_token = {
    val json =  simple_http( "https://oauth2.googleapis.com/token", post_body, Map("Accept"->"*/*") )
    val mapper = new ObjectMapper()
    val tree:JsonNode = mapper.readTree(json)
    tree.path("access_token").asText() 
    }

  def send(url:String, body:String, header:Map[String,String]):String = simple_http(url, body, header.concat(Map( "Authorization" -> ( "Bearer " + get_token ) ) ) )   
  } 

class GoogleSheet( id:String, header_row:Int, key_name:String, google_account_id:String, private_key_path:String ) {

  val ssl = new JWTConnection(google_account_id, private_key_path)
 
  def google_sheets_batch_update_json_sparse(entries:Map[String, Buffer[Buffer[String]]]) = 
    {
    val user_entered = new TextNode("USER_ENTERED")
    val include_values_in_response = BooleanNode.FALSE
    val data:Buffer[JsonNode] = entries.toList.toBuffer.map( ((datum:Tuple2[String, Buffer[Buffer[String]]]) => {
      val dd:java.util.List[JsonNode] = datum._2.map( (v) => {
            val jj:java.util.List[JsonNode] = v.map( (u) => new TextNode(u) ).asJava.asInstanceOf[java.util.List[JsonNode]]
            new ArrayNode( JsonNodeFactory.instance,jj  ).asInstanceOf[JsonNode] } 
        ).asJava
      new ObjectNode(
      JsonNodeFactory.instance,
      Map[String, JsonNode](
        "range" -> new TextNode(datum._1),
        "majorDimension" -> new TextNode("ROWS"),
        "values" -> new ArrayNode(JsonNodeFactory.instance, dd) 
           ).asJava )
      }))
    val jsn = new ObjectNode(JsonNodeFactory.instance, Map[String, JsonNode]( "valueInputOption" -> user_entered, "data" -> new ArrayNode(JsonNodeFactory.instance, data.asJava), "includeValuesInResponse" -> include_values_in_response ).asJava )
    jsn.toString 
    }
  
  def set_values(values:Map[String,String]) = 
    {
    val data = google_sheets_batch_update_json_sparse( values.view.mapValues( (s) => Buffer(Buffer(s)) ).toMap )
    val url = s"https://sheets.googleapis.com/v4/spreadsheets/$id/values:batchUpdate/"
    ssl.send( url, data, Map("Content-Type" -> "x-application/json") )
    }
  
   def extract_array_from_response( response:String ) = 
    {
    val mapper = new ObjectMapper()
    val tree:JsonNode = mapper.readTree(response)
    val data:JsonNode = tree.path("values").path(0)
    val iterator = data.elements()
    val list = new ListBuffer[String]
    while(iterator.hasNext) { list.append( iterator.next().asText )  } 
    list 
    }
  
   def get_column( column : String ) = 
    {
    val url = s"https://sheets.googleapis.com/v4/spreadsheets/$id/values/$column:$column?majorDimension=COLUMNS"
    val response = ssl.send(url, "", Map() ) 
    extract_array_from_response( response )
    }
  
   def get_row( row : Int ) = 
    {
    val url = s"https://sheets.googleapis.com/v4/spreadsheets/$id/values/$row:$row"
    val response = ssl.send(url, "", Map() ) 
    extract_array_from_response( response )
    }

  val header = get_row(header_row)
  val index = get_column( (header.indexOf(key_name) + 'A').toChar.toString )

  def add_column(new_column:String) = {
    header.append(new_column)
    (header.indexOf(new_column) + 'A').toChar.toString
    } 

  def update_record( record:Map[String, String] ) =  {
    val updates:ListBuffer[Tuple2[String,String]] = ListBuffer()
    //record has key value pairs, the keys are column names that need to be translated into letters
    val this_index = record.keys.map((col) => ( col -> (header.indexOf(col) + 'A').toChar.toString ) ).
                     map( t => t match {
                       case Tuple2(k, "@") => 
                         val c = add_column(k)
                         updates.append( ( c + header_row ) -> k )
                         (k -> c) 
                       case Tuple2(k, v) => (k -> v) 
                       }).toMap
    val row_id = index.indexOf( record( key_name ) ) match { 
      case -1 => index.append(record( key_name )) ;  index.length 
      case i => i+1 
      } 
    updates ++= record.toSeq.map( (v)  => ( (this_index(v._1) + row_id) -> v._2 )  )
    set_values( updates.toMap ) 
    //if column names do not exist we need to assign a new letter
    //the row index will be found from the key
    //if there is no key in the record then do nothing
    //if the key isn't in the index then add it 
    }
   
   
  }
