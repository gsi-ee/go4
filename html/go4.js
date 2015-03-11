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
   
   // ===========================================================================
   
   
      // it is important to run this function at the end when all other
   // functions are available
   (function() {
      var scripts = document.getElementsByTagName('script');

      for (var n in scripts) {
         if (scripts[n]['type'] != 'text/javascript') continue;

         var src = scripts[n]['src'];
         if ((src == null) || (src.length == 0)) continue;

         var pos = src.indexOf("scripts/JSRootCore.");
         if (pos<0) continue;

         JSROOT.source_dir = src.substr(0, pos);
         JSROOT.source_min = src.indexOf("scripts/JSRootCore.min.js")>=0;

         console.log("Set JSROOT.source_dir to " + JSROOT.source_dir);

         if (JSROOT.GetUrlOption('gui', src)!=null) {
            window.onload = function() { JSROOT.BuildSimpleGUI(); }
            return;
         }

         var prereq = "";
         if (JSROOT.GetUrlOption('io', src)!=null) prereq += "io;";
         if (JSROOT.GetUrlOption('2d', src)!=null) prereq += "2d;";
         if (JSROOT.GetUrlOption('jq2d', src)!=null) prereq += "jq2d;";
         if (JSROOT.GetUrlOption('3d', src)!=null) prereq += "3d;";
         if (JSROOT.GetUrlOption('mathjax', src)!=null) prereq += "mathjax;";
         var user = JSROOT.GetUrlOption('load', src);
         if ((user!=null) && (user.length>0)) prereq += "load:" + user;
         var onload = JSROOT.GetUrlOption('onload', src);

         if ((prereq.length>0) || (onload!=null))
            window.onload = function() {
              if (prereq.length>0) JSROOT.AssertPrerequisites(prereq, onload); else
              if (onload!=null) {
                 onload = JSROOT.findFunction(onload);
                 if (typeof onload == 'function') onload();
              }
         }

         return;
      }
   })();

   

})();
