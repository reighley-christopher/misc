
function HTMLCatalog( root )
   {

   this.create = function( tablename, columnlist ) 
      {
      var t = document.createElement( "table" );
      var tr = document.createElement( "tr" );
      var th;
      var i;
      t.id = tablename;
      t.appendChild(tr);
      for(i = 0; i < columnlist.length; i++)
         {
         th = document.createElement("th");
         th.innerHTML = columnlist[i];
         tr.appendChild(th);
         }
      root.appendChild(t);
      }

   this.insert = function( tablename, valuelist )
      {
      var td;
      var tr = document.createElement( "tr" );
      var t = document.getElementById(tablename);
      t.appendChild(tr);
      for(i = 0; i < valuelist.length; i++)
         {
         td = document.createElement( "td");
         td.innerHTML = valuelist[i];
         tr.appendChild(td);
         }
      }

  function tableMeta( tableElement )
      {
      var ths = tableElement.getElementsByTagName("tr")[0].getElementsByTagName("th");
      this.getCursor = function()
         { 
         return new tableCursor
            ( 
            tableElement
            )
         }
      this.columns = new Array();
      for( i = 0; i < ths.length; i++ )
         {
         this.columns.push({table:tableElement.id, name:ths[i].innerHTML, alias:ths[i].innerHTML});
         } 

      this.getContext = function() { return createContext(this.columns); }
      this.setAlias = function(alias)
         {
         var i;
         for(i = 0; i < this.columns.length; i++ ) { this.columns[i].table = alias; }
         }
      }

  this.getTable = function( tablename )
      {
      return new tableMeta(document.getElementById(tablename));
      }

   function tableCursor( tableElement )
      {
      var trs;
      var i = 0;
      this.eof = false;
      trs = tableElement.getElementsByTagName("tr");
      this.next = function()
         {
         i = i+1;
         if( i >= trs.length ) {i = i-1; this.eof = true;}
         }
      this.next(); //get rid of the header row;
      this.toArray = function()
         {
         var tds = trs[i].getElementsByTagName("td");
         var ret = new Array();
         var j = 0;
         var val;
         var num;
         for(j = 0; j < tds.length; j++) 
            { 
            val = tds[j].innerHTML;
            num = parseInt(val,10);
            if(!isNaN(num)) val = num;
            ret.push(val); 
            }
         return ret;
         }
      }

   }
