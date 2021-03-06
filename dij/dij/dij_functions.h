struct _parameter
   {
   char               curry_flag;
   char               consumed_flag;
   struct _object     value;
   struct _parameter *next; 
   };

typedef void (*half_execution_mechanism)(struct _function_metadata *meta, struct _parameter *p, struct _machine *M );

struct _partial_call
   {
   int params;
   struct _parameter *first_param;
   struct _parameter *last_param;
   
   };

struct _function_codeblock
{
   int *instructions;
   struct _function_codeblock *next, *last;
};

struct _function_metadata
{
   int num_parameters, num_locals, num_returns;
   int *parameters, *locals, *returns;
   struct _function_metadata *up;
   iMachine *(*generator)(void *code);
   void *code;
};

/*
the objects which need to be exposed by this module are :

(1)  a procedure symbol
A node of a graph of related procedures.  The operations on it are :

(a) bind parameters
(b) left join
(c) namespace join
(d) load

it's child types are :

(A) primitive procedure
(B) parameter binding
(C) lvalue/rvalue distinction
(D) namespace connection

(2) an incomplete call
this is an object that is going to be the argument to a bind parameters operation, when it is done.  The operations on it are :

(a) apply
(b) curry
(c) close
*/

/*begin procedure symbol data type*/

enum _procedure_subclass_type
{
  primitive,
  binding,
  lvalue,
  namespace
};

typedef struct _procedure_symbol
{
enum _procedure_subclass_type type
union
      
void _procedure_symbol_load( procedure_symbol *p )
{
  select (p->type)
    {
    case primitive: break;
    case binding: break;
    case lvalue: break;
    case namespace: break;
    } 
} 

