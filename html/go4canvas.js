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

   GO4.MarkerPainter.prototype.drawMarker = function(grx, gry) {
      if ((grx !== undefined) && (gry !== undefined)) {

         this.draw_g.selectAll('*').remove();

         this.grx = grx;

         this.gry = gry;

      } else {

         this.CreateG(); // can draw in complete pad

         var marker = this.GetObject();

         this.createAttMarker({ attr: marker });

         console.error('size ' + this.markeratt.GetFullSize());

         this.grx = this.AxisToSvg("x", marker.fX);
         this.gry = this.AxisToSvg("y", marker.fY);
      }

      var path = this.markeratt.create(this.grx,this.gry);

      if (path)
          this.draw_g.append("svg:path")
             .attr("d", path)
             .call(this.markeratt.func);
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

   GO4.MarkerPainter.prototype.ProcessTooltip = function(pnt) {
      var hint = this.ExtractTooltip(pnt);
      if (!pnt || !pnt.disabled) this.ShowTooltip(hint);
      return hint;
   }

   GO4.MarkerPainter.prototype.ExtractTooltip = function(pnt) {
      if (!pnt) return null;

      var marker = this.GetObject();

      var hint = { name: marker.fName,
                   title: marker.fTitle,
                   x: this.grx - (this.frame_x() || 0),
                   y: this.gry - (this.frame_y() || 0),
                   color1: this.markeratt.color,
                   lines: ["first", "second"] };

      var dist = Math.sqrt(Math.pow(pnt.x - hint.x, 2) + Math.pow(pnt.y - hint.y, 2));

      if (dist < 2.5 * this.markeratt.GetFullSize()) hint.exact = true;
      // res.menu = res.exact; // activate menu only when exactly locate bin
      // res.menu_dist = 3; // distance always fixed

      if (pnt.click_handler && hint.exact)
         hint.click_handler = this.InvokeClickHandler.bind(this);

      return hint;
   }

   GO4.MarkerPainter.prototype.movePntHandler = function() {
      var pos = d3.mouse(this.svg_frame().node());

      this.drawMarker(pos[0] + this.delta_x, pos[1] + this.delta_y);
   }

   GO4.MarkerPainter.prototype.endPntHandler = function() {
      if (this.snapid) {
         console.log('EXEC BACK!!!!');
      }

      d3.select(window).on("mousemove.markerPnt", null)
                       .on("mouseup.markerPnt", null);
   }

   GO4.MarkerPainter.prototype.InvokeClickHandler = function(hint) {
      if (!hint.exact) return; //

      d3.select(window).on("mousemove.markerPnt", this.movePntHandler.bind(this))
                       .on("mouseup.markerPnt", this.endPntHandler.bind(this), true);

      // coordinate in the frame
      var pos = d3.mouse(this.svg_frame().node());

      this.delta_x = this.grx - pos[0];
      this.delta_y = this.gry - pos[1];
   }


   GO4.MarkerPainter.prototype.ShowTooltip = function(hint) {
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
