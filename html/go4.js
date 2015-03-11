(function(){

   if (typeof JSROOT != "object") {
      var e1 = new Error("go4.js requires JSROOT to be already loaded");
      e1.source = "go4.js";
      throw e1;
   }

   if (typeof DABC == "object") {
      var e1 = new Error("go4.js DO NOT requires dabc.js");
      e1.source = "go4.js";
      throw e1;
   }
   
   DABC = { hpainter: JSROOT.hpainter };

   GO4 = {};

   GO4.version = "4.7.1";
   
   
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

})();
