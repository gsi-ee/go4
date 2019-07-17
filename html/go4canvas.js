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
      // if (!pnt || !pnt.disabled) this.ShowTooltip(hint);
      return hint;
   }

//   GO4.MarkerPainter.prototype.FillContextMenu = function(menu) {
//      var marker = this.GetObject();
//      menu.add("header:"+ marker._typename + "::" + marker.fxName);
//      function select(name,exec) {
//         var marker = this.GetObject();
//         marker[name] = !marker[name];
//         this.WebCanvasExec(exec + (marker[name] ? '(true)' : '(false)'));
//         this.Redraw();
//      }
//      menu.addchk(marker.fbHasLabel, 'Label', select.bind(this, 'fbHasLabel', 'SetLabelDraw'));
//      menu.addchk(marker.fbHasConnector, 'Connector', select.bind(this, 'fbHasConnector', 'SetLineDraw'));
//      menu.addchk(marker.fbXDraw, 'Draw X', select.bind(this, 'fbXDraw', 'SetXDraw'));
//      menu.addchk(marker.fbYDraw, 'Draw Y', select.bind(this, 'fbYDraw', 'SetYDraw'));
//      menu.addchk(marker.fbXbinDraw, 'Draw X bin', select.bind(this, 'fbXbinDraw', 'SetXbinDraw'));
//      menu.addchk(marker.fbYbinDraw, 'Draw Y bin', select.bind(this, 'fbYbinDraw', 'SetYbinDraw'));
//      menu.addchk(marker.fbContDraw, 'Draw content', select.bind(this, 'fbContDraw', 'SetContDraw'));
//      return true;
//   }

   GO4.MarkerPainter.prototype.ExtractTooltip = function(pnt) {
      if (!pnt) return null;

      var marker = this.GetObject();

      var hint = { name: marker.fxName,
                   title: marker.fxName,
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

   GO4.MarkerPainter.prototype.InvokeClickHandler = function(hint) {
      if (!hint.exact) return; //

      d3.select(window).on("mousemove.markerPnt", this.movePntHandler.bind(this))
                       .on("mouseup.markerPnt", this.endPntHandler.bind(this), true);

      // coordinate in the frame
      var pos = d3.mouse(this.svg_frame().node());
      this.delta_x = this.grx - pos[0] - this.frame_x();
      this.delta_y = this.gry - pos[1] - this.frame_y();
   }

   GO4.MarkerPainter.prototype.movePntHandler = function() {
      var pos = d3.mouse(this.svg_frame().node()),
          main = this.frame_painter(),
          marker = this.GetObject();

      marker.fX = main.RevertX(pos[0] + this.delta_x);
      marker.fY = main.RevertY(pos[1] + this.delta_y);

      this.drawMarker(true);
   }

   GO4.MarkerPainter.prototype.endPntHandler = function() {
      d3.select(window).on("mousemove.markerPnt", null)
                       .on("mouseup.markerPnt", null);

      var marker = this.GetObject();
      this.WebCanvasExec("SetXY(" + marker.fX + "," + marker.fY + ")");
      this.drawLabel();
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

   // =========================================================================

   GO4.ConditionPainter = function(cond) {
      JSROOT.TObjectPainter.call(this, cond);
      this.pave = null; // drawing of stat
   }

   GO4.ConditionPainter.prototype = Object.create(JSROOT.TObjectPainter.prototype);

   GO4.ConditionPainter.prototype.Test = function(x,y) {
      //  JAM: need to put this here, since condition object will lose internal definition after cloning it again!
      var cond = this.GetObject();
      if (!cond.fbEnabled)
         return cond.fbResult;

      if (cond.fxCut)
         return cond.fxCut.IsInside(x,y) ? cond.fbTrue : cond.fbFalse;

      if ((x < cond.fLow1) || (x > cond.fUp1)) return cond.fbFalse;

      if ((cond.fiDim==2) && ((y < cond.fLow2) || (y > cond.fUp2))) return cond.fbFalse;

      return cond.fbTrue;
   }


   GO4.ConditionPainter.prototype.isPolyCond = function() {
      return this.MatchObjectType("TGo4PolyCond") || this.MatchObjectType("TGo4ShapedCond");
   }

   GO4.ConditionPainter.prototype.isEllipseCond = function() {
      return this.MatchObjectType("TGo4ShapedCond");
   }

   GO4.ConditionPainter.prototype.drawCondition = function(interactive) {

      var cond = this.GetObject();

      if (this.isPolyCond()) {
         if (cond.fxCut) {
            // look here if cut is already drawn in divid:
            var cutfound = false, pthis = this;
            this.ForEachPainter(function(p) {
                if (!p.MatchObjectType("TCutG")) return;
                p.UpdateObject(cond.fxCut);
                p.Redraw();
                cutfound = true;
             });
            if(!cutfound) {
               // only redraw if previous cut display was not there:
               cond.fxCut.fFillStyle = 3006;
               cond.fxCut.fFillColor = 2;
               JSROOT.draw(this.divid, cond.fxCut, "LF", function(p) {
                  if (p) p.snapid = pthis.snapid + "#member_fxCut";
               });
            }
         }
         return;
      }

      if (interactive && this.draw_g) {
         this.draw_g.selectAll('*').remove();
      } else {
         this.CreateG(true);
      }

      if ((cond.fFillStyle==1001) && (cond.fFillColor==19)) {
         cond.fFillStyle = 3006;
         cond.fFillColor = 2;
      }

      this.createAttFill({attr: cond});
      this.createAttLine({attr: cond});

      var main = this.frame_painter();

      this.grx1 = main.grx(cond.fLow1);
      this.grx2 = main.grx(cond.fUp1);

      if (cond.fiDim == 2) {
         this.gry1 = main.gry(cond.fUp2);
         this.gry2 = main.gry(cond.fLow2);
      } else {
         this.gry1 = 0;
         this.gry2 = this.frame_height();
      }

      this.draw_g.append("svg:rect")
             .attr("x", this.grx1)
             .attr("y", this.gry1)
             .attr("width", this.grx2 - this.grx1)
             .attr("height", this.gry2 - this.gry1)
             .call(this.lineatt.func)
             .call(this.fillatt.func);
   }

   GO4.ConditionPainter.prototype.drawLabel = function() {

      var cond = this.GetObject(), painter = this;

      if (!cond.fbLabelDraw) return;

      var pave_painter = this.FindPainterFor(this.pave);

      if (!pave_painter) {
         this.pave = JSROOT.Create("TPaveStats");
         this.pave.fName = "stats_" + cond.fName;
         JSROOT.extend(this.pave, { fX1NDC: 0.1, fY1NDC: 0.4, fX2NDC: 0.4, fY2NDC: 0.65, fBorderSize: 1, fFillColor: 0, fFillStyle: 1001 });

         var st = JSROOT.gStyle;
         JSROOT.extend(this.pave, { fFillColor: st.fStatColor, fFillStyle: st.fStatStyle, fTextAngle: 0, fTextSize: st.fStatFontSize,
                                    fTextAlign: 12, fTextColor: st.fStatTextColor, fTextFont: st.fStatFont});
      } else {
         this.pave.Clear();
      }

      this.pave.AddText(cond.fName);

      this.pave.AddText("Counts = " + cond.fiCounts);

      if (cond.fbLimitsDraw)
         if (this.isPolyCond()) {
            var res = { xmin: 0, xmax: 0, ymin: 0, ymax: 0 };
            if (cond.fxCut.fNpoints > 0) {
               res.xmin = res.xmax = cond.fxCut.fX[0];
               res.ymin = res.ymax = cond.fxCut.fY[0];
               for (var i=1; i<cond.fxCut.fNpoints; i++) {
                  res.xmin = Math.min(res.xmin, cond.fxCut.fX[i]);
                  res.xmax = Math.max(res.xmax, cond.fxCut.fX[i]);
                  res.ymin = Math.min(res.ymin, cond.fxCut.fY[i]);
                  res.ymax = Math.max(res.ymax, cond.fxCut.fY[i]);
               }
            }
            this.pave.AddText("Xmin = " + res.xmin);
            this.pave.AddText("Xmax = " + res.xmax);
            this.pave.AddText("Ymin = " + res.ymin);
            this.pave.AddText("Ymax = " + res.ymax);
         } else {
            this.pave.AddText("Xmin = " + cond.fLow1);
            this.pave.AddText("Xmax = " + cond.fUp1);
            if (cond.fiDim==2) {
               this.pave.AddText("Ymin = " + cond.fLow2);
               this.pave.AddText("Ymax = " + cond.fUp2);
            }
         }

      var stat = this.main_painter().CountStat(function(x,y) { return painter.Test(x,y); });

      if (cond.fbIntDraw) this.pave.AddText("Integral = " + JSROOT.FFormat(stat.integral, "14.7g"));

      if (cond.fbXMeanDraw) this.pave.AddText("Mean x = " + JSROOT.FFormat(stat.meanx, "6.4g"));

      if (cond.fbXRMSDraw) this.pave.AddText("RMS x = " + JSROOT.FFormat(stat.rmsx, "6.4g"));

      if (cond.fiDim==2) {
         if (cond.fbYMeanDraw) this.pave.AddText("Mean y = " + JSROOT.FFormat(stat.meany, "6.4g"));
         if (cond.fbYRMSDraw) this.pave.AddText("RMS y = " + JSROOT.FFormat(stat.rmsy, "6.4g"));
      }

      if (cond.fbXMaxDraw) this.pave.AddText("X max = " + JSROOT.FFormat(stat.xmax, "6.4g"));

      if (cond.fiDim==2)
         if (cond.fbYMaxDraw) this.pave.AddText("Y max = " + JSROOT.FFormat(stat.ymax, "6.4g"));
      if (cond.fbCMaxDraw) this.pave.AddText("C max = " + JSROOT.FFormat(stat.wmax, "14.7g"));

      if (!pave_painter)
         pave_painter = JSROOT.draw(this.divid, this.pave, "");
      else
         pave_painter.Redraw();
   }

   GO4.ConditionPainter.prototype.ProcessTooltip = function(pnt) {
      var hint = this.ExtractTooltip(pnt);
      // if (!pnt || !pnt.disabled) this.ShowTooltip(hint);
      return hint;
   }

//   GO4.ConditionPainter.prototype.FillContextMenu = function(menu) {
//      var cond = this.GetObject();
//      menu.add("header:"+ cond._typename + "::" + cond.fName);
//      function select(name,exec) {
//         var cond = this.GetObject();
//         cond[name] = !cond[name];
//         this.WebCanvasExec(exec + (cond[name] ? '(true)' : '(false)'));
//         this.Redraw();
//      }
//      menu.addchk(cond.fbLabelDraw, 'Label', select.bind(this, 'fbLabelDraw', 'SetLabelDraw'));
//      menu.addchk(cond.fbLimitsDraw, 'Limits', select.bind(this, 'fbLimitsDraw', 'SetLimitsDraw'));
//      menu.addchk(cond.fbIntDraw, 'Integral', select.bind(this, 'fbIntDraw', 'SetIntDraw'));
//      menu.addchk(cond.fbXMeanDraw, 'X mean', select.bind(this, 'fbXMeanDraw', 'SetXMeanDraw'));
//      menu.addchk(cond.fbXRMSDraw, 'X rms', select.bind(this, 'fbXRMSDraw', 'SetXRMSDraw'));
//      menu.addchk(cond.fbXMaxDraw, 'X max', select.bind(this, 'fbXMaxDraw', 'SetXMaxDraw'));
//      menu.addchk(cond.fbYMeanDraw, 'Y mean', select.bind(this, 'fbYMeanDraw', 'SetYMeanDraw'));
//      menu.addchk(cond.fbYRMSDraw, 'Y rms', select.bind(this, 'fbYRMSDraw', 'SetYRMSDraw'));
//      menu.addchk(cond.fbYMaxDraw, 'Y max', select.bind(this, 'fbYMaxDraw', 'SetYMaxDraw'));
//      return true;
//   }

   GO4.ConditionPainter.prototype.ExtractTooltip = function(pnt) {
      if (!pnt) return null;

      var cond = this.GetObject();

      var hint = { name: cond.fName,
                   title: cond.fTitle,
                   painter: this,
                   menu: true,
                   x: pnt.x,
                   y: pnt.y };

      if (this.isPolyCond()) {

      } else {

         hint.color1 = this.fillatt.color;
         hint.color2 = this.lineatt.color;

         hint.menu_dist = Math.sqrt(Math.pow(pnt.x - (this.grx1 + this.grx2)/2, 2) + Math.pow(pnt.y - (this.gry1 + this.gry2)/2, 2));
         hint.exact = (this.grx1 <= pnt.x) && (pnt.x <= this.grx2) && (this.gry1 <= pnt.y) && (pnt.y <= this.gry2);
         if (Math.abs(this.grx1 - pnt.x) < 5) hint.sidex = -1;
         if (Math.abs(this.grx2 - pnt.x) < 5) hint.sidex = 1;
         if (cond.fiDim == 2) {
            if (Math.abs(this.gry1 - pnt.y) < 5) hint.sidey = 1;
            if (Math.abs(this.gry2 - pnt.y) < 5) hint.sidey = -1;
         }
      }

      if (hint.exact)
         hint.lines = ["condition", cond.fName ];

      if (pnt.click_handler && (hint.sidex || hint.sidey))
         hint.click_handler = this.InvokeClickHandler.bind(this);

      return hint;
   }

   GO4.ConditionPainter.prototype.InvokeClickHandler = function(hint) {
      if (!hint.exact) return; //

      d3.select(window).on("mousemove.condLmt", this.movePntHandler.bind(this))
                       .on("mouseup.condLmt", this.endPntHandler.bind(this), true);

      // coordinate in the frame
      var pos = d3.mouse(this.svg_frame().node());
      this.sidex = hint.sidex || 0;
      this.sidey = hint.sidey || 0;
      this.fullset = false;

      if (this.sidex < 0)
         this.deltax = this.grx1 - pos[0];
      else if (this.sidex > 0)
         this.deltax = this.grx2 - pos[0];
      if (this.sidey < 0)
         this.deltay = this.gry2 - pos[1];
      else if (this.sidey > 0)
         this.deltay = this.gry1 - pos[1];
   }

   GO4.ConditionPainter.prototype.movePntHandler = function() {
      var pos = d3.mouse(this.svg_frame().node()),
          main = this.frame_painter(),
          cond = this.GetObject();

      if (this.sidex < 0) {
         cond.fLow1 = main.RevertX(pos[0] + this.deltax);
      } else if (this.sidex > 0) {
         cond.fUp1 = main.RevertX(pos[0] + this.deltax);
      }

      if (this.sidex && (cond.fLow1 > cond.fUp1)) {
         this.sidex = -this.sidex; this.fullset = true;
         var d = cond.fLow1; cond.fLow1 = cond.fUp1; cond.fUp1 = d;
      }

      if (this.sidey < 0) {
         cond.fLow2 = main.RevertY(pos[1] + this.deltay);
      } else if (this.sidey > 0) {
         cond.fUp2 = main.RevertY(pos[1] + this.deltay);
      }

      if (this.sidey && (cond.fLow2 > cond.fUp2)) {
         this.sidey = -this.sidey; this.fullset = true;
         var d = cond.fLow2; cond.fLow2 = cond.fUp2; cond.fUp2 = d;
      }

      this.drawCondition(true);
   }

   GO4.ConditionPainter.prototype.endPntHandler = function() {
      d3.select(window).on("mousemove.condLmt", null)
                       .on("mouseup.condLmt", null);
      var cond = this.GetObject(), exec = "";
      if ((this.sidex < 0) || this.fullset)
         exec += "SetXLow(" + cond.fLow1 + ");;";
      if ((this.sidex > 0) || this.fullset)
         exec += "SetXUp(" + cond.fUp1 + ");;";
      if ((this.sidey < 0) || this.fullset)
         exec += "SetYLow(" + cond.fLow2 + ");;";
      if ((this.sidey > 0) || this.fullset)
         exec += "SetYUp(" + cond.fUp2 + ");;";
      if (exec) this.WebCanvasExec(exec + "SetChanged()");

      this.sidex = this.sidey = this.deltax = this.deltay = 0;

      this.drawLabel();
   }

   GO4.ConditionPainter.prototype.RedrawObject = function(obj) {
      if (this.UpdateObject(obj))
         this.Redraw(); // no need to redraw complete pad
   }

   GO4.ConditionPainter.prototype.Cleanup = function(arg) {
      if (this.pave) {
         var pp = this.FindPainterFor(this.pave);
         if (pp) pp.DeleteThis();
         delete this.pave;
      }

      JSROOT.TObjectPainter.prototype.Cleanup.call(this, arg);
   }

   GO4.ConditionPainter.prototype.Redraw = function() {
      this.drawCondition();
      this.drawLabel();
   }

   GO4.drawGo4Cond = function(divid, cond, option) {

      if (GO4.web_canvas || (option=='same')) {
         var condpainter = new GO4.ConditionPainter(cond);
         condpainter.SetDivId(divid);
         condpainter.drawCondition();
         condpainter.drawLabel();
         return condpainter.DrawingReady();
      }

      // from here normal code for plain THttpServer

      if (((cond.fxHistoName=="") || (option=='editor')) && GO4.ConditionEditor) {
         // $('#'+divid).append("<br/>Histogram name not specified");
         var h = $("#"+divid).height(), w = $("#"+divid).width();
         if ((h<10) && (w>10)) $("#"+divid).height(w*0.4);
         var editor = new GO4.ConditionEditor(cond);
         return editor.drawEditor(divid);
      }

      // $('#'+divid).append("<br/>Histogram name is " + cond.fxHistoName);

      if (!JSROOT.hpainter) {
         $('#'+divid).append("<br/>Error - did not found hierarchy painter");
         return;
      }

      var histofullpath = null;

      JSROOT.hpainter.ForEach(function(h) {
         if ((h['_name'] == cond.fxHistoName) && (h['_kind'].indexOf("ROOT.TH")==0)) {
            histofullpath = JSROOT.hpainter.itemFullName(h);
            return true;
         }
      });

      if (histofullpath == null) {
         $('#'+divid).append("<br/>Error - did not found histogram " + cond.fxHistoName);

         histofullpath = "../../Histograms/" + cond.fxHistoName;

         JSROOT.hpainter.Find({ name: histofullpath, force: true})['_kind'] = "ROOT.TH1I";

         console.log("Try histogram" + histofullpath);
      }

      $('#'+divid).append("<br/>Drawing histogram " + histofullpath);

      $('#'+divid).empty();

      var condpainter = new GO4.ConditionPainter(cond);

      JSROOT.hpainter.display(histofullpath, "divid:" + divid, function(res) {
         if (res==null) return console.log("fail to get histogram " + histofullpath);
         condpainter.SetDivId(divid);
         condpainter.drawCondition();
         condpainter.drawLabel();
         condpainter.DrawingReady();
      });

      return condpainter;
   }


   // =======================================================================

   if (GO4.web_canvas) {
      JSROOT.addDrawFunc("TGo4Marker", GO4.drawGo4Marker, "");
      JSROOT.addDrawFunc("TGo4WinCond", GO4.drawGo4Cond, "");
      JSROOT.addDrawFunc("TGo4PolyCond", GO4.drawGo4Cond, "");
      JSROOT.addDrawFunc("TGo4ShapedCond", GO4.drawGo4Cond, "");
   }

   return GO4;

}));
