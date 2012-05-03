
function tokenizer(_input) 
   {
   var rx = /^\s*(([\,\.\(\)\*])|\[([^\]]*)\]|([^\s\,\.\(\)\=\[]*))/;
   var keywords = 
      [
      "select","from","where","left", "right", "outer", "inner", "join",
      "on","having","order", "by","insert","into","update","set","delete", "from", "create",
      "table", "values", "drop", "column", "alter"
      ];

   var input = _input;

   this.next = function()
      {
      var A = rx.exec(input);
      var length = A[0].length;
      var ret = new Object();
      input = input.substring(length);
      if( A[1].length == 0 ) { ret.type = "eof"; return ret; }
      if( A[2] != null) { ret.type = A[2]; return ret;}
      if( A[3] != null) { ret.type = "expression"; ret.expression = A[3]; return ret;}
      if( A[4] != null) 
         {
         /*note, I should not be using indexOf.*/
         /*TODO sort the keywords alphabetically and write a binary search*/
         if( keywords.indexOf(A[4].toLowerCase() ) != -1 ) {ret.type = A[4]; return ret;} 
         else { ret.type = "identifier"; ret.identifier = A[4]; return ret;}
         }
      ret.type = "error";
      ret.token = A[1];
      return ret;
      }
   }

function toStringIndent( string, indent )
   {
   var ret = string;
   for(i = 0; i < indent; i++) { ret = "-&nbsp;" + ret; }
   return ret;
   }

function identifierColumn( table, name, alias )
   {
   this.toString = function( indent ) { return toStringIndent(";table:"+table+";name:"+name+";alias:"+alias + "<br/>", indent); }
   this.type = "column";
   this.evaluate = function( context, accumulator, fromclause )
      {
      accumulator.push( context.evaluate(table?table+"."+name:name));
      }
   this.names = function(fromclause, database){ return [{table:table, name:name, alias:alias}] };
   }

function expressionColumn( expression, alias )
   {
   this.toString = function( indent) { return toStringIndent(";expression:"+expression+";alias:"+alias+"<br/>", indent); }
   this.evaluate = function( context, accumulator, fromclause )
      {
      accumulator.push( context.evaluate(expression) );
      }
   this.type = "column";
   this.alias = function(){ return alias; }
   this.names = function( fromclause, database) { return [{table:null, name:null, alias:alias}] };
   }

function everythingColumn()
   {
   this.toString = function( indent ) {return toStringIndent(";**<br/>", indent);}
   this.type = "column";
   this.names = function( fromclause, database ) 
      { 
      return fromclause.columns;
      };
   this.evaluate = function( context, accumulator, fromclause )
      {
      var c = fromclause.columns;
      var i = 0;
      for(i = 0; i < c.length; i++)
         {
         accumulator.push( context.evaluate(c[i].table?c[i].table+"."+c[i].name:c[i].name));
         }
      }
   }

function wholeTableColumn( table )
   {
   this.toString = function( indent ) {return toStringIndent(";"+table+"*<br/>", indent);}
   this.type = "column";
   this.names = function( fromclause, database ) 
      {
      var ret = new Array();
      var c = fromclause.columns;
      var i = 0;
      for( i = 0; i < c.length; i++ )
         {
         if( c[i].table == table )
            { ret.push( c[i] ); }
         }
      return ret;
      };
   this.evaluate = function( context, accumulator, fromclause )
      {
      var c = fromclause.columns;
      var i = 0;
      for(i = 0; i < c.length; i++)
         {
         if(c[i].table == table)
          { accumulator.push( context.evaluate(c[i].table?c[i].table+"."+c[i].name:c[i].name)); }
         }
      }
   }

function columnList( column )
   {
   var list = new Array();
   this.type = "columnList";
   list.push(column);
   this.add = function( column )
      {
      list.push(column);
      }
   this.toString = function( indent ) 
      {
      var ret = toStringIndent("columnList:</br>", indent); 
      var i = 0;
      for(i = 0; i < list.length; i++) { ret = ret + list[i].toString( indent + 1 );}
      return ret;
      }
   this.columns = function() { return list; }
   }

function tableNameAtom( name, alias )
   {
   this.type = "tableAtom";
   this.toString = function(indent) { return toStringIndent("["+ name + "];alias:" + alias + "</br>", indent); }
   this.fetch = function( database )
      {
      s = database.getTable( name );
      s.setAlias(alias);
      return s;
      }
   }

function subqueryAtom( sql, alias )
   {
   this.type = "tableAtom";
   this.toString = function(indent) { return toStringIndent("SUBQUERY;"+ alias + "<br/>{") + sql.toString(indent+1) + toStringIndent("}<br/>", indent); }
   this.fetch = function( database ) { var s = database.query(sql); s.setAlias(alias); return s; }
   }

function subtreeAtom( subtree, alias )
   {
   this.type = "tableAtom";
   this.toString = function(indent) { return toStringIndent("SUBTREE;"+ alias + "<br/>{") + subtree.toString(indent+1) + toStringIndent("}<br/>", indent); };
   this.fetch = function( database ) { var s = subtree.fetch(database); s.setAlias(alias); return s; } 
   }

function dummyTableAtom()
   {
   this.type = "tableAtom";
   this.toString = function(indent) { return toStringIndent("DUMMY", indent); }
   this.fetch = function( database )
      {
      return new MemoryDataTable( { rows:[null] } );
      }
   }

function tableLeafExpression( atom )
   {
   this.type = "tableExpression";
   this.toString = function(indent) { return atom.toString(indent); };
   this.fetch = function( database )
      {
      return atom.fetch( database );
      }
   }

function joinExpression( op, lhs, rhs, onClause )
   {
   this.type = "tableExpression";
   this.toString = function(indent) { return rhs.toString(indent+1) + toStringIndent("JOIN<br/>", indent) + lhs.toString(indent+1) + toStringIndent(" ON " + onClause + "<br/>", indent); }
   this.fetch = function( database )
      {
      ret = new JoinDataTable(op, lhs.fetch(database), rhs.fetch(database), onClause );
      return ret;
      }
   }

function constructor_table()
   {
   }

function selectStatement( selectList, tablesExpression, whereClause, orderClause )
   {
   this.type = "selectStatement";
   this.selectList = selectList;
   this.tablesExpression = tablesExpression;
   this.whereClause = whereClause;
   this.orderClause = orderClause;
   this.toString = function(indent)
      {
      return toStringIndent("SELECT <br/>", indent) + selectList.toString(indent+1) + toStringIndent("FROM<br/>", indent) + tablesExpression.toString(indent+1) + toStringIndent("WHERE " + whereClause + "<br/>", indent)
             + toStringIndent("ORDER BY<br/>" + orderClause + "<br/>", indent);
      }
   }

function columnDefinitionList( ident )
   {
   this.type = "columnDefinitionList";
   var idents = new Array();
   idents.push(ident);
   this.columnNames = function() { return idents; }
   this.add = function(ident) { idents.push(ident); }
   this.toString = function(indent)
      {
      ret = "";
      for(i = 0; i < idents.length; i++) { ret = ret + idents[i].toString() + "<br/>"; }
      return ret;
      }
   }

function createStatement( constructor, name, columnlist )
   {
   this.type = "createStatement";
   this.tablename = name;
   this.columnnames = columnlist.columnNames();
   this.toString = function(indent) { return "CREATE TABLE " + name + "<br/>" + columnlist.toString(indent+1); }
   }

function insertStatement( tablename, query )
   {
   this.type = "insertStatement";
   this.tablename = tablename;
   this.query = query;
   this.toString = function(indent) { return "INSERT INTO " + tablename + "<br/>" + query.toString(indent+1); }
   }

function state_machine( tokenizer, initialstate )
   {

   var outStack = new Array(0);
   var inStack = new Array(0);
   var currentToken = tokenizer.next();
   var currentState = initialstate;
   this.executeCurrentState = function() { currentState(this, currentToken.type); }

   this.running = function()
      {
      return currentState != null
      }

   this.pushToken = function( token )
      {
      inStack.push( token );
      }

   this.popToken = function()
      {
      currentState = outStack.pop();
      return inStack.pop();
      }

   this.shift = function( state )
      {
      inStack.push(currentToken);
      currentToken = tokenizer.next();
      outStack.push(currentState);
      currentState = state;
      }

   this.reduce = function( production, state )
      {
      production( this );
      outStack.push( currentState );
      currentState = state;
      }

   this.reducego = function( production )
      {
      production( this );
      currentState(this, inStack[inStack.length-1].type);
      }

   this.go = function( state )
      {
      outStack.push( currentState );
      currentState = state;
      }

   this.shiftreduce = function( production, state )
      {
      inStack.push(currentToken);
      currentToken = tokenizer.next();
      outStack.push( null );
      production( this );
      outStack.push( currentState );
      currentState = state;
      }

   this.shiftreducego = function( production )
      {
      inStack.push(currentToken);
      currentToken = tokenizer.next();
      outStack.push( null );
      production( this );
      currentState(this, inStack[inStack.length-1].type);
      }

   this.error = function( message )
      {
      //TODO I can do better than this.
      //The tokenizer can keep track of line numbers, the parser can give state information.
      alert(message);
      currentState = null;
      }

   this.checkToken = function( type )
      {
      var t = inStack.pop();
      currentState = outStack.pop();
      if( type != t.type )
         {
         this.error( "check failed : expected " + type + ", found " + t.type );
         }
      }

   this.complete = function()
      {
      currentState = null;
      }

   }

function parser()
   {

   var data = "";
   this.data = function() { return data; }
   function note( s )
      {
      data = data + s;
      }

   this.parse = function(input)
      {
      machine = new state_machine( new tokenizer(input), state0 );
      note("start");
      i = 0;
      while( machine.running() && i < 1000)
         {
         note(".");
         machine.executeCurrentState();
         i++;
         }
      note("finish");
      }

   this.getResult = function() { return machine.popToken(); }

   /*column1 -> identifier|expression (!identifier) */
   function column1(machine)
      {
      token = machine.popToken();
      switch( token.type )
         {
         case "identifier": machine.pushToken( new identifierColumn( "", token.identifier, token.identifier ) ); break;
         case "expression": machine.pushToken( new expressionColumn( token.expression, "" ) ); break;
         case "*": machine.pushToken( new everythingColumn() ); break;

         default : machine.error("there is an unexpected error with a column1 production");
         }
      }

   /*column2 -> identifier|expression identifier*/
   function column2(machine)
      {
      nameToken = machine.popToken();
      valueToken = machine.popToken();
      switch( valueToken.type )
         {
         case "identifier": machine.pushToken( new identifierColumn( "", valueToken.identifier, nameToken.identifier ) ); break;
         case "expression": machine.pushToken( new expressionColumn( valueToken.expression, nameToken.identifier ) ); break;
         default : error("there is an unexpected error with a column2 production"); 
         }
      }

   /*column3 -> identifier.identifier (!identifier) */
   function column3(machine)
      {
      columnToken = machine.popToken();
      machine.checkToken(".");
      tableToken = machine.popToken();
      machine.pushToken( new identifierColumn( tableToken.identifier, columnToken.identifier, columnToken.identifier ) );
      }

   /*column4 -> identifier.identifier identifier*/
   function column4(machine)
      {
      nameToken = machine.popToken();
      valueToken = machine.popToken();
      machine.checkToken(".");
      tableToken = machine.popToken();
      machine.pushToken( new identifierColumn( tableToken.identifier, valueToken.identifier, nameToken.identifier ) );
      }

   /*column5 -> * */
   function column5(machine)
      {
      machine.checkToken("*");
      machine.pushToken( new everythingColumn() );
      }

   /*column6 -> identifier.* */
   function column6(machine)
      {
      machine.checkToken("*");
      machine.checkToken(".");
      tableToken = machine.popToken();
      machine.pushToken(new wholeTableColumn( tableToken.identifier ) );
      }

   /*columnList1 -> column*/
   function columnList1(machine)
      {
      token = machine.popToken();
      machine.pushToken( new columnList( token ) );
      }

   /*columnList2 -> columnList, column*/
   function columnList2( machine )
      {
      columnToken = machine.popToken();
      machine.checkToken(",");
      columnListToken = machine.popToken();
      columnListToken.add(columnToken);
      machine.pushToken(columnListToken);
      }

   /*tableAtom1 -> identifier (!identifier)*/
   function tableAtom1( machine )
      {
      note("tableAtom1");
      token = machine.popToken();
      machine.pushToken( new tableNameAtom( token.identifier, token.identifier ) );
      }

   /*tableAtom2 -> identifier identifier*/
   function tableAtom2( machine )
      {
      nameToken = machine.popToken();
      valueToken = machine.popToken();
      machine.pushToken( new tableNameAtom( valueToken.identifier, nameToken.identifier ) );
      } 

   /*tableAtom3 -> ( selectStatement ) (!identifier)*/
   function tableAtom3( machine )
      {
      machine.checkToken( ")" );
      selectToken = machine.popToken();
      machine.checkToken( "(" );
      machine.pushToken( new subqueryAtom(selectToken, "") );
      }

   /*tableAtom4 -> ( tableExpression ) identifier */
   function tableAtom4( machine )
      {
      nameToken = machine.popToken();
      machine.checkToken( ")" );
      subTreeToken = machine.popToken();
      machine.checkToken( "(" );
      machine.pushToken( new subtreeAtom( subTreeToken, nameToken.identifier ) );
      }

   /*tableAtom5 -> ( tableExpression )*/
   function tableAtom5( machine )
      {
      machine.checkToken( ")" );
      subTreeToken = machine.popToken();
      machine.checkToken( "(" );
      machine.pushToken( new subtreeAtom( subTreeToken, "" ));
      }

   /*tableAtom6 -> ( selectStatement ) identifier */
   function tableAtom6( machine )
      {
      nameToken = machine.popToken();
      machine.checkToken( ")" );
      sqlToken = machine.popToken();
      machine.checkToken( "(" );
      machine.pushToken( new subqueryAtom( sqlToken, nameToken.identifier ) );
      }

   /*tableExpression1 -> tableatom*/
   function tableExpression1( machine )
      {
      note("tableExpression1");
      machine.pushToken( new tableLeafExpression( machine.popToken() ) );
      }

   /*tableExpression2 -> tableExpression inner join tableExpression*/
   function tableExpression2( machine )
      {
      rhs = machine.popToken();
      machine.checkToken("join");
      machine.checkToken("inner");
      lhs = machine.popToken();
      machine.pushToken( new joinExpression( "inner", lhs, rhs, "true" ) );
      }

   /*tableExpression3 -> tableExpression inner join tableExpression on expression*/
   function tableExpression3( machine )
      {
      onclause = machine.popToken();
      machine.checkToken("on");
      rhs = machine.popToken();
      machine.checkToken("join");
      machine.checkToken("inner");
      lhs = machine.popToken();
      machine.pushToken( new joinExpression( "inner", lhs, rhs, onclause.expression ) );
      }

   /*tableExpression4 -> tableExpression D outer join tableExpression on expression*/
   function tableExpression4( machine )
      {
      onclause = machine.popToken();
      machine.checkToken("on");
      rhs = machine.popToken();
      machine.checkToken("join");
      machine.checkToken("outer");
      direction = machine.popToken();
      lhs = machine.popToken();
      machine.pushToken( new joinExpression( direction.type, lhs, rhs, onclause.expression ) );
      }

   /*selectStatement1 -> select columnList from tableExpression*/
   function selectStatement1( machine )
      {
      tables = machine.popToken();
      machine.checkToken("from");
      columns = machine.popToken();
      machine.checkToken("select");
      machine.pushToken( new selectStatement( columns, tables, "true", "0" ) );
      }

   /*selectStatement2 -> select columnList from tableExpression where expression*/
   function selectStatement2( machine )
      {
      where = machine.popToken();
      machine.checkToken("where");
      tables = machine.popToken();
      machine.checkToken("from");
      columns = machine.popToken();
      machine.checkToken("select");
      machine.pushToken( new selectStatement( columns, tables, where.expression, "0"));
      }

   /*selectStatement3 -> select columnList from tableExpression order by expression*/
   function selectStatement3( machine )
      {
      order = machine.popToken();
      machine.checkToken("by");
      machine.checkToken("order");
      tables = machine.popToken();
      machine.checkToken("from");
      columns = machine.popToken()
      machine.checkToken("select");
      machine.pushToken( new selectStatement( columns, tables, "true", order.expression) );
      }

   /*selectStatement4 -> select columnList from tableExpression where expression order by expression*/
   function selectStatement4( machine )
      {
      order = machine.popToken();
      machine.checkToken("by");
      machine.checkToken("order");
      where = machine.popToken();
      machine.checkToken("where");
      tables = machine.popToken();
      machine.checkToken("from");
      columns = machine.popToken();
      machine.checkToken("select");
      machine.pushToken( new selectStatement( columns, tables, where.expression, order.expression ) );
      }

   /*selectStatement5 -> select columnList*/
   function selectStatement5( machine )
      {
      columns = machine.popToken();
      machine.checkToken("select");
      machine.pushToken( new selectStatement( columns, new tableLeafExpression( new dummyTableAtom() ), "true", "true" ) );
      }

   /*createStatement1 -> create table identifier ( columnDefinitionList )*/
   function createStatement1( machine )
      {
      machine.checkToken(")");
      columns = machine.popToken();
      machine.checkToken("(");
      alias = machine.popToken().identifier;
      machine.checkToken("table");
      machine.checkToken("create");
      machine.pushToken( new createStatement( constructor_table, alias, columns ) );
      }

   /*columnDefinitions1 -> identifier*/
   function columnDefinitions1( machine )
      {
      machine.pushToken( new columnDefinitionList( machine.popToken().identifier ) );
      }

   /*columnDefinitions2 -> columnDefinitions, identifier*/
   function columnDefinitions2( machine )
      {
      ident = machine.popToken();
      machine.checkToken(",");
      defs = machine.popToken();
      defs.add(ident.identifier);
      machine.pushToken(defs);
      }

   /*insert statements*/
   
   /*insertStatement1 -> insert into identifier selectStatement*/
   function insertStatement1( machine )
      {
      query = machine.popToken();
      tablename = machine.popToken();
      machine.checkToken("into");
      machine.checkToken("insert");
      machine.pushToken( new insertStatement(tablename.identifier, query) );
      }

   /*state 0 nothing has been read*/
   function state0( machine, symbol )
      {
      switch( symbol )
         {
         case "select": machine.shift(state_select); break;
         case "create": machine.shift(state_create); break;  
         case "insert": machine.shift(state_insert); break;       
         case "selectStatement": machine.complete(); break;
         case "createStatement": machine.complete(); break;
         case "insertStatement": machine.complete(); break;
         /*case "updateStatement": machine.complete(); break;
         case "deleteStatement": machine.complete(); break;*/
         default: machine.error("expected {select create insert}");
         }
      }

   function state_stub( machine, symbol )
      {
      machine.error("state stub should never have been run; symbol was " + symbol);
      }

   function state_select( machine, symbol )
      {
      switch( symbol )
         {
         case "identifier": machine.shift(state_select_identifier); break;
         case "expression": machine.shift(state_select_expression); break;
         case "*":          machine.shiftreduce(column1, state_stub); machine.reduce(columnList1, state_select_columnList); break;
         default: machine.error("expected valid column");
         }
      }

   function state_select_identifier( machine, symbol )
      {
      switch( symbol )
         {
         case ".": machine.shift(state_select_identifier_dot); break;
         case ",": machine.reduce(column1, state_stub); machine.reduce(columnList1, state_select_columnList); break;
         case "identifier": machine.shiftreduce(column2, state_stub); machine.reduce(columnList1, state_select_columnList); break;
         case "from": machine.reduce(column1, state_stub); machine.reduce(columnList1, state_select_columnList); break;
         default: machine.error("expected {. , from identifier}");
         }
      }

   function state_select_expression( machine, symbol )
      {
      switch( symbol )
         {
         case "from":
         case ",": machine.reduce(column1, state_stub); machine.reduce(columnList1, state_select_columnList); break;
         case "identifier": machine.shiftreduce(column2, state_stub); machine.reduce(columnList1, state_select_columnList); break;
         case "eof": machine.reduce(column1, state_stub); machine.reduce(columnList1, state_select_columnList); break;
         default: machine.error("expected { , from identifier}");
         }
      }

   function state_select_columnList( machine, symbol )
      {
      switch( symbol )
         {
         case ",": machine.shift(state_select_columnList_comma); break;
         case "from": machine.shift(state_select_columnList_from); break;
         case "eof": machine.reducego(selectStatement5); break;
         default: machine.error("expect from");
         }
      }

   function state_select_identifier_dot( machine, symbol )
      {
      switch( symbol )
         {
         case "identifier": machine.shift( state_select_identifier_dot_identifier ); break;
         case "*": machine.shiftreduce( column6, state_stub ); machine.reduce( columnList1, state_select_columnList ); break;
         default: machine.error("expect * or identifier after .");
         }
      }

   function state_select_columnList_comma( machine, symbol )
      {
      switch( symbol )
         {
         case "identifier": machine.shift(state_select_columnList_comma_identifier); break;
         case "expression": machine.shift(state_select_columnList_comma_expression); break;
         case "*":       machine.shiftreduce(column1, state_stub); machine.reduce(columnList2, state_select_columnList); break; 
         default: machine.error("expected valid column");
         }
      }

   function state_select_columnList_from( machine, symbol )
      {
      switch( symbol )
         {
         case "identifier": machine.shift(state_from_X_identifier); break;
         case "(": machine.shift(state_from_X_oparen); break;
         case "tableExpression": machine.go( state_from_X_tableExpression ); break;
         default: machine.error("syntax error after from");
         }
      }

   function state_select_identifier_dot_identifier( machine, symbol )
      {
      switch( symbol )
         {
         case "identifier": machine.shiftreduce(column4, state_stub); machine.reduce(columnList1, state_select_columnList); break;
         case ",": 
         case "from": machine.reduce(column3, state_stub); machine.reduce(columnList1, state_select_columnList); break;
         default: machine.error("expected {, from identifier}");
         }
      }

   function state_select_columnList_comma_identifier( machine, symbol )
      {
      switch( symbol )
         {
         case ".": machine.shift(state_select_columnList_comma_identifier_dot); break;
         case ",": 
         case "from": machine.reduce(column1, state_stub); machine.reduce(columnList2, state_select_columnList); break;
         case "identifier": machine.shiftreduce(column2, state_stub); machine.reduce(columnList2, state_select_columnList); break
         default: machine.error("expected {. , from identifier}");
         }
      }

   function state_select_columnList_comma_expression( machine, symbol )
      {
      switch( symbol )
         {
         case "from":
         case ",": machine.reduce(column1, state_stub); machine.reduce(columnList2, state_select_columnList); break;
         case "identifier": machine.shiftreduce(column2, state_stub); machine.reduce(columnList2, state_select_columnList); break
         case "eof": machine.reduce(column1, state_stub); machine.reduce(columnList2, state_select_columnList); break;
         default: machine.error("expected {. , from identifier}");
         }
      }

   function state_select_columnList_comma_identifier_dot( machine, symbol )
      {
      switch( symbol )
         {
         case "identifier": machine.shift( state_select_columnList_comma_identifier_dot_identifier ); break;
         case "*": machine.shiftreduce(column6, state_stub); machine.reduce( columnList2, state_select_columnList ); break;
         default: machine.error("expect * or identifier after .");
         }
      }

   function state_select_columnList_comma_identifier_dot_identifier( machine, symbol )
      {
      switch( symbol )
         {
         case "identifier": machine.shiftreduce( column4, state_stub ); machine.reduce(columnList2, state_select_columnList); break;
         case ",": 
         case "from": machine.reduce( column3, state_stub ); machine.reduce(columnList2, state_select_columnList); break;
         default: machine.error("expect {, from identifier}");
         }
      }

   function state_from_X_identifier( machine, symbol )
      {
      note("state_from_identifier");
      switch( symbol )
         {
         case "identifier": machine.shiftreduce( tableAtom2 ); machine.reducego(tableExpression1); break;
         case "on": 
            machine.reduce( tableAtom1, state_stub); 
            machine.reducego( tableExpression1 ); 
            break;
         case "where":
         case "order":
         case "left": 
         case "right":
         case "inner":
         case ")": 
             machine.reduce( tableAtom1, state_stub ); 
             machine.reducego( tableExpression1 ); break;
         case "eof": 
             machine.reduce( tableAtom1, state_stub ); 
             machine.reducego( tableExpression1); break;
         default: machine.error("error in FROM clause, expected table or JOIN statement");
         }
      }

   function state_from_X_oparen( machine, symbol )
      {
      switch( symbol )
         {
         case "identifier": machine.shift( state_from_X_identifier ); break;
         case "(": machine.shift( state_from_X_oparen ); break;
         case "select": machine.shift( state_select ); break;
         case "tableExpression": machine.go( state_from_X_oparen_tableExpression ); break;
         case "selectStatement": machine.go( state_from_X_oparen_selectStatement ); break;
         default: machine.error("error after (");
         }
      }

   function state_from_X_oparen_tableExpression( machine, symbol )
      {
      note("state_from_X_oparen_tableExpression");
      switch( symbol )
         {
         case "left": machine.shift( state_from_X_tableExpression_D ); break;
         case "right": machine.shift( state_from_X_tableExpression_D ); break;
         case "inner": machine.shift( state_from_X_tableExpression_inner); break;
         case ")": machine.shift( state_from_X_oparen_tableExpression_cparen ); break;
         case "eof": machine.reduce( selectStatement1 ); break;
         default: machine.error("unclosed parenthesis after join");
         }
      }

   function state_from_X_oparen_selectStatement( machine, symbol )
      {
      switch( symbol )
         {
         case ")": machine.shift( state_from_X_oparen_selectStatement_cparen ); break
         default: machine.error("unclosed parenthesis after subquery");
         }
      }

   /*this state indicates that we are somewhere inside a where clause*/
   function state_from_X_tableExpression( machine, symbol )
      {
      switch( symbol )
         {
         case "where": machine.shift( state_from_tableExpression_where ); break;
         case "order": machine.shift( state_from_tableExpression_order ); break;
         case "left": machine.shift( state_from_X_tableExpression_D ); break;
         case "right": machine.shift( state_from_X_tableExpression_D ); break;
         case "inner": machine.shift( state_from_X_tableExpression_inner); break;
         case ")": machine.reducego( selectStatement1 ); break;
         case "eof": machine.reducego( selectStatement1 ); note("reduce select at eof"); break;
         case "on": machine.error( "peculiar on clause" ); break;
         default: machine.error("expected {where order left right inner eof}");
         }
      }

   function state_from_tableExpression_where( machine, symbol )
      {
      switch( symbol )
         {
         case "expression": machine.shift( state_where_expression ); break;
         default: machine.error("required expression after where, found : " + symbol);
         }
      }

   function state_from_tableExpression_order( machine, symbol )
      {
      switch( symbol )
         {
         case "by": machine.shift( state_from_tableExpression_order_by ); break;
         default: machine.error("missing BY in ORDER BY : found " + symbol);
         }

      }

   function state_from_X_tableExpression_D( machine, symbol )
      {
      switch( symbol )
         {
         case "outer": machine.shift( state_from_tableExpression_D_outer ); break;
         default: machine.error("expected OUTER");
         }
      }

   function state_from_X_tableExpression_inner( machine, symbol )
      {
      switch( symbol )
         {
         case "join": machine.shift( state_from_tableExpression_inner_join ); break;
         default: machine.error("expected JOIN");
         }
      }

   function state_from_X_oparen_selectStatement_cparen( machine, symbol )
      {
      switch( symbol )
         {
         case "identifier": 
            machine.shiftreduce( tableAtom6, state_stub ); 
            machine.reducego( tableExpression1 ); break;
         case "inner": 
         case "left": 
         case "right": 
         case "eof": 
         case ")": 
         case "on":
         case "where":
         case "order":
            note("reducing select in subquery");
            machine.reduce( tableAtom3, state_stub);
            machine.reducego( tableExpression1 ); break;
         default: machine.error("identifier or join after nested select statement");
         }
      }

   function state_from_X_oparen_tableExpression_cparen( machine, symbol )
      {
      note("state_from_oparen_tableExpression_cparen");
      switch( symbol )
         {
         case "identifier": 
            machine.shiftreduce( tableAtom4, state_stub ); 
            machine.reducego( tableExpression1 ); break;
         case "inner": 
         case "left": 
         case "right": 
         case ")": 
         case "eof": 
         case "where": 
         case "order": 
            machine.reduce( tableAtom5, state_stub ); 
            machine.reducego( tableExpression1 ); break;
         case "on": 
            machine.reduce( tableAtom5, state_stub); 
            machine.reducego( tableExpression1 ); 
            break;
         default: machine.error("Syntax error after closing parenthesis");
         }
      }

   function state_where_expression( machine, symbol )
      {
      switch( symbol )
         {
         case "order": machine.shift( state_where_expression_order ); break;
         case "eof": machine.reducego( selectStatement2 ); break;
         case ")": machine.reducego( selectStatement2 ); break;
         default: machine.error("expect {) EOF order}");
         }
      }

   function state_from_tableExpression_order_by( machine, symbol )
      {
      switch( symbol )
         {
         case "expression": machine.shiftreducego( selectStatement3 ); break;
         default: machine.error("ORDER BY clause must be a single expression, sorry");
         }
      }

   function state_from_tableExpression_D_outer( machine, symbol )
      {
      switch( symbol )
         {
         case "join": machine.shift( state_from_tableExpression_D_outer_join ); break;
         default: machine.error("OUTER must be followed by JOIN");
         }
      }

   function state_from_tableExpression_inner_join( machine, symbol )
      {
      note("state_from_tableExpression_inner_join");
      switch( symbol )
         {
         case "identifier": machine.shift( state_from_X_identifier ); break;
         case "(": machine.shift( state_from_X_oparen ); break;
         case "tableExpression": machine.go( state_from_tableExpression_inner_join_tableExpression ); break;
         default: machine.error("expected table after inner join");
         }
      }

   function state_where_expression_order( machine, symbol )
      {
      switch( symbol )
         {
         case "by": machine.shift( state_where_expression_order_by ); break;
         default: machine.error("expected BY after ORDER");
         }
      }

   function state_from_tableExpression_D_outer_join( machine, symbol )
      {
      switch( symbol )
         {
         case "identifier": machine.shift( state_from_X_identifier ); break;
         case "(": machine.shift( state_from_X_oparen ); break;
         case "tableExpression": machine.go( state_from_tableExpression_D_outer_join_tableExpression ); break;
         default: machine.error("expected table after outer join");
         }
      }

   function state_from_tableExpression_inner_join_tableExpression( machine, symbol )
      {
      switch( symbol )
         {
         case "on": machine.shift( state_inner_join_tableExpression_on ); break;
         case "eof": machine.reducego( tableExpression2 ); note("reduce tableExpression at eof"); break;
         case ")": machine.reducego( tableExpression2 ); break;
         case "order": machine.reducego( tableExpression2 ); break;
         case "where": machine.reducego( tableExpression2); break;
         case "left":
         case "right": machine.shift( state_from_X_tableExpression_D ); break;
         case "inner": machine.shift( state_from_X_tableExpression_inner); break;
         default: machine.error("malformed inner join");
         }
      }

   function state_where_expression_order_by( machine, symbol )
      {
      switch( symbol )
         {
         case "expression": machine.shiftreducego( selectStatement4 ); break;
         default: machine.error("ORDER BY clause must be a single expression, so sorry");
         }
      }

   function state_from_tableExpression_D_outer_join_tableExpression( machine, symbol )
      {
      note("state_from_tableExpression_D_outer_join_tableExpression");
      switch( symbol )
         {
         case "on": machine.shift( state_outer_join_tableExpression_on ); break;
         case "left":
         case "right": machine.shift( state_from_X_tableExpression_D ); break;
         case "inner": machine.shift( state_from_X_tableExpression_inner); break;
         default: machine.error("outer join expression must have ON clause");
         }
      }

   function state_inner_join_tableExpression_on( machine, symbol )
      {
      switch( symbol )
         {
         case "expression": machine.shiftreducego( tableExpression3 ); break;
         default: machine.error("ON clause requires expression");
         }
      }

   function state_outer_join_tableExpression_on( machine, symbol )
      {
      switch( symbol )
         {
         case "expression": machine.shiftreducego( tableExpression4 ); break;
         default: machine.error("ON clause requires expression");
         }
      }
   
   /*CREATE statement*/

   function state_create( machine, symbol )
      {
      switch( symbol )
         {
         case "table": machine.shift(state_create_table); break;
         default: machine.error("create must be table");
         }
      }

   function state_create_table( machine, symbol )
      {
      switch( symbol )
         {
         case "identifier": machine.shift(state_create_table_identifier); break;
         default: machine.error("table requires a name");
         }
      }

   function state_create_table_identifier( machine, symbol )
      {
      switch( symbol )
         {
         case "(": machine.shift( state_table_oparen ); break;
         default: machine.error("create statement lacks definitions");
         }
      }

   function state_table_oparen( machine, symbol )
      {
      switch( symbol )
         {
         case "identifier": machine.shiftreduce( columnDefinitions1, state_table_oparen_columnDefs ); break;
         default: machine.error("column names must be valid identifiers");
         }
      }

   function state_table_oparen_columnDefs( machine, symbol )
      {
      switch( symbol )
         {
         case ",": machine.shift(state_columnDefs_comma); break;
         case ")": machine.shiftreducego( createStatement1 ); break;
         default: machine.error("column names to be separated by commas");
         }
      }

   function state_columnDefs_comma( machine, symbol )
      {
      switch( symbol )
         {
         case "identifier": machine.shiftreduce( columnDefinitions2, state_table_oparen_columnDefs ); break;
         default: machine.error("error after comma : did not expect " + symbol);
         }
      }

   /*INSERT statement */
   function state_insert( machine, symbol )
      {
      switch( symbol )
         {
         case "into": machine.shift( state_insert_into ); break;
         default: machine.error("expected into after insert");
         }
      }

   function state_insert_into( machine, symbol )
      {
      switch( symbol )
         {
         case "identifier": machine.shift( state_insert_into_identifier ); break;
         default: machine.error("expect table name after into");
         }
      }
  
   function state_insert_into_identifier( machine, symbol )
      {
      switch( symbol )
         {
         case "select": machine.shift( state_select ); break;
         case "selectStatement": machine.go( state_insert_select ); break;
         }
      }

   function state_insert_select( machine, symbol )
      {
      switch( symbol )
         {
         case "eof": machine.reducego( insertStatement1 ); break;
         default: machine.error("insert statement improperly terminated");
         }
      }
   }

function recordset()
   {
   var table = new Array();

   function sorter( a, b )
      {
      return a.index - b.index;
      }

   function cursor()
      {
      var i = 0;
      this.eof = false;
      this.toArray = function()
         {
         return table[i].row;
         }
      this.next = function()
         {
         i = i+1;
         if( i >= table.length) { this.eof = true; i = i-1; }
         }
      }

   this.getCursor = function() { return new cursor(); }

   this.add = function( row, index )
      {
      table.push( { index:index, row:row } );
      }

   this.toHTML = function() 
      {
      var ret = "<table><tr>"; 
      var i = 0;
      var j = 0;
      table.sort( sorter );
      for(i = 0; i < this.columns.length; i++)
         {
         ret = ret+"<th>"+this.columns[i].alias+"</th>"
         }
      ret = ret+"</tr>";
      for(i = 0; i < table.length; i++)
         {
         ret = ret + "<tr>"
         for(j = 0; j < table[i].row.length; j++)
            {
            ret = ret + "<td>" + table[i].row[j] + "</td>";
            }
         ret = ret + "</tr>"
         }
      return ret+"</table>"; 
      }

   this.getContext = function() { return createContext(this.columns); }

      this.setAlias = function(alias)
         {
         var i;
         for(i = 0; i < this.columns.length; i++ ) { this.columns[i].table = alias; }
         }
   }

function database()
   {

   var catalogs = new Array();

   this.addCatalog = function( catalog )
      {
      catalogs.push(catalog);
      }

   this.execute = function( command )
      {
      switch( command.type )
         {
         case "selectStatement": return( executeSelect(command, this).toHTML() ); break;
         case "createStatement": return( executeCreate(command, this) ); break;
         case "insertStatement": return( executeInsert(command, this) ); break;
         default: alert("database error");
         }
      }

   function executeSelect( command, database )
      {
      /*naive execution of a SELECT statement*/
      /*fetch the result as a record set*/
      /*remove rows based on where clause*/
      /*generate sort key*/
      /*execute select list in row context*/
      var result = new recordset();
      var tables = command.tablesExpression.fetch(database);
      var context = tables.getContext();
      var row; 
      var index;
      var i = 0;
      var j = 0;
      var columns = command.selectList.columns();
      var context = createContext( tables.columns );
      var cursor = tables.getCursor();
      result.columns = new Array();
      for(i = 0; i < columns.length; i++)
         { 
         for(j = 0; j < columns[i].names(tables, this).length; j++)
            {
            result.columns.push( columns[i].names(tables, this)[j] );
            }
         }
    
      while( !cursor.eof )
         {
         row = new Array();
         context.initialize( cursor.toArray() );
         for( j = 0; j < columns.length; j++ )
            {
            if( context.evaluate( command.whereClause ) )
               {
               columns[j].evaluate( context, row, tables );
               }
            }
         result.add( row , context.evaluate(command.orderClause) );
         cursor.next();
         }
      return result;
      }

   function executeCreate( command , database )
      {
      catalogs[0].create( command.tablename, command.columnnames );
      return "CREATE COMMAND SUCCESSFUL"
      }

   function executeInsert( command , database)
      {
      var i = 0;
      var recordset = executeSelect( command.query );
      var cursor = recordset.getCursor();
      while( !cursor.eof )
         {
         catalogs[0].insert( command.tablename , cursor.toArray() );
         i = i+1;
         cursor.next();
         }
      return "INSERT COMMAND SUCCESSFUL : " + i + " ROWS"
      }

   /*this is the generic database interface*/
   this.getTable = function(tablename){return catalogs[0].getTable( tablename );} 
   this.query = function(query){ return executeSelect(query, this); }
   }


