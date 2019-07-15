(function(){

   // this is code for web canvas to support Go4 classes like
   // TGo4Marker or TGo4Condition in the go4 gui
   // it is slightly different to go4.js which is dedicated with usage of THttpServer

   if (typeof JSROOT != "object") {
      var e1 = new Error("go4canvas.js requires JSROOT to be already loaded");
      e1.source = "go4canvas.js";
      throw e1;
   }

   var GO4 = {};

   GO4.version = "5.3.x";


   GO4.MarkerPainter = function(marker) {
      JSROOT.TObjectPainter.call(this, marker);
      this.pave = null; // drawing of stat
   }

   GO4.MarkerPainter.prototype = Object.create(JSROOT.TObjectPainter.prototype);

   GO4.MarkerPainter.prototype.drawMarker = function() {
      var marker = this.GetObject(),
          att = new JSROOT.TAttMarkerHandler(marker),
          isndc = false;

      //  create svg:g container for box drawing
      this.CreateG();

      var x = this.AxisToSvg("x", marker.fX, isndc),
          y = this.AxisToSvg("y", marker.fY, isndc),
          path = att.create(x,y);

      console.log("Drawing marker");

      if (path)
          this.draw_g.append("svg:path")
             .attr("d", path)
             .call(att.func);
   }

   GO4.MarkerPainter.prototype.drawLabel = function() {
   }

   GO4.MarkerPainter.prototype.RedrawObject = function(obj) {
      if (this.UpdateObject(obj))
         this.Redraw(); // no need to redraw complete pad
   }

   GO4.MarkerPainter.prototype.Redraw = function() {
      this.drawMarker();
      this.drawLabel();
   }


   GO4.drawGo4Marker = function(divid, obj, option) {
      var painter = new GO4.MarkerPainter(obj);
      painter.SetDivId(divid);
      painter.drawMarker();
      painter.drawLabel();
      return painter.DrawingReady();
   }

   JSROOT.addDrawFunc("TGo4Marker", GO4.drawGo4Marker, "");

})();
