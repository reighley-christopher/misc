/*a namespace represents a map between the name of a variable and the place it is stored 
  in memory.  If the variable may be an anonymous but required parameter (created in dij
  by a curry, or by a call out to C or some other framework for which the names of the
  parameters are unavailable.  In this case the name will be -1.  The namespace map
  covers only the required parameters, the non-required ones are presumably nameless.
  the operations on a namespace are :
     parameter application
        if x is a function with at least 2 parameters
           x, x., x.. and x... are all equivalent 
           x.2, x.2. and x..2 are functions with one less parameter and one more local variable.
        with only one parameter
           x, x. and x.. are all equivalent
           x... is a function with two parameters, the second one passed anonymously.
           x.2 is a function with no paramaters. 
           x.2. can be expected to execute.
           x..2 is a function with with one parameter, 2 is passed anonymously.
        with no parameters
           x. can be expected to execute.
           x.. is a function with one parameter, passed anonymously.
           x... is a function with two parameters, passed anonymously.
           x.2 is a function with no parameters.
           x.2. can be expected to execute.
           x..2 is a function with one parameter.
     generally, if F is a list of identifiers and . symbols:
        (F). is not necessarily equivalent to F.
        if F is a function with no parameters, then F. will execute.
        if F is a function then F.. will not execute, neither will (F..).
        if .F is a function which takes another function as its first parameter.
*/