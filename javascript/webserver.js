http = require('http');
fs = require('fs');
url = require('url');
mongo = require('mongodb');

var db;
var db_tables;
var db_columns;
var db_rows;
var cache = {};

function createConnection()
   {
   db = new mongo.Db('test', new mongo.Server('localhost', 27017, {}), {});
   db.open( function(err, con) 
      { 
      if( err ) { console.log(err) }; 
      db_tables = db.collection('tables');
      db_columns = db.collection('columns');
      db_rows = db.collection('rows');
      }); 
   }

function insertTable( table )
   {
   var i;
   var _table = db.pkFactory.createPk();
   table._id = _table;
   console.log( table._id );
   db_tables.insert({"name" : table.name, "_id" : _table });
   for( i=0; i < table.columns.length; i++ )
      {
      table.columns[i]._id = db.pkFactory.createPk();
      table.columns[i]._table = _table;
      }
   db_columns.insert(table.columns);
   for( i=0; i < table.rows.length; i++ )
      {
      table.rows[i]._table = _table;
      table.rows[i]._id = db.pkFactory.createPk();
      }
   db_rows.insert(table.rows); 
   console.log("*" + JSON.stringify(table));
   return table;
   }

function readTable( name, callback )
   {
   var table;
   var columns;
   console.log(name);
   db_tables.findOne({name:name}, function(err,table)
      {
      db_columns.find({"_table": table._id}).toArray( function(err, columns)
         {
         table.columns = columns;
         db_rows.find({"_table": table._id}).toArray(function(err, rows)
            {
            table.rows = rows;
            callback(table);
            });
         });
      });
   return table;
   }

function updateTable( table, callback )
   {
   var i;
   var _id;
   for(i = 0; i < table.rows.length; i++)
      {
      if( table.rows[i]._id ) { table.rows[i]._table = mongo.ObjectID(table.rows[i]._table); _id = mongo.ObjectID(table.rows[i]._id); delete table.rows[i]._id; console.log(table.rows[i]); db_rows.update({_id:_id}, {$set:table.rows[i]}, {safe:true}, function(err, result){console.log("update"); console.log(err); console.log(result);} ); }
      else { table.rows[i]._table = mongo.ObjectID(table._id); table.rows[i]._id = db.pkFactory.createPk(); db_rows.insert(table.rows[i]); }
      }      
   for(i = 0; i < table.columns.length; i++)
      {
      if( table.columns[i]._id ) { table.columns[i]._table = mongo.ObjectID(table.columns[i]._table); _id = mongo.ObjectID(table.columns[i]._id); delete table.columns[i]._id; db_columns.update({_id:_id}, {$set:table.columns[i]} ); }
      else { table.columns[i]._table = mongo.ObjectID(table._id); table.columns[i]._id = db.pkFactory.createPk(); db_columns.insert(table.columns[i]); }
      }
   readTable( table.name, callback );   
   }

server = http.createServer( function( request, response ) {
   if( request.method == 'GET' )
      {
      var data;
      var req_url = url.parse( request.url=='/'?'/face.html':request.url ).pathname.substring(1);
      try 
         {
         if( cache[req_url] )
            { data = cache[req_url]; response.write( data ); response.end(); } 
         else if( req_url.indexOf('/') > 0 )
            {
            readTable( req_url.substring(0, req_url.indexOf('/') ), function( data ) 
               {
               console.log(data);
               response.write( JSON.stringify(data) );
               response.end();
               });
            } else
            { data = fs.readFileSync( req_url, 'utf8' ); /*cache[req_url] = data;*/ response.write( data ); response.end(); }
         
         }
      catch( er )
         {
         console.log(er);
         response.writeHeader(404);
         response.end();
         }  
         
      }
   if( request.method == 'POST' )
      {
      console.log('POST ');
      request.on('data', function(chunk) { 
         response.write( JSON.stringify( insertTable( JSON.parse(chunk) ) ));
         response.writeHeader(200, {'Content-Type': 'application/json'});
         response.end();
         } );      
      }
   if( request.method == 'PUT' )
      {
      console.log('PUT ');
      request.on('data', function(chunk) 
         { 
         updateTable( JSON.parse(chunk), function(data)
            {
            response.write(JSON.stringify(data)); 
            response.end(); 
            console.log(data);
            } )
          } );
      response.writeHeader(200);
      }
   } );
createConnection();
server.listen('8080');

