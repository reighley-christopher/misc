    $.Event.prototype.special_offsetX = function(){return (this.offsetX || this.pageX - $("#container").offset().left || this.originalEvent.touches[0].pageX- $("#container").offset().left)} 
    $.Event.prototype.special_offsetY = function(){return (this.offsetY || this.pageY - $("#container").offset().top || this.originalEvent.touches[0].pageY- $("#container").offset().left)}

    TouchManage = Backbone.View.extend({
     events : 
       {
       "mouseup" : "mouseup",
       "mousedown" : "mousedown",
       "touchstart" : "touchstart",
       "touchend" : "touchend",
       "touchmove" : "touchmove"
       },
       touchstart : function(e) {
          if(e.originalEvent.touches.length == 2)
            {
            var dx = e.originalEvent.touches[0].pageX - 
                      e.originalEvent.touches[1].pageX;
            var dy = e.originalEvent.touches[0].pageY - 
                      e.originalEvent.touches[1].pageY;
            this.scalingfactor = Math.sqrt(dx*dx + dy*dy);
            this.aspecialX = (e.originalEvent.touches[0].pageX + 
                           e.originalEvent.touches[1].pageX)/2;
            this.aspecialY = (e.originalEvent.touches[0].pageY + 
                           e.originalEvent.touches[1].pag/eY)/2;
            this.touch_el = null; /*one finger hit the screen first, but that doesn't make this a click*/
            } else {
            this.aspecialX = e.originalEvent.changedTouches[0].pageX;
            this.aspecialY =  e.originalEvent.changedTouches[0].pageY;
            this.touch_el = document.elementFromPoint(this.aspecialX , this.aspecialY);
            /*ERR(el().tagName + " " + el().tagName);*/
            }
       },
       touchend : function(e) {
          var pageX = e.originalEvent.changedTouches[0].pageX;
          var pageY = e.originalEvent.changedTouches[0].pageY;
          if(e.originalEvent.touches.length + e.originalEvent.changedTouches.length == 2) {
                if(this.translate_and_scale)
                  {
                  var touches = _.select([
                                  e.originalEvent.touches[0],
                                  e.originalEvent.changedTouches[0],
                                  e.originalEvent.touches[1],
                                  e.originalEvent.changedTouches[1]
                                  ], function(x){return x} );
                   var dx = touches[0].pageX - 
                            touches[1].pageX;
                   var dy = touches[0].pageY - 
                            touches[1].pageY;
                   var new_scalingfactor = Math.sqrt(dx*dx + dy*dy);
                  var new_aspecialX = (touches[0].pageX + 
                                        touches[1].pageX)/2;
                   var new_aspecialY = (touches[0].pageY + 
                                        touches[1].pageY)/2;
                  this.translate_and_scale(new_aspecialX - this.aspecialX,                                           new_aspecialY - this.aspecialY,
                                           new_scalingfactor/this.scalingfactor
                                           );
                  }
                } else {
          var el = document.elementFromPoint(pageX , pageY);
          if(el == this.touch_el && this.touch_click) 
            {
            ERR("touch_click");
            this.touch_click(pageX, pageY);
            } else if(this.touch_drag) {
            ERR("touch_drag");
            this.touch_drag(pageX, pageY);
            }
          }
       return false;
       },
       touchmove : function(e){
          e.preventDefault(); 
       },
       mouseup : function(e){
          if('ontouchstart' in window)
            {
            ERR("mouseup prevent");
            } else {
            ERR("mouseup");
            this.drop(e);
            }
       },
       mousedown : function(e){
          if('ontouchstart' in window)
            {
            ERR("mousedown prevent");
            } else {
            ERR("mousedown");
            this.createDialog(e);
            }
       }
    });

    Diagram = function(data)
      {
      this.toggle_link = function(a, b)
        {
        var c = data.connect;
        var kill_me =  _.select(c, function(cc){
          return (cc.from == a && cc.to == b) 
          });
        if(kill_me.length) {data.connect = _.difference(data.connect, kill_me); return false;}
        else {data.connect.push({from: a, to: b}); return true;}
        }
      this.remove_node = function(a)
        {
        if(data.processes.hasOwnProperty(a)) {delete data.processes[a]}
        if(data.inputs.hasOwnProperty(a)) {delete data.inputs[a]}
        if(data.outputs.hasOwnProperty(a)) {delete data.outputs[a]}

        var c = data.connect;
        var kill_me =  _.select(c, function(cc){
          return (cc.from == a || cc.to == a) 
          });
        if(kill_me.length) {
          var xxx = _.difference(data.connect, kill_me); 
          data.connect = xxx;
          }
        }
      this.find_node = function(a)
        {
        if(data.processes.hasOwnProperty(a)) {return data.processes[a]}
        if(data.inputs.hasOwnProperty(a)) {return data.inputs[a]}
        if(data.outputs.hasOwnProperty(a)) {return data.outputs[a]}
        }
      }

    SVGBase = TouchManage.extend({
       initialize: function(opts)
          {
          this.data = opts.data;
          /*
          this.editBox = new EditBox();
          this.editBox.$el.attr("class", "editbox");
          this.$el.append(this.editBox.$el);
          this.editBox.$el.show();
          */
          },
       events : 
          {
          "mousedown polygon" : "click", 
          "mousedown circle#out" : "start_join",
          "mousedown circle#in" : "end_join", 
          "mouseup polygon" : "activate",
          "touchstart" : "touchstart",
          "touchend" : "touchend",
          },
      click : function()
          {
          this.trigger("grab");
          return false;
          },
       start_join : function()
          {
          this.trigger("start_join");
          return false;
          },
       end_join : function()
          {
          this.trigger("end_join");
          return false;
          },
       activate : function()
          {
          this.trigger("edit", this);
          return false;
          },
       el : function() 
          {
          var ret = $("svg defs " + this.shape_def).clone();
          ret.id = null;
          return ret;
          },
       move : function(x, y)
         {
         this.data.coordinates[0] = x;
         this.data.coordinates[1] = y;
         this.trigger("move");
         this.render();
         },
       render : function()
         {
         this.$el.attr("id", this.id);
         this.$el.attr("transform", "translate("+this.data.coordinates[0]+","+ this.data.coordinates[1]+")");
         },
       destroy : function()
         {
         this.$el.remove();
         this.trigger("destroy");
         },
       reconfigure : function(taskName, catalog)
         {
         if(taskName == ""){ return };
         if(!this.data.task){this.data.task = {}};
         if(!this.data.task.parameters){this.data.task.parameters = {}};
         this.data.task.name = taskName;
         var parameters = catalog[taskName].schema.parameters;
         for(v in parameters){console.log(v); this.data.task.parameters[parameters[v]] = this.data.task.parameters[parameters[v]]||null};
         },
       touch_click : function()
         {
         this.trigger("edit", this);
         },
       touch_drag : function(x, y)
         {
         var el = document.elementFromPoint(x , y);
         if(el.tagName == "svg") 
           {
           var scale = container.current_scale;
           var offset_x = container.current_x;
           var offset_y = container.current_y;
           this.move((x/scale)-offset_x,(y/scale)-offset_y);
           } else {
           /*have to use javascript to find parent, sad!*/
           var target = el;
           while(target && target.tagName != "g") 
             {target = target.parentElement};
           /*TODO do this with event handlers and not direct references to container*/
           var other = container.index[target.id];
           container.start_join = this;
           container.end_join = other;
           container.link();
           }
         }
    });

    PipeLine = Backbone.View.extend({
      initialize : function(opts)
         {
         this.from = opts.from;
         this.to = opts.to;
         this.from.on('move', this.render, this);
         this.to.on('move', this.render, this);
         this.from.on('destroy', function(){this.destroy()}, this );
         this.to.on('destroy', function(){this.destroy()}, this ); 
         },
      el : function()
         {
         var ret =  document.createElementNS("http://www.w3.org/2000/svg", "g"); 
         var path = document.createElementNS("http://www.w3.org/2000/svg", "path");
         $(path).attr("stroke-width", "0.1")
         $(ret).append(path);
         return ret;
         },
      setEnds : function(x, raw_y, output_right, output_y, input_left, input_y)
         {
         /*cases to be handled:
         the vertical difference is 0
         the vertical difference is >0 but <0.8
         the left element is above the right element
         the right element is above the left element
         the horizontal distance is small
         */
         var marker_size = 0.4;
         var y = raw_y+input_y-output_y; 
         if( y == 0 ) 
           {
           this.$("path").attr("d", "M " + output_right + " 1 H "+ (x + input_left - marker_size )).attr("marker-end", "url(#Triangle)");;
           } else if(Math.abs(y) > 1) {
           var turn1 = (y > 0) ? " a 0.4 0.4 0 0 1 0.4 0.4 " : " a 0.4 0.4 0 0 0 0.4 -0.4 " ;
           var turn2 = (y > 0) ? " a 0.4 0.4 0 0 0 0.4 0.4 " : " a 0.4 0.4 0 0 1 0.4 -0.4 " ;
           var vert = ( y > 0 ) ? y+0.6 : y+1.4;
           this.$("path").attr("d", "M " + output_right + " " + output_y + " H " +x/2.0+ turn1 + "V "+vert+ turn2 + 
                               "H "+(x + input_left - marker_size )).attr("marker-end", "url(#Triangle)");
           } else {
           this.$("path").attr("d", "M " + output_right + " " + output_y + 
                              " c "+ (x/2.0) +" 0 "+(x/2.0)+" "+ y +" "+(x-marker_size - output_right - 0.2)+" "+y
                              ).attr("marker-end", "url(#Triangle)");
           }
         },
      destroy : function()
        {
        this.$el.remove();
        this.trigger('destroy');
        },
      render : function()
        {
        var out_x = Number(this.from.$el.find("#out").attr("cx"));
        var out_y = Number(this.from.$el.find("#out").attr("cy"));
        var in_x = Number(this.to.$el.find("#in").attr("cx"));
        var in_y = Number(this.to.$el.find("#in").attr("cy"));
        var out_r = Number(this.from.$el.find("#out").attr("r"));
        var in_r = Number(this.to.$el.find("#in").attr("r"));
        var marker_size = 0.4;
        this.$el.attr("transform", "translate("+this.from.data.coordinates[0]+","+this.from.data.coordinates[1]+")");
         var input_left = in_x-in_r;
         var output_right = out_x+out_r;
         var input_y = in_y;
         var output_y = out_y;
 
        this.setEnds(this.to.data.coordinates[0] - this.from.data.coordinates[0], 
                     this.to.data.coordinates[1] - this.from.data.coordinates[1],
                     output_right, output_y, input_left, input_y
                    );
        }
    });
    
    Container = TouchManage.extend({
     initialize : function(opts) {
       this.data = opts.data;
       this.helper = new Diagram(opts.data);
       this.index = {};
       this.menu = new Menu({el : this.$("#main_menu")});
       this.edit = new EditBox({el : this.$(".editbox")});
       this.menu.on("create", this.create, this);
       this.current_scale = 20;
       this.current_x = 0;
       this.current_y = 0;
       this.factories = 
          { 
          InputTriangle : SVGBase.extend({
           shape_def : "#input_triangle",
           data_def : "inputs"
          }),

          ProcessRectangle : SVGBase.extend({
            shape_def : "#process_rectangle",
            data_def : "processes"
          }),

          OutputTriangle : SVGBase.extend({
            shape_def : "#output_triangle",
            data_def : "outputs"
          })
          } 
       },
    render : function(){
      var data = this.data;
      var svgdoc = this.$('svg #main');
      for(i in data.inputs)
        {
        var t = new this.factories["InputTriangle"]({id : i, data : data.inputs[i]});
        this.add(t);
        t.render();
        }

      for(i in data.outputs)
        {
        var t = new this.factories["OutputTriangle"]({id : i, data : data.outputs[i]});
        this.add(t);
        t.render();
        }
      
      for(i in data.processes)
        {
        var t = new this.factories["ProcessRectangle"]({id : i, data : data.processes[i]});
        this.add(t);
        t.render();
        }

      for(c in data.connect)
        {
        var to = this.index[data.connect[c].to];
        var from = this.index[data.connect[c].from];
        this.connect(from, to);
        }
    },
    newId : function() {
      var merge =  _.union( Object.keys(this.data.inputs),
                  Object.keys(this.data.outputs), 
                  Object.keys(this.data.processes));
      var count = merge.length;
      while(true) {
        if( !(_.include(merge, "i" + count)) ) { return "i" + count }
        count = count + 1;  
      }
    },
    add : function( unit ){
      this.$('svg #main').append(unit.$el);
      this.index[unit.id] = unit;
      unit.on("grab", function(){ this.moving = unit; }, this);
      unit.on("start_join", function() { this.start_join = unit; this.link() }, this );
      unit.on("end_join", function() {this.end_join = unit; this.link() }, this );
      unit.on("edit", function(e){ this.moving = null; this.edit.open(e)}, this );
      unit.on("destroy", function(){ this.helper.remove_node(unit.id) }, this );
    },
    create : function( name, x, y){
      var klass = this.factories[name];
      var data =  { coordinates : [Math.floor(x/20),  Math.floor(y/20)]};
      var id = this.newId();
      obj = new klass({id : id, data : data});
      this.data[obj.data_def][id] = data;
      this.add(obj);
      obj.render();
    },
    link : function() {
      if( !this.end_join || !this.start_join ) { return };
      var result = this.helper.toggle_link(this.start_join.id, this.end_join.id);
      if(result) {
               this.connect(this.start_join, this.end_join);
                } else {
                this.disconnect( this.start_join, this.end_join ); 
                }
      this.start_join = null;
      this.end_join = null;
    },
    connect : function( from, to ) {
      var data = {from : from, to : to};
      var p = new PipeLine(data);
      this.$('svg #main').append(p.$el);
      this.index[[from.id, to.id]] = p;
      p.render();
    },
    disconnect : function( from, to ) {
      this.index[[from.id, to.id]].$el.remove();
    },
    drop : function(e){
      var new_x = Math.floor(e.special_offsetX()/20); 
      var new_y = Math.floor(e.special_offsetY()/20);
      if( this.moving ) { this.moving.move(new_x, new_y); }
      this.menu.inactivate();
      this.moving = null;
    },
    createDialog : function(e){
      ERR("createDialog");
      this.menu.activate(e.special_offsetX(), e.special_offsetY());
    },
    translate_and_scale : function(x, y, s){
      ERR("tranlate_and_scale");
      this.current_x += x/this.current_scale;
      this.current_y += y/this.current_scale;
      this.current_scale *= s;
      this.$("svg #main").attr("transform", 
             "scale(" + this.current_scale + ") " +
             "translate(" + this.current_x + "," + this.current_y + ")");
    },
    touch_click : function(x, y) {
      this.menu.activate(x, y);
    } 
    });
    
    EditBox = TouchManage.extend({
       events : {
          "mousedown .close" : function(){return false},
          "click .close" : "close",
          "change .task" : "update"
          },
       initialize : function(){
          var e = this.$el.find('.task');
          var self = this;
          $.get("/run/catalog", "", function(d){ console.log(d); self.catalog=d; _.each(d, function(val, key){e.append($("<option></option>").val(key).text(key)) } ) }, "json") 
          },
       open : function(e)
         {
         this.target = e;
         this.$el.find('.task').val(e.data.task?e.data.task.name:"");
         this.reconfigure();
         this.$el.show();
         ERR(e.el.tagName + e.el.id + JSON.stringify(e.$el.offset()));
         this.$el.offset(e.$el.offset());
         },
       reconfigure : function()
         {
         var value = this.$el.find('.task').val(); 
         if(!this.catalog[value]) {this.$el.find(".content").html(""); return}
         this.target.reconfigure(value, this.catalog);
         if(this.catalog[value].schema.template) {
         this.$el.find(".content").html(_.template(this.catalog[value].schema.template)(this.target.data.task.parameters)); } else {
         this.$el.find(".content").html("INTENTIONALLY BLANK");
         }
         var parameters = this.catalog[value].schema.parameters; 
         for( v in parameters ) {
           var z = this.target.data.task.parameters;
           var e =  this.$el.find(".content [name="+parameters[v]+"]");
           e.on("change", function(){z[parameters[v]] = e.val()});
           }
         },
       close : function()
         {
         this.$el.hide();
         return false;
         },
       update : function(e)
         {
         if(e.currentTarget.value == "DELETE") 
           {
           this.target.destroy();
           return false;
           } else {
           this.target.reconfigure(e.currentTarget.value, this.catalog)
           this.reconfigure();
           return false;
           }
         }
       });

    Menu = TouchManage.extend({
      initialize : function() {
        this.$("use").on("mouseup", this, this.select );
        this.$("use").on("touchend", this, this.touch_select );
        this.$el.hide();
        },
      touch_possible : function(e)
        {
        ERR(this.$el.position().top)
        _.each(this.$("use"), function(u){
          ERR(u.children.length);
          });
        },
      select : function(e)
        {
        e.data.trigger("create", e.currentTarget.id, e.special_offsetX(), e.special_offsetY());
        e.data.inactivate();
        return false;
        },
      touch_select : function(e)
        {
        var x = e.originalEvent.changedTouches[0].pageX;
        var y = e.originalEvent.changedTouches[0].pageY;
        e.data.trigger("create", e.currentTarget.id, x, y);
        e.data.inactivate();
        return false;
        },
      activate : function(x, y)
        {
        this.$el.attr("transform", "translate("+x+","+y+") scale(20)");
        this.$el.show();
        },
      inactivate : function()
        {
        this.$el.hide();
        },
      drop : function()
       {
       /*STUB*/
       ERR("POW");
       },
      append : function(svgdoc) 
       {
       var svg$ = $(svgdoc);
       var children =  svg$.find('g');
       var self = this;
       children.each(function(c)
         {
         var fact_key = $(children[c]).attr("id");
         var shape_def = "#" + fact_key;
         container.factories[fact_key] = SVGBase.extend({
           shape_def : shape_def,
           data_def : "processes"
         });


         $('#c3d defs').append(children[c]);
         var use = $(document.createElementNS("http://www.w3.org/2000/svg", "svg:use"));
         use.attr("id", fact_key).
             attr("href", shape_def).
             attr("x", "1").
             attr("y", 8.0+(c*2.5))
         /*$('#c3d #main_menu').append(children[1]);*/
         self.$el.append(use);
         use.on("use").on("mouseup", self, self.select );
         use.on("touchend", self, self.touch_select );
         });
       }
    });
 
