/*the interface to the apply operation subsystem*/

struct _apply_controller
   {
   struct _apply_substitution *memory;
   int named_i, anon_i, nsize;
   };

struct _apply_substitution
   {
   int name;
   struct _object value;
   };

struct _apply_machine
   {
   long int *memory;
   struct _object_type **types;
   struct _apply_substitution *substitutions;
   };

struct _apply_controller *new_apply_controller( int nsize, int asize );

struct iCode *apply_get_iCode( struct _apply_controller *AC );

void add_named_substitution( struct _apply_controller *AC, int name, struct _object value );

void add_anonymous_substitution( struct _apply_controller *AC, struct _object value );





