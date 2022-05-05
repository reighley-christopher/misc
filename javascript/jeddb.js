http = require('http');
fs = require('fs');
url = require('url');

server = http.createServer( function( request, response ) {
   if( request.method == 'GET' )
      {
      response.write( JSON.stringify("you should not have used GET") );
      response.end();
      }
   if( request.method == 'POST' )
      {
      console.log('POST ');
      request.on('data', function(chunk) {
         fs.writeFile("/www/static/jeddb.json", chunk);
         response.write( JSON.stringify( "nothing to report" ) );
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
server.listen('15080');

