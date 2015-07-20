(function(){

   if (typeof JSROOT != "object") {
      var e1 = new Error("go4.js requires JSROOT to be already loaded");
      e1.source = "go4.js";
      throw e1;
   }

   GO4 = {};

   GO4.version = "4.9.8";
   
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
      
      JSROOT.Painter.AdjustLayout(null, $('#'+divid).height()+10, false);
      
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
      if (item==null) return;
      
      if (obj==null) {
         delete item['last-id'];
         return;
      } 
      // ignore all other classes   
      if (obj['_typename'] != 'TList') return;
         
      obj['_typename'] = "TGo4MsgList";
      
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
         main.append("pre").html(this.lst.arr[i].fString);
      
      // (re) set painter to first child element
      this.SetDivId(this.divid);

   }
   
   GO4.DrawMsgList = function(divid, lst, opt) {
      var painter = new GO4.MsgListPainter(lst);
      painter.SetDivId(divid);
      painter.Draw();
      return painter.DrawingReady();
   }
   
   // ============================================================================== 
   
   JSROOT.addDrawFunc("TGo4WinCond", { script: GO4.source_dir + 'html/condition.js', func: 'GO4.drawGo4Cond' }, ";editor");
   JSROOT.addDrawFunc("TGo4PolyCond", { script: GO4.source_dir + 'html/condition.js', func: 'GO4.drawGo4Cond' }, ";editor");
   JSROOT.addDrawFunc("TGo4ShapedCond", { script: GO4.source_dir + 'html/condition.js', func: 'GO4.drawGo4Cond' }, ";editor");
   
   JSROOT.addDrawFunc("TGo4AnalysisWebStatus", { script: GO4.source_dir + 'html/analysiseditor.js', func: 'GO4.drawGo4AnalysisStatus' }, "editor");

   JSROOT.addDrawFunc("TGo4MsgList", GO4.DrawMsgList, "");

})();
