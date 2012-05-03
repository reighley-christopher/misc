function createContext( columns )
   {
   var s1 = "new function(){ this.evaluate=function(from){ return eval(from);} ; this.initialize=function(from){  "
   var s2 = "";
   var i;
   var o;
   var dict = new Object();
   for( i = 0; i < columns.length; i++ )
      {
      if( columns[i].table != null && columns[i].table.length>0 && dict[columns[i].table] == null  )
         {
         s2 = s2 + "var " + columns[i].table + " = new Object(); ";
         dict[columns[i].table] = true;
         }
      }
   for( i = 0; i < columns.length; i++ )
      {
      if( dict[columns[i].name] == null )
         {
         s2 = s2 + "var " + columns[i].name + "; ";
         s1 = s1 + columns[i].name + "=from[" + i + "]; ";
         if( columns[i].table != null  && columns[i].table.length > 0) 
            {
            s1 = s1 + columns[i].table + "." + columns[i].name + "=from[" + i + "]; ";
            }
         dict[columns[i].name] = true;
         }
      } 
   s1 = s1 + " } ; " + s2 + " } ";
   return eval(s1);
   }

/*these are functions for operations on heterogeneous data sources*/
/*the standard interface must provide : */
/*   getCursor() */ 
/*   columns */
/*getCursor() return a cursor which includes the members */
/*   eof */
/*   next() */
/*   toArray() */
function MemoryDataTable( initializer )
   {
   var data = initializer;
   this.addRow = function( row )
      {
      initializer.rows.push(row);
      }

   function cursor()
      {
      this.eof = false;
      var i = 0;
      this.next = function()
         {
         i = i+1;
         if( i >= data.rows.length ) { i=i-1; this.eof = true }
         }
      this.toArray = function() { return data.rows[i]; }
      }

   /*standard interface starts here*/
   this.getCursor = function() {return new cursor();}
   
   this.columns = initializer.columns==null?[]:initializer.columns.map(function (c) { return {name:c, alias:c} } );
      this.setAlias = function(alias)
         {
         var i;
         for(i = 0; i < columns.length; i++ ) { this.columns[i].table = alias; }
         }
   this.getContext = function() { return createContext(this.columns); };

   }

function JoinDataTable( direction, lhs, rhs, onClause )
   {
   var primary = direction=="right"?rhs:lhs;
   var secondary = direction=="right"?lhs:rhs;
   
   this.columns = lhs.columns.concat( rhs.columns );
  
   var context = createContext( this.columns );
   this.getCursor = function() { return new cursor(); }

   this.getContext = function() { return createContext(this.columns); }
      this.setAlias = function(alias)
         {
         var i;
         for(i = 0; i < this.columns.length; i++ ) { this.columns[i].table = alias; }
         }
   function cursor()
      {
      var pr_c = primary.getCursor();
      var se_c = secondary.getCursor();
      var on;
      var row;
      var null_row = true;
      this.eof = false;
      /*I do this a lot , because I need context and a row to return*/
      /*will need to be called every time I incriment one of the cursors*/
      this.update_state = function()
         {
         this.eof = pr_c.eof;
         row = direction=="right"?se_c.toArray().concat(pr_c.toArray()):pr_c.toArray().concat(se_c.toArray());
         context.initialize(row);
         on = context.evaluate( onClause );
         }

      this.next = function()
         {
         se_c.next();
         this.update_state();

         /*
         the condition I wish to hold at the end of this process is that either se_c and pr_c
         satisfy the on clause (on == true && se_c.eof == false) or this is an outer join and I have exhausted se_c 
         without ever matching to pr_c or pr_c is done
         */
         while( !( 
                 (on && !se_c.eof && !pr_c.eof ) || /*a match*/
                 (direction != "inner" && se_c.eof && null_row) || /*a null row*/ 
                 pr_c.eof /*end of join*/
               ) )
            {
            se_c.next();
            if( se_c.eof && (!null_row || direction=="inner") )
               {
               pr_c.next();
               se_c = secondary.getCursor();
               null_row = true;
               }
            this.update_state();
            } 
         null_row = false;
         }

      this.update_state();
      if( !on ) { this.next(); } else { null_row = false; }

      this.toArray = function()
         {
         if( !se_c.eof ) 
            { 
            return row;
            } else {
            return direction=="right"?se_c.toArray().map( function(c) { return null; } ).concat(pr_c.toArray()):pr_c.toArray().concat( se_c.toArray().map( function(c) { return null;} ) );
            }
         }
      }
   }
