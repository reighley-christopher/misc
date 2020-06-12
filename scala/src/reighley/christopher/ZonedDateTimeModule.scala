package reighley.christopher

import com.fasterxml.jackson.databind.ObjectMapper
import com.fasterxml.jackson.databind.JsonSerializer
import com.fasterxml.jackson.databind.JsonDeserializer
import com.fasterxml.jackson.databind.DeserializationContext
import com.fasterxml.jackson.databind.module.SimpleModule
import com.fasterxml.jackson.annotation.PropertyAccessor
import com.fasterxml.jackson.databind.SerializerProvider
import com.fasterxml.jackson.annotation.JsonAutoDetect.Visibility
import com.fasterxml.jackson.core.JsonParser
import com.fasterxml.jackson.core.JsonGenerator
import java.time.ZonedDateTime
import java.time.format.DateTimeFormatter
import scala.reflect._

object ZonedDateTimeDeserializer extends JsonDeserializer[ZonedDateTime] {
  def deserialize( p : JsonParser, ctxt : DeserializationContext ) : ZonedDateTime = 
    ZonedDateTime.parse( p.getText )
  } 

object ZonedDateTimeSerializer extends JsonSerializer[ZonedDateTime] {
  def serialize( value : ZonedDateTime, gen : JsonGenerator, serializers : SerializerProvider ) : Unit = 
    {
    gen.writeString(value.format(DateTimeFormatter.ISO_ZONED_DATE_TIME))
    } 
  }

object ZonedDateTimeModule
  {
  def apply() : SimpleModule =  
    {
    val ret = new SimpleModule("ZonedDateTimeModule")
    ret.addDeserializer[ZonedDateTime](classTag[ZonedDateTime].runtimeClass.asInstanceOf[Class[ZonedDateTime]], ZonedDateTimeDeserializer);
    ret.addSerializer[ZonedDateTime](classTag[ZonedDateTime].runtimeClass.asInstanceOf[Class[ZonedDateTime]], ZonedDateTimeSerializer);
    return ret;
    }
  } 

