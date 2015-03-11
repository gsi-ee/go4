(function(){

   if (typeof JSROOT != "object") {
      var e1 = new Error("go4.js requires JSROOT to be already loaded");
      e1.source = "go4.js";
      throw e1;
   }

   GO4 = {};

   GO4.version = "4.7.1";
   
   // use location to load all other scripts when required
   GO4.source_dir = function() {
      var scripts = document.getElementsByTagName('script');

      for (var n in scripts) {
         if (scripts[n]['type'] != 'text/javascript') continue;

         var src = scripts[n]['src'];
         if ((src == null) || (src.length == 0)) continue;

         var pos = src.indexOf("go4.js");
         if (pos<0) continue;
         if (src.indexOf("JSRootCore")>=0) continue;
         console.log('Set GO4.source_dir to ' + src.substr(0, pos));
         return src.substr(0, pos);
      }
      return "";
   }(); 
   
   
     // ==================================================================================
   
   GO4.DrawAnalysisStatus = function(divid, itemname) {
      
      var html = "<div style='padding-top:2px'>";
      html += "<label class='event_rate' style='border: 1px solid gray; font-size:large; vertical-align:middle; background-color: grey'>---</label> Ev/s ";
      html += "<label class='aver_rate' style='border: 1px solid gray; font-size:large; vertical-align:middle'>---</label> Ev/s "; 
      html += "<label class='run_time' style='border: 1px solid gray; font-size:large; vertical-align:middle'>---</label> s "; 
      html += "<label class='total_events' style='border: 1px solid gray; font-size:large; vertical-align:middle'>---</label> Ev";
      html += "</div>";
      
      $('#'+divid).css('overflow','hidden')
                  .css('padding-left','5px')
                  .css('display', 'inline-block')
                  .css('white-space', 'nowrap')
                  .html(html);
      
      var xreq = null;
      
      function UpdateStatus() {
         if (xreq!=null) return;
         
         xreq = JSROOT.NewHttpRequest(itemname+"/item.json", 'object', function(res) {
            xreq = null;
            if (res==null) return;
            
            $('#'+divid + " .event_rate").css('background-color', res.value=='Stopped' ? 'red' : 'lightgreen');
            
            $('#'+divid + " .event_rate").text(res.event_rate);
            $('#'+divid + " .aver_rate").text(res.aver_rate); 
            $('#'+divid + " .run_time").text(res.run_time); 
            $('#'+divid + " .total_events").text(res.event_count);
         });
         
         xreq.send(null);
      }
       
      setInterval(UpdateStatus, 2000);
   }
   
   // ============================================================================== 
   
   JSROOT.addDrawFunc("TGo4WinCond", { script: GO4.source_dir + 'condition.js', func: 'GO4.drawGo4Cond' }, ";editor");
   JSROOT.addDrawFunc("TGo4PolyCond", { script: GO4.source_dir + 'condition.js', func: 'GO4.drawGo4Cond' }, ";editor");
   JSROOT.addDrawFunc("TGo4ShapedCond", { script: GO4.source_dir + 'condition.js', func: 'GO4.drawGo4Cond' }, ";editor");

})();
