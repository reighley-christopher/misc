/*this is the web worker
its job is to stay awake and manage the clock
that is all it needs to do every second it sends a heartbeat to the serviceworker to keep it awake!
on android if it doesn't do anything for a long time it is gradually deprioritized so it sends a message every 100 milliseconds to keep itself relevant
chromium devs say they are going to start downgrading wakeup clocks for pages that have been hidden for 5 minutes or more so the final time keeper
should be robust to sleeping for along time

API :
messages accepted are the same as the status messages sent to the server 
"start" resets the clock
"stop" kills all timers

messages sent are of the form {"type":type, "payload":payload}
if type is keepalive

if type is notificaiton

if type is clock

*/

var ticker = 0;
var lasttime = (new Date).getTime()
var maxinterval = 0
var lastmessage = (new Date).getTime()
var starttime = new Date
console.log("worker start")
onmessage = function(message){
  console.log("clock worker start")
  send_notification("clock worker start")
  setTimeout( function() { send_notification("Twenty Five Minute Timer " + starttime) } , 25 * 60 * 1000) 
  inner_loop()
}

var keepalive_counter = 0;
function send_keepalive() {
  keepalive_counter = keepalive_counter + 1
  postMessage( { "type" : "keepalive" , "payload" : keepalive_counter } )
  }

function send_notification( note ) {
  postMessage( {"type" : "notification", "payload" : note } )
  }

function inner_loop() {
    var time = (new Date).getTime()
    var interval = time - lasttime
    lasttime = time
    if( interval > maxinterval) { maxinterval = interval }
    if( lasttime - lastmessage > 60000 ) {
      send_notification( "one minute timer " + (lastmessage - lasttime) + " " + maxinterval )
      lastmessage = lasttime
      } 
    send_keepalive() 
  setTimeout( inner_loop, 100 )
  }
