(function(){

   if (typeof JSROOT != "object") {
      var e1 = new Error("go4.js requires JSROOT to be already loaded");
      e1.source = "go4.js";
      throw e1;
   }

   if (typeof DABC != "object") {
      var e1 = new Error("go4.js requires DABC to be already loaded");
      e1.source = "go4.js";
      throw e1;
   }

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
         
         xreq = JSROOT.NewHttpRequest(itemname+"/get.json", 'object', function(res) {
            xreq = null;
            if (res==null) return;
            
            for (var i in res._childs) {
               var value = res._childs[i].value;
               var name = res._childs[i]._name;
               if (name == 'State') 
                  $('#'+divid + " .event_rate").css('background-color',  value=='Stopped' ? 'red' : 'lightgreen');
               if (name == 'EventRate') $('#'+divid + " .event_rate").text(value);
               if (name == 'AverRate') $('#'+divid + " .aver_rate").text(value); 
               if (name == 'RunTime') $('#'+divid + " .run_time").text(value); 
               if (name == 'EventCount') $('#'+divid + " .total_events").text(value);
            }
            
         });
         
         xreq.send(null);
      }
       
      setInterval(UpdateStatus, 2000);
   }

})();
