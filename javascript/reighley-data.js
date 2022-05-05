
function default_path() {
  var key = window.location.search.substr(1);
  var default_path = "http://new.reighley-christopher.net/riak/buckets/jeddb/keys/" + key + "/"
  }

function default_riak_loader(path)
  {
  this.load = function(callback)
    {
    $.ajax({ url: path,  cache : false } ).
      done(function(d){ 
      callback(data);
      } ).
      fail( function(e) { callback({"schema":[], "data":[]}); console.log("ERROR"); console.log(e) } ) 
    return DB;
    }
  
  this.store = function(document)
    {
    $.ajax({ url:path , type : "POST", data : JSON.stringify( document ) }
      ).fail( function(e) { console.log("ERROR"); console.log(e) } )
    }
  }

function localhost_loader()
  {
  var key = window.location.search.substr(1);
  }

function Database(loader)
  {
  var DB;
  loader.load( function(d){DB=d} );
  this.commit = function()
    {
    loader.store(DB);
    }
  this.raw = function() { return DB }
  }


