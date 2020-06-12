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

class EnumerationDeserializer[T](obj : scala.Enumeration) extends JsonDeserializer[T] {
  def deserialize( p : JsonParser, ctxt : DeserializationContext ) = obj.withName(p.getText).asInstanceOf[T] 
  } 

class EnumerationSerializer[T] extends JsonSerializer[T] {
  def serialize( value : T, gen : JsonGenerator, serializers : SerializerProvider ) : Unit =
    {
    gen.writeString( value.toString )
    }
  }

object EnumerationModule
  {
  val mod = new SimpleModule("EnumerationModule")
  def add[T](obj : scala.Enumeration)( implicit tag : ClassTag[T] ) : Unit =  
    {
    mod.addDeserializer(tag.runtimeClass.asInstanceOf[Class[T]], new EnumerationDeserializer[T](obj))
    mod.addSerializer(tag.runtimeClass.asInstanceOf[Class[T]], new EnumerationSerializer[T]());
    }
  def apply() = mod
  } 

