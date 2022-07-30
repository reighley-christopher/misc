use std::io;
use std::io::*;
use std::rc::Rc;

//graph protocol 
//create and delete remote nodes and edges
//sync local representation from remote implementation
//query possible node and connection types (and bridge capabilities)
//modify existing node or edge
//create bridge nodes between domains what does this mean?
//types of connections we may want to support
// - unix fifo
// - tcp socket (maybe unix domain socket also)
// - kafka topic (maybe other kinds of pub subs?)
// - web socket
// - http post
// 
// some bridges may be able to cross only certain domains
// for example fifo requires both domain providers be running on the same host

enum Protocol
  {
  Fifo(String)
  }

struct Domain
  {
  }

struct BridgeCap
  {
  protocol: Protocol 
  }
 
struct NodeType
  {
  name: String,
  } 


enum Command
  {
  Query,
  Exit,
  SetInit(String),
  SetTerm(String),
  Node(String),
  Domain(String, String),
  Environment(String, String),
  Configure(String, String),
  Unknown(String)
  }

fn string_clipper<'a>( s:&'a String ) -> (Option<&'a str>, &'a str, Option<&'a str>)
  {
  for i in 1 .. s.len() 
    {
    if s[0..1] == *" " { return ( Some(&s[0..1]), &s[1..i], Some( &s[i+1..s.len()-1] ) ); }
    }
  return ( Some(&s[0..1]), &s[1..s.len()-1], None); 
  }

fn command_parser( s:String ) -> Command
  {
  use Command::*;
  let stdout = io::stdout();
  let mut handle = stdout.lock();
  handle.write(s.as_bytes()).unwrap_or_else(|_| panic!() );
  match string_clipper(&s) 
    {
    ( Some("?"), _, _ ) => Query,
    ( Some("x"), _, _ ) => Exit,
    ( Some("+"), n, _ ) => Node(n.to_string().clone()),
    ( Some("<"), n, _ ) => SetInit(n.to_string().clone()),
    ( Some(">"), n, _ ) => SetTerm(n.to_string().clone()),
    ( Some("@"), n, Some(d) ) => Domain(n.to_string().clone(), d.to_string().clone()), 
    ( Some("@"), n, None ) => Domain(n.to_string().clone(), String::new()), 
    ( Some("|"), n, Some(d) ) => Configure(n.to_string().clone(), d.to_string().clone()),
    ( Some("|"), n, None ) => Configure(n.to_string().clone(), String::new()),
    ( Some("$"), n, Some(d) ) => Environment(String::new(), String::new()),
    ( Some(x), _, _ ) => Unknown(x.to_string().clone()),
    ( None, _, _ ) => Unknown(String::new())
    }
  }

struct Node
  {
  name:String
  }

struct Edge
  {
  initial:Rc<Node>,
  terminal:Rc<Node>
  } 

struct Graph
  {
  nodes : Vec<Rc<Node>>,
  edges : Vec<Edge> 
  }

impl Graph
  {
  fn query(&self)
    {
    for n in &self.nodes
      {
      println!("node:{}", n.name);
      }
    for e in &self.edges
      {
      println!("edge:{} -> {}", e.initial.name, e.terminal.name); 
      } 
    }


  fn add_edge(&mut self, init_name:String, term_name:String)
    {
    let edge:Edge = Edge{initial: self.add_node(init_name) , terminal: self.add_node(term_name) };
    self.edges.push(edge);
    }

  fn add_node(&mut self, name:String) -> Rc<Node> 
    {
    //self.nodes is passed to push as mut, I don't know why but that is how it is
    let opt = self.find_node(&name);
    return match opt 
      {
      None => { 
        let ret = Rc::new( Node{name : name} ); 
        self.nodes.push( ret.clone() ); 
        ret
        }, 
      Some(x) => x
      }
    }

  fn find_node(&self, name:&str) -> Option<Rc<Node>>
    {
    for n in &self.nodes
      {
      if n.name == *name { return Some(n.clone()) }
      }
    return None
    }

  }

fn main() 
  {
  let stdin = io::stdin();
  let mut graph = Graph{edges:vec![], nodes:vec![]};
  let mut initial:Option<String> = None;
  let mut terminal:Option<String> = None;
  loop
    {
    let mut buffer = String::new();
    stdin.read_line(&mut buffer).unwrap_or_else(|_| panic!());
    match command_parser( buffer )
      {
      Command::Unknown(x) => println!("unknown command {}", x.to_string() ),
      Command::Query => { graph.query() },
      Command::SetInit(x) if terminal.is_some() => { graph.add_edge(x.to_string(), terminal.unwrap_or_default()); terminal=None },
      Command::SetInit(x) => { initial=Some(x.to_string() ); },
      Command::SetTerm(x) if initial.is_some() => { graph.add_edge(initial.unwrap_or_default(), x.to_string()); initial=None }, 
      Command::SetTerm(x) => { terminal=Some( x.to_string() ); },
      Command::Environment(_,_) => (),
      Command::Configure(_,_) => (),
      Command::Domain(_,_) => (),
      Command::Node(x) => { graph.add_node(x.to_string()); }, 
      Command::Exit => break 
      }
    }
  }
