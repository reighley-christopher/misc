/*
if the service worker doesn't do anything for about 30 seconds it gets cleaned up
what does "do something" mean? updating local variables is not enough, sending a notification is not enough
therefore I will need a Web Worker to manage the clock (web worker won't send notifications)

the service worker is responsible for :
the login data
  accepts login data when it starts
the clock
  keeps a local clock
  syncs it to the server clock
  sends clock update messages every second when the clock is running
  raises a notification when the clock changes state (and clears the notification when the button is pressed)
the leaderboard
  sends a leaderboard update message when the leaderboard updates
  creates an entry with 0 on the leaderboard if there is none for this user
the server
  accepts start and cancel messages
*/

var clocker = 0

function clock() {
    /*on Android Chrome, inside a Service Worker Process, setInterval behaves like setTimeout and does not
      trigger for long wait times (30 seconds triggers, 2 minutes does not
    */
    if( clocker % 10 == 0 ) { 
      self.registration.showNotification("TEN SECONDS" + clocker, {})
      } 
    clocker = clocker+1
    setTimeout( clock, 1000 )
    console.log("I tried");
    }
 
console.log("start worker")
onmessage = function(e) {
  //var notification = new Notification("FOCUS ONE", {} )
  console.log("mark 1")
  self.registration.showNotification("FOCUS ONE rev12 " + clocker, {})
  console.log("mark 2")

  try {
    setTimeout( clock, 1000 ) 
    } catch(err) {
    clocker = err.message
    self.registration.showNotification(err.message)
    }

  //start_clock()

  }

oninstall = function(e) {
  install_new_worker()
  }

function install_new_worker() {
  /*Android Chrome does not seem to kill the service worker when there are no more pages using it,
    so when I download a new worker I should explicitely reboot every time*/
    skipWaiting();
    /*TODO use Clients.claim() to find existing tabs and attach them to the new worker*/
    /*TODO get login information from old environment so I don't have to log in again*/
  }

function start_clock() {
  setInterval( function() {
    postMessage({"type":"tick", "mode":current_mode(), "time":current_time()})
    }, 1000 )
  }

