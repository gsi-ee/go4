(function(){

   if (typeof JSROOT != "object") {
      var e1 = new Error("go4.js requires JSROOT to be already loaded");
      e1.source = "go4.js";
      throw e1;
   }

   GO4 = {};

   GO4.version = "4.9.5";
   
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
      html += "<img class='go4_logo' style='vertical-align:middle;' src='go4sys/icons/go4logorun4.gif' alt='logo'></img>";
      html += "<label class='event_source' style='border: 1px solid gray; font-size:large; vertical-align:middle;'>file.lmd</label> ";
      html += "<label class='event_rate' style='border: 1px solid gray; font-size:large; vertical-align:middle; background-color: grey'>---</label> Ev/s ";
      html += "<label class='aver_rate' style='border: 1px solid gray; font-size:large; vertical-align:middle'>---</label> Ev/s "; 
      html += "<label class='run_time' style='border: 1px solid gray; font-size:large; vertical-align:middle'>---</label> s "; 
      html += "<label class='total_events' style='border: 1px solid gray; font-size:large; vertical-align:middle'>---</label> Events ";
      html += "<label class='analysis_time' style='border: 1px solid gray; font-size:large; vertical-align:middle;'>file.lmd</label>";
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
            if (res==null) return;
            
            $('#'+divid + " .event_rate").css('background-color', res.fbRunning ? 'lightgreen' : 'red');

            if (was_running != res.fbRunning)
               $('#'+divid + " .go4_logo").attr("src", res.fbRunning ? 'go4sys/icons/go4logorun4.gif' : 'go4sys/icons/go4logo_t.png');
            
            was_running = res.fbRunning;
            
            $('#'+divid + " .event_source").text(res.fxEventSource);
            $('#'+divid + " .event_rate").text(res.fdRate.toFixed(1));
            $('#'+divid + " .aver_rate").text((res.fdTime > 0 ? res.fuCurrentCount / res.fdTime : 0).toFixed(1)); 
            $('#'+divid + " .run_time").text(res.fdTime.toFixed(1)); 
            $('#'+divid + " .total_events").text(res.fuCurrentCount);
            $('#'+divid + " .analysis_time").text(res.fxDateString);
         });
         
         xreq.send(null);
      }
       
      setInterval(UpdateRatemeter, 2000);
   }

   
   // ============================================================================== 
   
   JSROOT.addDrawFunc("TGo4WinCond", { script: GO4.source_dir + 'html/condition.js', func: 'GO4.drawGo4Cond' }, ";editor");
   JSROOT.addDrawFunc("TGo4PolyCond", { script: GO4.source_dir + 'html/condition.js', func: 'GO4.drawGo4Cond' }, ";editor");
   JSROOT.addDrawFunc("TGo4ShapedCond", { script: GO4.source_dir + 'html/condition.js', func: 'GO4.drawGo4Cond' }, ";editor");
   
   JSROOT.addDrawFunc("TGo4AnalysisWebStatus", { script: GO4.source_dir + 'html/analysiseditor.js', func: 'GO4.drawGo4AnalysisStatus' }, "editor");



})();
