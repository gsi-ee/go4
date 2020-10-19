(function( factory ) {

   // this is code for web canvas to support Go4 classes like
   // TGo4Marker or TGo4Condition in the go4 gui
   // it is slightly different to go4.js which is dedicated with usage of THttpServer

   if (typeof JSROOT != "object") {
      var e1 = new Error("go4canvas.js requires JSROOT to be already loaded");
      e1.source = "go4canvas.js";
      throw e1;
   }

   var myGO4 = { version: "5.9.x", web_canvas: true };

   factory(JSROOT, (typeof GO4 != 'undefined') ? GO4 : myGO4);

} (function(JSROOT, GO4) {

   "use strict";

   GO4.MarkerPainter = function(marker) {
      JSROOT.TObjectPainter.call(this, marker);
      this.pave = null; // drawing of stat
   }

   GO4.MarkerPainter.prototype = Object.create(JSROOT.TObjectPainter.prototype);

   GO4.MarkerPainter.prototype.moveDrag = function(dx,dy) {
      this.grx += dx;
      this.gry += dy;
      this.draw_g.select('path').attr("d",this.markeratt.create(this.grx, this.gry));
   }

   GO4.MarkerPainter.prototype.moveEnd = function() {
      var marker = this.GetObject();
      marker.fX = this.SvgToAxis("x", this.grx);
      marker.fY = this.SvgToAxis("y", this.gry);
      this.WebCanvasExec("SetXY(" + marker.fX + "," + marker.fY + ")");
      this.drawLabel();
   }

   GO4.MarkerPainter.prototype.drawMarker = function() {
      this.CreateG(); // can draw in complete pad

      var marker = this.GetObject();

      this.createAttMarker({ attr: marker });

      this.grx = this.AxisToSvg("x", marker.fX);
      this.gry = this.AxisToSvg("y", marker.fY);

      var path = this.markeratt.create(this.grx, this.gry);

      if (path)
          this.draw_g.append("svg:path")
             .attr("d", path)
             .call(this.markeratt.func);

      if (typeof this.AddMove == 'function')
         this.AddMove();
      else
          JSROOT.require(['interactive'])
               .then(inter => inter.DragMoveHandler.AddMove(this));
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

      return hint;
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

   GO4.ConditionPainter.prototype.afterCutDraw = function(p) {
      if (!p || !this.snapid || p._oldexec) return;
      p.snapid = this.snapid + "#member_fxCut";

      // catch TCutG exec and mark condition as modified
      p._oldexec = p.WebCanvasExec;
      p._condpainter = this;

      p.WebCanvasExec = function(exec, arg) {
         this._oldexec(exec, arg);
         p._condpainter.WebCanvasExec("SetChanged()");
      }
   }

   GO4.ConditionPainter.prototype.drawCondition = function(interactive) {

      var cond = this.GetObject();

      if (!cond || !cond.fbVisible) return;

      if (this.isPolyCond()) {
         if (cond.fxCut) {
            // look here if cut is already drawn in divid:

            var cutpaint = this.FindPainterFor(null, cond.fName, 'TCutG');

            if (cutpaint) {
               if (cutpaint.UpdateObject(cond.fxCut)) cutpaint.Redraw();
               this.afterCutDraw(cutpaint);
            } else {
               cond.fxCut.fFillStyle = 3006;
               cond.fxCut.fFillColor = 2;
               JSROOT.draw(this.divid, cond.fxCut, "LF", this.afterCutDraw.bind(this));
            }
         }
         return;
      }

      this.CreateG(true); // drawing performed inside frame

      if ((cond.fFillStyle==1001) && (cond.fFillColor==19)) {
         cond.fFillStyle = 3006;
         cond.fFillColor = 2;
      }

      this.createAttFill({attr: cond});
      this.createAttLine({attr: cond});

      this.grx1 = this.AxisToSvg("x", cond.fLow1);
      this.grx2 = this.AxisToSvg("x", cond.fUp1);

      if (cond.fiDim == 2) {
         this.gry1 = this.AxisToSvg("y", cond.fUp2);
         this.gry2 = this.AxisToSvg("y", cond.fLow2);
         this.candy = true;
      } else {
         this.gry1 = 0;
         this.gry2 = this.frame_height();
         this.candy = false;
      }

      this.draw_g.append("svg:rect")
             .attr("x", this.grx1)
             .attr("y", this.gry1)
             .attr("width", this.grx2 - this.grx1)
             .attr("height", this.gry2 - this.gry1)
             .call(this.lineatt.func)
             .call(this.fillatt.func);

      if (typeof this.AddMove == 'function')
         this.AddMove();
      else
          JSROOT.require(['interactive'])
               .then(inter => inter.DragMoveHandler.AddMove(this));
   }

   GO4.ConditionPainter.prototype.moveStart = function(x,y) {
      this.swapx = this.swapy = false;
      this.dx1 = Math.abs(x-this.grx1) < 5;
      this.dx2 = Math.abs(x-this.grx2) < 5;
      this.dy1 = Math.abs(y-this.gry1) < 5;
      this.dy2 = Math.abs(y-this.gry2) < 5;
      if (!this.dx1 && !this.dx2 && !this.dy1 && !this.dy2)
         this.dx1 = this.dx2 = this.dy1 = this.dy2 = true;
      if (!this.candy) this.dy1 = this.dy2 = false;
   }


   GO4.ConditionPainter.prototype.swap = function(n1,n2) {
      var d = this[n1];
      this[n1] = this[n2];
      this[n2] = d;
   }

   GO4.ConditionPainter.prototype.moveDrag = function(dx,dy) {
      if (this.dx1) this.grx1 += dx;
      if (this.dx2) this.grx2 += dx;
      if (this.grx1 > this.grx2) { this.swapx = true; this.swap('grx1', 'grx2'); this.swap('dx1', 'dx2'); }
      if (this.dy1) this.gry1 += dy;
      if (this.dy2) this.gry2 += dy;
      if (this.gry1 > this.gry2) { this.swapy = true; this.swap('gry1', 'gry2'); this.swap('dy1', 'dy2'); }
      this.draw_g.select('rect').attr("x",this.grx1).attr("y", this.gry1)
                 .attr("width", this.grx2 - this.grx1).attr("height", this.gry2 - this.gry1);
   }

   GO4.ConditionPainter.prototype.moveEnd = function() {
      var cond = this.GetObject(), exec = "";
      if (this.dx1 || this.swapx) { cond.fLow1 = this.SvgToAxis("x", this.grx1); exec += "SetXLow(" + cond.fLow1 + ");;"; }
      if (this.dx2 || this.swapx) { cond.fUp1 = this.SvgToAxis("x", this.grx2); exec += "SetXUp(" + cond.fUp1 + ");;"; }
      if (this.dy2 || this.swapy) { cond.fLow2 = this.SvgToAxis("y", this.gry2); exec += "SetYLow(" + cond.fLow2 + ");;"; }
      if (this.dy1 || this.swapy) { cond.fUp2 = this.SvgToAxis("y", this.gry1); exec += "SetYUp(" + cond.fUp2 + ");;"; }
      if (exec) {
         this.WebCanvasExec(exec + "SetChanged()");
         this.drawLabel();
      }
   }

   GO4.ConditionPainter.prototype.drawLabel = function() {

      var cond = this.GetObject(), painter = this;

      if (!cond.fbLabelDraw || !cond.fbVisible) return;

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

      return hint;
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

   GO4.drawCondArray = function(divid, obj, option) {
      var arr = obj.condarr.arr;
      var num = obj.fiNumCond;
      for (var k=0;k<num;++k)
         GO4.drawGo4Cond(divid, arr[k], "");
   }


   // =======================================================================

   if (GO4.web_canvas) {
      JSROOT.addDrawFunc({ name: "TGo4Marker", func: GO4.drawGo4Marker });
      JSROOT.addDrawFunc({ name: "TGo4WinCond", func: GO4.drawGo4Cond });
      JSROOT.addDrawFunc({ name: "TGo4PolyCond", func: GO4.drawGo4Cond });
      JSROOT.addDrawFunc({ name: "TGo4ShapedCond", func: GO4.drawGo4Cond });
      JSROOT.addDrawFunc({ name: "TGo4CondArray", func: GO4.drawCondArray });
   }

   return GO4;

}));
