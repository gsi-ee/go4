(function(){

   if (typeof JSROOT != "object") {
      var e1 = new Error("go4.js requires JSROOT to be already loaded");
      e1.source = "go4.js";
      throw e1;
   }

   GO4 = {};

   GO4.version = "5.1.1";
   
   // use location to load all other scripts when required
   GO4.source_dir = function() {
      var scripts = document.getElementsByTagName('script');

      for (var n in scripts) {
         if (scripts[n]['type'] != 'text/javascript') continue;

         var src = scripts[n]['src'];
         if ((src == null) || (src.length == 0)) continue;

         var pos = src.indexOf("html/go4.js");
         if (pos<0) continue;
         if (src.indexOf("JSRootCore")>=0) continue;
         console.log('Set GO4.source_dir to ' + src.substr(0, pos));
         return src.substr(0, pos);
      }
      return "";
   }(); 
   
   
   // ==================================================================================
    
   GO4.DrawAnalysisRatemeter = function(divid, itemname) {
      
      var html = "<div style='padding-top:2px'>";
      html += "<img class='go4_logo' style='vertical-align:middle;margin-left:5px;margin-right:5px;' src='go4sys/icons/go4logorun4.gif' alt='logo'></img>";
      html += "<label class='event_source' style='border: 1px solid gray; font-size:large; vertical-align:middle; padding-left:3px; padding-right:3px;'>file.lmd</label> ";
      html += "<label class='event_rate' style='border: 1px solid gray; font-size:large; vertical-align:middle; background-color: grey'; padding-left:3px; padding-right:3px;>---</label> Ev/s ";
      html += "<label class='aver_rate' style='border: 1px solid gray; font-size:large; vertical-align:middle; padding-left:3px; padding-right:3px;'>---</label> Ev/s "; 
      html += "<label class='run_time' style='border: 1px solid gray; font-size:large; vertical-align:middle; padding-left:3px; padding-right:3px;'>---</label> s "; 
      html += "<label class='total_events' style='border: 1px solid gray; font-size:large; vertical-align:middle; padding-left:3px; padding-right:3px;'>---</label> Events ";
      html += "<label class='analysis_time' style='border: 1px solid gray; font-size:large; vertical-align:middle; padding-left:3px; padding-right:3px;'>time</label>";
      html += "</div>";
      
      $('#'+divid).css('overflow','hidden')
                  .css('padding-left','5px')
                  .css('display', 'inline-block')
                  .css('white-space', 'nowrap')
                  .html(html);
      
      // use height of child element 
      JSROOT.Painter.AdjustLayout(null, $('#'+divid+' div').height()+4, false);
      
      var xreq = null;
      var was_running = null;
      
      function UpdateRatemeter() {
         if (xreq!=null) return;
         
         xreq = JSROOT.NewHttpRequest(itemname+"/root.json.gz", 'object', function(res) {
            xreq = null;
            
            if (res==null) 
               return $('#'+divid + " .event_rate").css('background-color','grey');
            
            $('#'+divid + " .event_rate").css('background-color', res.fbRunning ? 'lightgreen' : 'red');
            if (was_running != res.fbRunning)
               $('#'+divid + " .go4_logo").attr("src", res.fbRunning ? 'go4sys/icons/go4logorun4.gif' : 'go4sys/icons/go4logo_t.png');
            was_running = res.fbRunning;
            
            $('#'+divid + " .event_source").text(res.fxEventSource == "" ? "<source>" : res.fxEventSource);
            $('#'+divid + " .event_rate").text(res.fdRate.toFixed(1));
            $('#'+divid + " .aver_rate").text((res.fdTime > 0 ? res.fuCurrentCount / res.fdTime : 0).toFixed(1)); 
            $('#'+divid + " .run_time").text(res.fdTime.toFixed(1)); 
            $('#'+divid + " .total_events").text(res.fuCurrentCount);
            $('#'+divid + " .analysis_time").text(res.fxDateString == "" ? "<datime>" : res.fxDateString);
         });
         
         xreq.send(null);
      }
       
      setInterval(UpdateRatemeter, 2000);
   }

   
   GO4.MakeMsgListRequest = function(hitem, item) {
      var arg = "&max=2000";
      if ('last-id' in item) arg+= "&id="+item['last-id'];
      return 'exe.json.gz?method=Select' + arg;      
   }
   
   GO4.AfterMsgListRequest = function(hitem, item, obj) {
      if (!item) return;
      
      if (!obj) {
         delete item['last-id'];
         return;
      } 
      // ignore all other classes   
      if (obj._typename != 'TList') return;
         
      obj._typename = "TGo4MsgList";
      
      if (obj.arr.length>0) {
         var duplicate = (('last-id' in item) && (item['last-id'] == obj.arr[0].fString));
               
         item['last-id'] = obj.arr[0].fString;

         // workaround for snapshot, it sends always same messages many times 
         if (duplicate) obj.arr.length = 1;

         // add clear function for item
         if (!('clear' in item)) 
            item['clear'] = function() { delete this['last-id']; }
      }
   }
   
   
   GO4.MsgListPainter = function(lst) {
      JSROOT.TBasePainter.call(this);
      
      this.lst = lst;
         
      return this;
   }

   GO4.MsgListPainter.prototype = Object.create( JSROOT.TBasePainter.prototype );

   GO4.MsgListPainter.prototype.RedrawObject = function(obj) {
      this.lst = obj;
      this.Draw();
      return true;
   }

   GO4.MsgListPainter.prototype.Draw = function() {
      if (this.lst==null) return;
      
      var frame = d3.select("#" + this.divid);
      
      var main = frame.select("div");
      if (main.empty()) 
         main = frame.append("div")
                     .style('max-width','100%')
                     .style('max-height','100%')
                     .style('overflow','auto');
      
      var old = main.selectAll("pre");
      var newsize = old.size() + this.lst.arr.length - 1; 

      // in the browser keep maximum 2000 entries
      if (newsize > 2000) 
         old.select(function(d,i) { return i < newsize - 2000 ? this : null; }).remove();

      for (var i=this.lst.arr.length-1;i>0;i--)
         main.append("pre").style('margin','3px').html(this.lst.arr[i].fString);
      
      // (re) set painter to first child element
      this.SetDivId(this.divid);

   }
   
   GO4.DrawMsgList = function(divid, lst, opt) {
      var painter = new GO4.MsgListPainter(lst);
      painter.SetDivId(divid);
      painter.Draw();
      return painter.DrawingReady();
   }
   
   GO4.drawAnalysisTerminal = function(hpainter, itemname) {
      var url = hpainter.GetOnlineItemUrl(itemname);
      if (url == null) return null;
      var frame = hpainter.GetDisplay().FindFrame(itemname, true);
      if (frame==null) return null;
      var divid = d3.select(frame).attr('id');
      var h = $("#"+divid).height(), w = $("#"+divid).width();
      if ((h<10) && (w>10)) $("#"+divid).height(w*0.7);
      
      var player = new JSROOT.TBasePainter();
      player.url = url;
      player.hpainter = hpainter;
      player.itemname = itemname;
      player.draw_ready = true; 
      player.needscroll = false;
      
      
      player.DrawReady = function() {
     	  if(this.needscroll) {
     		  this.ClickScroll();    	  
     		  this.needscroll=false;
     	  }
    	  this.draw_ready = true;
     	  
      }
      
      player.ProcessTimer = function() {
         //var subid = this.divid + "_terminal";
    	  var subid ="anaterm_output_container";
         if ($("#" + subid).length==0) {
            // detect if drawing disappear 
            clearInterval(this.interval);
            this.interval = null;
            return;
         }
         if (!this.draw_ready) return;
         
         var msgitem = this.itemname.replace("Control/Terminal", "Status/Log");
         
         this.draw_ready = false;

         this.hpainter.display(msgitem,"divid:" + subid, this.DrawReady.bind(this));
      }
      
      player.ClickCommand = function(kind) {
    	 var pthis=this;
         this.hpainter.ExecuteCommand(this.itemname.replace("Terminal", "CmdExecute"), function(){pthis.needscroll=true}, kind);
         //console.log("Execute ClickCommand with" + kind);
         
      }
      
      player.ClickClear = function() {
    	  //console.log("Clear terminal");
    	  document.getElementById("anaterm_output_container").firstChild.innerHTML="";
      }
    
      player.ClickScroll = function() {
    	  //console.log("ScrollEndTerminal");	
    	  var disp = $("#anaterm_output_container").children(":first"); // inner frame created by hpainter has the scrollbars, i.e. first child
    	  disp.scrollTop(disp[0].scrollHeight - disp.height());      
      }
      
      
      player.fillDisplay = function() {
    	  
          var id = "#"+this.divid;
          var pthis=this;
          //console.log("fillDisplay for id - " + id);
          this.interval = setInterval(this.ProcessTimer.bind(this), 2000);
          
          $(id + " .go4_clearterm")
        	.button({text: false, icons: { primary: "ui-icon-blank MyTermButtonStyle"}})
        	.click(this.ClickClear.bind(this))
            .children(":first") // select first button element, used for images
            .css('background-image', "url(" + GO4.source_dir + "icons/clear.png)");
        	
          
          $(id + " .go4_endterm")
          	.button({text: false, icons: { primary: "ui-icon-blank MyTermButtonStyle"}})
          	.click(this.ClickScroll.bind(this))
            .children(":first") // select first button element, used for images
            .css('background-image', "url(" + GO4.source_dir + "icons/shiftdown.png)");
          	
          
          
          $(id + " .go4_printhistos")
          	.button({text: false, icons: { primary: "ui-icon-blank MyTermButtonStyle"}})
          	.click(this.ClickCommand.bind(this,"@PrintHistograms()"))
            .children(":first") // select first button element, used for images
            .css('background-image', "url(" + GO4.source_dir + "icons/hislist.png)");

          $(id + " .go4_printcond").button()
          	.button({text: false, icons: { primary: "ui-icon-blank MyTermButtonStyle"}})
          	.click(this.ClickCommand.bind(this,"@PrintConditions()"))
            .children(":first") // select first button element, used for images
            .css('background-image', "url(" + GO4.source_dir + "icons/condlist.png)");
          
          
          $("#go4_anaterm_cmd_form").submit(
                  function(event) {
                	 var command= pthis.itemname.replace("Terminal", "CmdExecute");
                     var cmdpar=document.getElementById("go4_anaterm_command").value;
                     console.log("submit command - " + cmdpar);
                     pthis.hpainter.ExecuteCommand(command,  function(){pthis.needscroll=true}, cmdpar);
                     event.preventDefault(); 
                  });
          
          
          $(id + " .go4_executescript").button()
        	   .button({text: false, icons: { primary: "ui-icon-blank MyTermButtonStyle"}})
            .children(":first") // select first button element, used for images
            .css('background-image', "url(" + GO4.source_dir + "icons/macro_t.png)");
      }
      
      
      player.Show = function(divid) {
    	  var pthis = this;
    	  $("#"+divid).load(GO4.source_dir + "html/terminal.htm", "", 
    	            function() { pthis.SetDivId(divid); pthis.fillDisplay();}
    	  				);

         return this;
      }
      
      player.CheckResize = function(force) {
    	   // console.log("CheckResize..., force=" + force);
      }
      
      return player.Show(divid);
   }

   
   // ============================================================================== 
   
   JSROOT.addDrawFunc("TGo4WinCond", { script: GO4.source_dir + 'html/condition.js', func: 'GO4.drawGo4Cond' }, ";editor");
   JSROOT.addDrawFunc("TGo4PolyCond", { script: GO4.source_dir + 'html/condition.js', func: 'GO4.drawGo4Cond' }, ";editor");
   JSROOT.addDrawFunc("TGo4ShapedCond", { script: GO4.source_dir + 'html/condition.js', func: 'GO4.drawGo4Cond' }, ";editor");
   
   JSROOT.addDrawFunc("TGo4AnalysisWebStatus", { script: GO4.source_dir + 'html/analysiseditor.js', func: 'GO4.drawGo4AnalysisStatus' }, "editor");

   JSROOT.addDrawFunc("TGo4MsgList", GO4.DrawMsgList, "");
   
   JSROOT.addDrawFunc({ name: "TGo4MbsEvent", noinspect: true });
   JSROOT.addDrawFunc({ name: "TGo4EventElement", noinspect: true });

})();
