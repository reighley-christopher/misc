(function(){
  _.decode =   function(arr) {
    str = "";
    for(i in arr) {
      str += String.fromCharCode(arr[i]);
    }
    return JSON.parse(str);
  }

  Backbone.Socket = function(name) {
    _.extend(this, Backbone.Events);
    this.lastmessage = {};
    var self = this;
    var socket = io.connect('/');
    socket.on(name, function (data) {
      self.lastmessage = data;
      self.trigger("message", _.decode(data.data));
      console.log(_.decode(data.data));
    });
  };

  executor = function(str) {
    var set_output_websocket = function(socket_name)
      {
      console.log("I am executing");
      console.log(socket_name);
      container.output = new Backbone.Socket(socket_name);
      };
    eval(str);
  };

})();

