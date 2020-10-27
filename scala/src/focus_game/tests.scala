package focus_game.tests
import org.scalatest._
import focus_game._ 
import java.time.ZonedDateTime

class RecordSpec extends FunSuite {

  val testjson = "{\"id\":\"test\",\"state\":\"resting\",\"score\":1,\"period\":\"2020-05-01T01:01:10.123Z\",\"lastUpdate\":\"2020-05-01T01:01:10.123Z\",\"expectedUpdate\":\"2020-05-01T01:01:10.123Z\"}"

  val testrec = FocusLogic.Record("test", FocusLogic.State.resting, 1, ZonedDateTime.parse("2020-05-01T01:01:10.123Z"), ZonedDateTime.parse("2020-05-01T01:01:10.123Z"), ZonedDateTime.parse("2020-05-01T01:01:10.123Z") )

  test("serialize") {
    assert( FocusLogic.RecordFromJson(testjson).id == "test" ) 
    }
  
  test("deserialize") {
    assert( FocusLogic.JsonFromRecord(testrec).length == testjson.length )
    }

  test("complete loop") {
    val rerec = FocusLogic.RecordFromJson(  FocusLogic.JsonFromRecord( testrec ) )
    assert( rerec.id == testrec.id ) 
    assert( rerec.state == testrec.state ) 
    assert( rerec.score == testrec.score ) 
    assert( rerec.period == testrec.period ) 
    assert( rerec.lastUpdate == testrec.lastUpdate ) 
    assert( rerec.expectedUpdate == testrec.expectedUpdate ) 
    }

  }

class InputSpec extends FunSuite {

  val testjson = "{\"id\":\"test\",\"event\":\"start\",\"timestamp\":\"2020-05-01T01:01:10.123Z\"}"

  val testinput = FocusLogic.Input("test", FocusLogic.Event.start, ZonedDateTime.parse("2020-05-01T01:01:10.123Z") )

  test("serialize") {
    assert( FocusLogic.InputFromJson(testjson).id == "test" ) 
    }
  
  test("deserialize") {
    assert( FocusLogic.JsonFromInput(testinput).length == testjson.length )
    }

  test("complete loop") {
    val rerec = FocusLogic.InputFromJson( FocusLogic.JsonFromInput( testinput ) )
    assert( rerec.id == testinput.id ) 
    assert( rerec.event == testinput.event ) 
    assert( rerec.timestamp == testinput.timestamp ) 
    }

  }

class MigrationSpec extends FunSuite {
  //what if the Record is mangled, ie what if we have changed the schema
  //if we are deserializing a mangled record from the local store then we need to ignore old columns and give new columns reasonable defaults
 
  test("unfamiliar value") {
    val testjson = "{\"id\":\"test\",\"state\":\"resting\",\"score\":1,\"period\":\"2020-05-01T01:01:10.123Z\",\"lastUpdate\":\"2020-05-01T01:01:10.123Z\",\"expectedUpdate\":\"2020-05-01T01:01:10.123Z\", \"foo\":\"bar\"}"
   assert( FocusLogic.RecordFromJson(testjson).id == "test" )
   }

  test("missing value") {
    val testjson = "{\"id\":\"test\",\"state\":\"resting\",\"score\":1,\"period\":\"2020-05-01T01:01:10.123Z\",\"lastUpdate\":\"2020-05-01T01:01:10.123Z\", \"foo\":\"bar\"}"
    assert( FocusLogic.RecordFromJson(testjson).id == "test" )
    }
  }

