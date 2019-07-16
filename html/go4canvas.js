(function( factory ) {

   // this is code for web canvas to support Go4 classes like
   // TGo4Marker or TGo4Condition in the go4 gui
   // it is slightly different to go4.js which is dedicated with usage of THttpServer

   if (typeof JSROOT != "object") {
      var e1 = new Error("go4canvas.js requires JSROOT to be already loaded");
      e1.source = "go4canvas.js";
      throw e1;
   }

   var myGO4 = { version: "5.3.x", web_canvas: true };

   factory(JSROOT, (typeof GO4 != 'undefined') ? GO4 : myGO4);

} (function(JSROOT, GO4) {

   "use strict";

   GO4.MarkerPainter = function(marker) {
      JSROOT.TObjectPainter.call(this, marker);
      this.pave = null; // drawing of stat
   }

   GO4.MarkerPainter.prototype = Object.create(JSROOT.TObjectPainter.prototype);

   GO4.MarkerPainter.prototype.drawMarker = function(interactive) {
      if (interactive && this.draw_g) {
         this.draw_g.selectAll('*').remove();
      } else {
         this.CreateG(); // can draw in complete pad
      }

      var marker = this.GetObject();

      this.createAttMarker({ attr: marker });

      this.grx = this.AxisToSvg("x", marker.fX);
      this.gry = this.AxisToSvg("y", marker.fY);

      var path = this.markeratt.create(this.grx, this.gry);

      if (path)
          this.draw_g.append("svg:path")
             .attr("d", path)
             .call(this.markeratt.func);
   }

   GO4.MarkerPainter.prototype.fillLabels = function(marker) {
      var lbls = [];

      var main = this.main_painter(), hint = null;
      if (main && typeof main.ProcessTooltip == 'function')
         hint = main.ProcessTooltip({ enabled: false, x: this.grx - this.frame_x(), y: this.gry - this.frame_y() });

      lbls.push(marker.fxName + ((hint && hint.name) ? (" : " + hint.name) : ""));

      if (marker.fbXDraw)
          lbls.push("X = " + JSROOT.FFormat(marker.fX, "6.4g"));

      if (marker.fbYDraw)
         lbls.push("Y = " + JSROOT.FFormat(marker.fY, "6.4g"));

      if (hint && hint.user_info) {
         if (marker.fbXbinDraw) {
            var bin = "<undef>";
            if (hint.user_info.binx !== undefined) bin = hint.user_info.binx; else
            if (hint.user_info.bin !== undefined) bin = hint.user_info.bin;
            lbls.push("Xbin = " + bin);
         }

         if (marker.fbYbinDraw) {
            lbls.push("Ybin = " + ((hint.user_info.biny !== undefined) ? hint.user_info.biny : "<undef>"));
         }

         if (marker.fbContDraw)
            lbls.push("Cont = " + hint.user_info.cont);
      }

      return lbls;
   }

   GO4.MarkerPainter.prototype.drawLabel = function() {

      var marker = this.GetObject();

      if (!marker.fbHasLabel) return;

      var pave_painter = this.FindPainterFor(this.pave);

      if (!pave_painter) {
         this.pave = JSROOT.Create("TPaveStats");
         this.pave.fName = "stats_" + marker.fName;

         var px = this.grx / this.pad_width() + 0.02,
             py = this.gry / this.pad_height() - 0.02;
         JSROOT.extend(this.pave, { fX1NDC: px, fY1NDC: py - 0.15, fX2NDC: px + 0.2, fY2NDC: py, fBorderSize: 1, fFillColor: 0, fFillStyle: 1001 });

         var st = JSROOT.gStyle;
         JSROOT.extend(this.pave, { fFillColor: st.fStatColor, fFillStyle: st.fStatStyle, fTextAngle: 0, fTextSize: st.fStatFontSize,
                                    fTextAlign: 12, fTextColor: st.fStatTextColor, fTextFont: st.fStatFont});
      } else {
         this.pave.Clear();
      }

      var lbls = this.fillLabels(marker);
      for (var k=0;k<lbls.length;++k)
         this.pave.AddText(lbls[k]);

      if (pave_painter)
         pave_painter.Redraw();
      else
         JSROOT.draw(this.divid, this.pave, "", function(p) {
           if (p) p.$secondary = true
         });
   }

   GO4.MarkerPainter.prototype.RedrawObject = function(obj) {
      if (this.UpdateObject(obj))
         this.Redraw(); // no need to redraw complete pad
   }

   GO4.MarkerPainter.prototype.Cleanup = function(arg) {
      if (this.pave) {
         var pp = this.FindPainterFor(this.pave);
         if (pp) pp.DeleteThis();
         delete this.pave;
      }

      JSROOT.TObjectPainter.prototype.Cleanup.call(this, arg);
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

   GO4.MarkerPainter.prototype.FillContextMenu = function(menu) {
      var marker = this.GetObject();
      menu.add("header:"+ marker._typename + "::" + marker.fxName);
      function select(name,exec) {
         var marker = this.GetObject();
         marker[name] = !marker[name];
         this.execServer(exec + (marker[name] ? '(true)' : '(false)'));
         this.Redraw();
      }
      menu.addchk(marker.fbHasLabel, 'Label', select.bind(this, 'fbHasLabel', 'SetLabelDraw'));
      menu.addchk(marker.fbHasConnector, 'Connector', select.bind(this, 'fbHasConnector', 'SetLineDraw'));
      menu.addchk(marker.fbXDraw, 'Draw X', select.bind(this, 'fbXDraw', 'SetXDraw'));
      menu.addchk(marker.fbYDraw, 'Draw Y', select.bind(this, 'fbYDraw', 'SetYDraw'));
      menu.addchk(marker.fbXbinDraw, 'Draw X bin', select.bind(this, 'fbXbinDraw', 'SetXbinDraw'));
      menu.addchk(marker.fbYbinDraw, 'Draw Y bin', select.bind(this, 'fbYbinDraw', 'SetYbinDraw'));
      menu.addchk(marker.fbContDraw, 'Draw content', select.bind(this, 'fbContDraw', 'SetContDraw'));
      return true;
   }

   GO4.MarkerPainter.prototype.ExtractTooltip = function(pnt) {
      if (!pnt) return null;

      var marker = this.GetObject();

      var hint = { name: marker.fName,
                   title: marker.fTitle,
                   painter: this,
                   menu: true,
                   x: this.grx - (this.frame_x() || 0),
                   y: this.gry - (this.frame_y() || 0),
                   color1: this.markeratt.color };

      var dist = Math.sqrt(Math.pow(pnt.x - hint.x, 2) + Math.pow(pnt.y - hint.y, 2));

      hint.menu_dist = dist;

      if (dist < 2.5 * this.markeratt.GetFullSize()) hint.exact = true;

      if (hint.exact)
         hint.lines = this.fillLabels(marker);

      // res.menu = res.exact; // activate menu only when exactly locate bin
      // res.menu_dist = 3; // distance always fixed

      if (pnt.click_handler && hint.exact)
         hint.click_handler = this.InvokeClickHandler.bind(this);

      return hint;
   }

   GO4.MarkerPainter.prototype.movePntHandler = function() {
      var pos = d3.mouse(this.svg_frame().node()),
          main = this.frame_painter(),
          marker = this.GetObject();

      marker.fX = main.RevertX(pos[0] + this.delta_x);
      marker.fY = main.RevertY(pos[1] + this.delta_y);

      this.drawMarker(true);
   }

   GO4.MarkerPainter.prototype.execServer = function(exec) {
      if (this.snapid && exec) {
         var canp = this.canv_painter();
         if (canp && (typeof canp.SendWebsocket == 'function'))
            canp.SendWebsocket("OBJEXEC:" + this.snapid + ":" + exec);
      }
   }

   GO4.MarkerPainter.prototype.endPntHandler = function() {
      d3.select(window).on("mousemove.markerPnt", null)
                       .on("mouseup.markerPnt", null);

      var marker = this.GetObject();
      if (marker)
         this.execServer("SetXY(" + marker.fX + "," + marker.fY + ")");
      this.drawLabel();
   }

   GO4.MarkerPainter.prototype.InvokeClickHandler = function(hint) {
      if (!hint.exact) return; //

      d3.select(window).on("mousemove.markerPnt", this.movePntHandler.bind(this))
                       .on("mouseup.markerPnt", this.endPntHandler.bind(this), true);

      // coordinate in the frame
      var pos = d3.mouse(this.svg_frame().node());
      this.delta_x = this.grx - pos[0] - this.frame_x();
      this.delta_y = this.gry - pos[1] - this.frame_y();
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

   if (GO4.web_canvas) {
      JSROOT.addDrawFunc("TGo4Marker", GO4.drawGo4Marker, "");
   }

   return GO4;

}));
