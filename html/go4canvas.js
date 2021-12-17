// $Id$

(function( factory ) {

   // this is code for web canvas to support Go4 classes like
   // TGo4Marker or TGo4Condition in the go4 gui
   // it is slightly different to go4.js which is dedicated with usage of THttpServer

   if (typeof JSROOT != "object") {
      let e1 = new Error("go4canvas.js requires JSROOT to be already loaded");
      e1.source = "go4canvas.js";
      throw e1;
   }

   let myGO4 = { version: "6.1.4", web_canvas: true, id_counter: 1 };

   factory(JSROOT, (typeof GO4 != 'undefined') ? GO4 : myGO4);

} (function(JSROOT, GO4) {

   "use strict";

   // adding support of JSROOT v6
   let ObjectPainter = JSROOT.ObjectPainter || JSROOT.TObjectPainter;

   let BasePainter = JSROOT.BasePainter || JSROOT.TBasePainter;

   if (!JSROOT.BasePainter.prototype.getDomId)
      JSROOT.BasePainter.prototype.getDomId = function() {
         let elem = this.selectDom();
         if (elem.empty()) return "";
         let id = elem.attr("id");
         if (!id) {
            id = "go4_element_" + GO4.id_counter++;
            elem.attr("id", id);
         }
         return id;
      }

   let FFormat, findPainter;

   if (!JSROOT._) {
      BasePainter.prototype.getItemName = BasePainter.prototype.GetItemName;
      ObjectPainter.prototype.getObject = ObjectPainter.prototype.GetObject;
      ObjectPainter.prototype.createG = ObjectPainter.prototype.CreateG;
      ObjectPainter.prototype.axisToSvg = ObjectPainter.prototype.AxisToSvg;
      ObjectPainter.prototype.svgToAxis = ObjectPainter.prototype.SvgToAxis;
      ObjectPainter.prototype.getMainPainter = ObjectPainter.prototype.main_painter;
      JSROOT.create = JSROOT.Create;
      FFormat = JSROOT.FFormat;
      findPainter = function(painter, obj, name, typ) {
         return painter.FindPainterFor(obj, name, typ);
      }
   } else {
      FFormat = JSROOT.Painter.floatToString;
      findPainter = (painter, obj, name, typ) => {
         let pp = painter.getPadPainter();
         return pp ? pp.findPainterFor(obj, name, typ) : null;
      }
   }

   GO4.MarkerPainter = function(divid, marker) {
      if (JSROOT._) {
         ObjectPainter.call(this, divid, marker);
      } else {
         ObjectPainter.call(this, marker);
         this.SetDivId(divid); // old
      }
      this.pave = null; // drawing of stat
   }

   GO4.MarkerPainter.prototype = Object.create(ObjectPainter.prototype);

   GO4.MarkerPainter.prototype.moveDrag = function(dx,dy) {
      this.grx += dx;
      this.gry += dy;
      this.draw_g.select('path').attr("d",this.markeratt.create(this.grx, this.gry));
   }

   GO4.MarkerPainter.prototype.moveEnd = function() {
      var marker = this.getObject();
      marker.fX = this.svgToAxis("x", this.grx);
      marker.fY = this.svgToAxis("y", this.gry);
      var exec = "SetXY(" + marker.fX + "," + marker.fY + ")";
      if (JSROOT._)
         this.submitCanvExec(exec);
      else
         this.WebCanvasExec(exec);
      this.drawLabel();
   }

   GO4.MarkerPainter.prototype.drawMarker = function() {
      let g = this.createG(); // can draw in complete pad

      var marker = this.getObject();

      this.createAttMarker({ attr: marker });

      this.grx = this.axisToSvg("x", marker.fX);
      this.gry = this.axisToSvg("y", marker.fY);

      var path = this.markeratt.create(this.grx, this.gry);

      if (path)
          g.append("svg:path")
             .attr("d", path)
             .call(this.markeratt.func);

      if (typeof this.AddMove == 'function')
         this.AddMove();
      else
          JSROOT.require(['interactive'])
               .then(inter => inter.addMoveHandler(this));
   }

   GO4.MarkerPainter.prototype.fillLabels = function(marker) {
      var lbls = [];

      var main = this.getMainPainter(), hint = null, fx = 0, fy = 0;
      if (JSROOT._) {
         let rect = this.getFramePainter().getFrameRect();
         fx = rect.x;
         fy = rect.y;
         if (main && typeof main.processTooltipEvent == 'function')
            hint = main.processTooltipEvent({ enabled: false, x: this.grx - fx, y: this.gry - fy });
      } else {
         fx = this.frame_x();
         fy = this.frame_y();
         if (main && typeof main.ProcessTooltip == 'function')
            hint = main.ProcessTooltip({ enabled: false, x: this.grx - fx, y: this.gry - fy });
      }


      lbls.push(marker.fxName + ((hint && hint.name) ? (" : " + hint.name) : ""));

      if (marker.fbXDraw)
          lbls.push("X = " + FFormat(marker.fX, "6.4g"));

      if (marker.fbYDraw)
         lbls.push("Y = " + FFormat(marker.fY, "6.4g"));

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

      var marker = this.getObject();

      if (!marker.fbHasLabel) return;

      var pave_painter = findPainter(this, this.pave);

      if (!pave_painter) {
         this.pave = JSROOT.create("TPaveStats");
         this.pave.fName = "stats_" + marker.fName;

         var pad_width = 10, pad_height = 10;

         if (JSROOT._) {
            let pp = this.getPadPainter();
            pad_width = pp.getPadWidth();
            pad_height = pp.getPadHeight();
         } else {
            pad_width = this.pad_width();
            pad_height = this.pad_height();
         }

         var px = this.grx / pad_width + 0.02,
             py = this.gry / pad_height - 0.02;
         JSROOT.extend(this.pave, { fX1NDC: px, fY1NDC: py - 0.15, fX2NDC: px + 0.2, fY2NDC: py, fBorderSize: 1, fFillColor: 0, fFillStyle: 1001 });

         var st = JSROOT.gStyle;
         JSROOT.extend(this.pave, { fFillColor: st.fStatColor, fFillStyle: st.fStatStyle, fTextAngle: 0, fTextSize: st.fStatFontSize,
                                    fTextAlign: 12, fTextColor: st.fStatTextColor, fTextFont: st.fStatFont });
      } else {
         this.pave.Clear();
      }

      var lbls = this.fillLabels(marker);
      for (var k = 0; k < lbls.length; ++k)
         this.pave.AddText(lbls[k]);

      if (JSROOT._) {
         if (pave_painter)
            pave_painter.redraw();
         else
            JSROOT.draw(this.divid, this.pave, "").then(p => { if (p) p.$secondary = true; });
      } else {
         if (pave_painter)
            pave_painter.redraw();
         else
            JSROOT.draw(this.divid, this.pave, "", function(p) {
              if (p) p.$secondary = true;
            });
      }
   }

   // should work for both jsroot v5 and v6 up to naming convention
   if (JSROOT._) {
      GO4.MarkerPainter.prototype.redrawObject = function(obj) {
         if (!this.updateObject(obj)) return false;
         this.redraw(); // no need to redraw complete pad
         return true;
      }

      GO4.MarkerPainter.prototype.cleanup = function(arg) {
         if (this.pave) {
            var pp = findPainter(this, this.pave);
            if (pp) {
               pp.removeFromPadPrimitives();
               pp.cleanup();
            }
            delete this.pave;
         }

         ObjectPainter.prototype.cleanup.call(this, arg);
      }
   } else {
      GO4.MarkerPainter.prototype.RedrawObject = function(obj) {
         if (!this.UpdateObject(obj)) return false;
         this.redraw(); // no need to redraw complete pad
         return true;
      }

      GO4.MarkerPainter.prototype.Cleanup = function(arg) {
         if (this.pave) {
            var pp = findPainter(this, this.pave);
            if (pp) pp.DeleteThis();
            delete this.pave;
         }

         ObjectPainter.prototype.Cleanup.call(this, arg);
      }
   }


   GO4.MarkerPainter.prototype.redraw = function() {
      this.drawMarker();
      this.drawLabel();
   }

//   GO4.MarkerPainter.prototype.fillContextMenu = function(menu) {
//      var marker = this.getObject();
//      menu.add("header:"+ marker._typename + "::" + marker.fxName);
//      function select(name,exec) {
//         var marker = this.getObject();
//         marker[name] = !marker[name];
//         this.submitCanvExec(exec + (marker[name] ? '(true)' : '(false)'));
//         this.redraw();
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

   GO4.MarkerPainter.prototype.processTooltipEvent = function(pnt) {
      if (!pnt) return null;

      var marker = this.getObject(), fx = 0, fy = 0, marker_sz = 1;
      if (JSROOT._) {
         let rect = this.getFramePainter().getFrameRect();
         fx = rect.x;
         fy = rect.y;
         marker_sz = this.markeratt.getFullSize();
      } else {
         fx = this.frame_x();
         fy = this.frame_y();
         marker_sz = this.markeratt.GetFullSize();
      }

      var hint = { name: marker.fxName,
                   title: marker.fxName,
                   painter: this,
                   menu: true,
                   x: this.grx - fx,
                   y: this.gry - fy,
                   color1: this.markeratt.color };

      var dist = Math.sqrt(Math.pow(pnt.x - hint.x, 2) + Math.pow(pnt.y - hint.y, 2));

      hint.menu_dist = dist;

      if (dist < 2.5 * marker_sz) hint.exact = true;

      if (hint.exact)
         hint.lines = this.fillLabels(marker);

      // res.menu = res.exact; // activate menu only when exactly locate bin
      // res.menu_dist = 3; // distance always fixed

      return hint;
   }

   if (!JSROOT._) {
      GO4.MarkerPainter.prototype.Redraw = GO4.MarkerPainter.prototype.redraw;
      GO4.MarkerPainter.prototype.ProcessTooltip = GO4.MarkerPainter.prototype.processTooltipEvent;
   }

   GO4.MarkerPainter.prototype.ShowTooltip = function(hint) {
   }

   GO4.drawGo4Marker = function(divid, obj, option) {
      var painter = new GO4.MarkerPainter(divid, obj);
      if (JSROOT._) {
         painter.drawMarker();
         painter.drawLabel();
         painter.addToPadPrimitives();
         return Promise.resolve(painter);
      }
      painter.drawMarker();
      painter.drawLabel();
      return painter.DrawingReady();
   }

   // =========================================================================

   if (!ObjectPainter.prototype.matchObjectType)
      ObjectPainter.prototype.matchObjectType = ObjectPainter.prototype.MatchObjectType;

   GO4.ConditionPainter = function(divid, cond) {
      if (JSROOT._) {
         ObjectPainter.call(this, divid, cond);
      } else {
         ObjectPainter.call(this, cond);
         this.SetDivId(divid, -1)
      }

      this.pave = null; // drawing of stat
   }

   GO4.ConditionPainter.prototype = Object.create(ObjectPainter.prototype);

   GO4.ConditionPainter.prototype.Test = function(x,y) {
      //  JAM: need to put this here, since condition object will lose internal definition after cloning it again!
      var cond = this.getObject();
      if (!cond.fbEnabled)
         return cond.fbResult;

      if (cond.fxCut)
         return cond.fxCut.IsInside(x,y) ? cond.fbTrue : cond.fbFalse;

      if ((x < cond.fLow1) || (x > cond.fUp1)) return cond.fbFalse;

      if ((cond.fiDim==2) && ((y < cond.fLow2) || (y > cond.fUp2))) return cond.fbFalse;

      return cond.fbTrue;
   }


   GO4.ConditionPainter.prototype.isPolyCond = function() {
      return this.matchObjectType("TGo4PolyCond") || this.matchObjectType("TGo4ShapedCond");
   }

   GO4.ConditionPainter.prototype.isEllipseCond = function() {
      return this.matchObjectType("TGo4ShapedCond");
   }

   GO4.ConditionPainter.prototype.afterCutDraw = function(p) {
      if (!p || !this.snapid || p._oldexec) return;
      p.snapid = this.snapid + "#member_fxCut";
      p._condpainter = this;

      // catch TCutG exec and mark condition as modified
      if (JSROOT._) {
         p._oldexec = p.submitCanvExec;
         p.submitCanvExec = function(exec, arg) {
            this._oldexec(exec, arg);
            p._condpainter.submitCanvExec("SetChanged()");
         }
      } else {
         p._oldexec = p.WebCanvasExec;
         p.WebCanvasExec = function(exec, arg) {
            this._oldexec(exec, arg);
            p._condpainter.WebCanvasExec("SetChanged()");
         }

      }
   }

   GO4.ConditionPainter.prototype.drawCondition = function(interactive) {

      var cond = this.getObject();

      if (!cond || !cond.fbVisible) return;

      if (this.isPolyCond()) {
         if (cond.fxCut) {
            // look here if cut is already drawn in divid:
            var cutpaint = findPainter(this, null, cond.fName, 'TCutG');

            if (JSROOT._) {
               if (cutpaint) {
                  if (cutpaint.updateObject(cond.fxCut)) cutpaint.redraw();
                  this.afterCutDraw(cutpaint);
               } else {
                  cond.fxCut.fFillStyle = 3006;
                  cond.fxCut.fFillColor = 2;
                  JSROOT.draw(this.divid, cond.fxCut, "LF").then(p => this.afterCutDraw(p));
               }

            } else { // old jsroot v5
               if (cutpaint) {
                  if (cutpaint.UpdateObject(cond.fxCut)) cutpaint.redraw();
                  this.afterCutDraw(cutpaint);
               } else {
                  cond.fxCut.fFillStyle = 3006;
                  cond.fxCut.fFillColor = 2;
                  JSROOT.draw(this.divid, cond.fxCut, "LF", p => this.afterCutDraw(p));
               }
            }
         }
         return;
      }


      let g = this.createG(true); // drawing performed inside frame

      if ((cond.fFillStyle==1001) && (cond.fFillColor==19)) {
         cond.fFillStyle = 3006;
         cond.fFillColor = 2;
      }

      this.createAttFill({attr: cond});
      this.createAttLine({attr: cond});

      this.grx1 = this.axisToSvg("x", cond.fLow1);
      this.grx2 = this.axisToSvg("x", cond.fUp1);

      if (cond.fiDim == 2) {
         this.gry1 = this.axisToSvg("y", cond.fUp2);
         this.gry2 = this.axisToSvg("y", cond.fLow2);
         this.candy = true;
      } else {
         this.gry1 = 0;
         this.gry2 = JSROOT._ ? this.getFramePainter().getFrameHeight() : this.frame_height();
         this.candy = false;
      }

      g.append("svg:rect")
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
               .then(inter => inter.addMoveHandler(this));
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

   GO4.ConditionPainter.prototype.moveDrag = function(dx,dy) {
      if (this.dx1) this.grx1 += dx;
      if (this.dx2) this.grx2 += dx;
      if (this.grx1 > this.grx2) {
         this.swapx = true;
         let tempx = this.grx1; this.grx1 = this.grx2; this.grx2 = tempx;
         tempx = this.dx1; this.dx1 = this.dx2; this.dx2 = tempx;
      }
      if (this.dy1) this.gry1 += dy;
      if (this.dy2) this.gry2 += dy;
      if (this.gry1 > this.gry2) {
         this.swapy = true;
         let tempy = this.gry1; this.gry1 = this.gry2; this.gry2 = tempy;
         tempy = this.dy1; this.dy1 = this.dy2; this.dy2 = tempy;
      }
      this.draw_g.select('rect').attr("x",this.grx1).attr("y", this.gry1)
                 .attr("width", this.grx2 - this.grx1).attr("height", this.gry2 - this.gry1);
   }

   GO4.ConditionPainter.prototype.moveEnd = function() {
      var cond = this.getObject(), exec = "";
      if (this.dx1 || this.swapx) { cond.fLow1 = this.svgToAxis("x", this.grx1); exec += "SetXLow(" + cond.fLow1 + ");;"; }
      if (this.dx2 || this.swapx) { cond.fUp1 = this.svgToAxis("x", this.grx2); exec += "SetXUp(" + cond.fUp1 + ");;"; }
      if (this.dy2 || this.swapy) { cond.fLow2 = this.svgToAxis("y", this.gry2); exec += "SetYLow(" + cond.fLow2 + ");;"; }
      if (this.dy1 || this.swapy) { cond.fUp2 = this.svgToAxis("y", this.gry1); exec += "SetYUp(" + cond.fUp2 + ");;"; }
      if (exec) {
         if (JSROOT._)
            this.submitCanvExec(exec + "SetChanged()");
         else
            this.WebCanvasExec(exec + "SetChanged()");
         this.drawLabel();
      }
   }

   GO4.ConditionPainter.prototype.drawLabel = function() {

      var cond = this.getObject(), painter = this, stat = {};

      if (!cond.fbLabelDraw || !cond.fbVisible) return;

      var pave_painter = findPainter(this, this.pave);

      if (!pave_painter) {
         this.pave = JSROOT.create("TPaveStats");
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

      if (JSROOT._) {
         stat = this.getMainPainter().countStat((x,y) => painter.Test(x,y));
      } else {
         stat = this.getMainPainter().CountStat(function(x,y) { return painter.Test(x,y); });
      }

      if (cond.fbIntDraw) this.pave.AddText("Integral = " + FFormat(stat.integral, "14.7g"));

      if (cond.fbXMeanDraw) this.pave.AddText("Mean x = " + FFormat(stat.meanx, "6.4g"));

      if (cond.fbXRMSDraw) this.pave.AddText("RMS x = " + FFormat(stat.rmsx, "6.4g"));

      if (cond.fiDim==2) {
         if (cond.fbYMeanDraw) this.pave.AddText("Mean y = " + FFormat(stat.meany, "6.4g"));
         if (cond.fbYRMSDraw) this.pave.AddText("RMS y = " + FFormat(stat.rmsy, "6.4g"));
      }

      if (cond.fbXMaxDraw) this.pave.AddText("X max = " + FFormat(stat.xmax, "6.4g"));

      if (cond.fiDim==2)
         if (cond.fbYMaxDraw) this.pave.AddText("Y max = " + FFormat(stat.ymax, "6.4g"));
      if (cond.fbCMaxDraw) this.pave.AddText("C max = " + FFormat(stat.wmax, "14.7g"));

      if (!pave_painter)
         JSROOT.draw(this.divid, this.pave, "");
      else if (JSROOT._)
         pave_painter.redraw();
      else
         pave_painter.Redraw(); // old jsroot v5
   }

//   GO4.ConditionPainter.prototype.fillContextMenu = function(menu) {
//      var cond = this.getObject();
//      menu.add("header:"+ cond._typename + "::" + cond.fName);
//      function select(name,exec) {
//         var cond = this.getObject();
//         cond[name] = !cond[name];
//         this.submitCanvExec(exec + (cond[name] ? '(true)' : '(false)'));
//         this.redraw();
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

   GO4.ConditionPainter.prototype.processTooltipEvent = function(pnt) {
      if (!pnt) return null;

      var cond = this.getObject();

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

   if (JSROOT._) {
      GO4.ConditionPainter.prototype.redrawObject = function(obj) {
         if (!this.updateObject(obj)) return false;
         this.redraw(); // no need to redraw complete pad
         return true;
      }
      GO4.ConditionPainter.prototype.cleanup = function(arg) {
         if (this.pave) {
            var pp = findPainter(this, this.pave);
            if (pp) {
               pp.removeFromPadPrimitives();
               pp.cleanup();
            }
            delete this.pave;
         }

         ObjectPainter.prototype.cleanup.call(this, arg);
      }

   } else {
      // support older jsroot v5 interface
      GO4.ConditionPainter.prototype.RedrawObject = function(obj) {
         if (!this.UpdateObject(obj)) return false;
         this.redraw(); // no need to redraw complete pad
         return true;
      }
      GO4.ConditionPainter.prototype.Cleanup = function(arg) {
         if (this.pave) {
            var pp = findPainter(this, this.pave);
            if (pp) pp.DeleteThis();
            delete this.pave;
         }

         ObjectPainter.prototype.Cleanup.call(this, arg);
      }
      GO4.ConditionPainter.prototype.ProcessTooltip = GO4.ConditionPainter.prototype.processTooltipEvent;
   }


   GO4.ConditionPainter.prototype.redraw = function() {
      this.drawCondition();
      this.drawLabel();
   }

   // only to support old jsroot v5 interface
   if (!JSROOT._) GO4.ConditionPainter.prototype.Redraw = GO4.ConditionPainter.prototype.redraw;

   GO4.drawGo4Cond = function(divid, cond, option) {

      if (!option) option = "";

      let condpainter = new GO4.ConditionPainter(divid, cond),
          realid = condpainter.getDomId();

      if (GO4.web_canvas || (option.indexOf('same') >= 0) || condpainter.getMainPainter()) {
         condpainter.drawCondition();
         condpainter.drawLabel();
         if (JSROOT._) {
            condpainter.addToPadPrimitives();
            return Promise.resolve(condpainter);
         }
         condpainter.SetDivId(divid); // old
         return condpainter.DrawingReady();
      }

      // from here normal code for plain THttpServer

      if (((cond.fxHistoName=="") || (option=='editor')) && GO4.ConditionEditor) {
         // $('#'+divid).append("<br/>Histogram name not specified");
         var h = $("#"+realid).height(), w = $("#"+realid).width();
         if ((h<10) && (w>10)) $("#"+realid).height(w*0.4);
         var editor = new GO4.ConditionEditor(realid, cond);
         if (JSROOT._) return new Promise(resolve => editor.drawEditor(realid, resolve));
         return editor.drawEditor(realid);
      }

      // $('#'+realid).append("<br/>Histogram name is " + cond.fxHistoName);

      if (!JSROOT.hpainter) {
         $('#'+realid).append("<br/>Error - did not found hierarchy painter");
         return;
      }

      var histofullpath = null;

      function TestItem(h) {
         if ((h._name == cond.fxHistoName) && h._kind && (h._kind.indexOf("ROOT.TH")==0))
            histofullpath = JSROOT.hpainter.itemFullName(h);
      }

      if (JSROOT._)
         JSROOT.hpainter.forEachItem(TestItem);
      else
         JSROOT.hpainter.ForEach(TestItem);

      if (histofullpath === null) {
         $('#'+realid).append("<br/>Error - did not found histogram " + cond.fxHistoName);

         histofullpath = "../../Histograms/" + cond.fxHistoName;

         let hitem = JSROOT._ ? JSROOT.hpainter.findItem({ name: histofullpath, force: true }) : JSROOT.hpainter.Find({ name: histofullpath, force: true });

         hitem._kind = "ROOT.TH1I";

         console.log("Try histogram" + histofullpath);
      }


      $('#'+realid).append("<br/>Drawing histogram " + histofullpath);
      $('#'+realid).empty();

      function drawCond(hpainter) {
         if (!hpainter) return console.log("fail to draw histogram " + histofullpath);
         condpainter.drawCondition();
         condpainter.drawLabel();
         if (JSROOT._) {
            condpainter.addToPadPrimitives();
            return condpainter;
         } else {
            condpainter.SetDivId(divid); // old
            return condpainter.DrawingReady();
         }
      }

      if (JSROOT._)
         return JSROOT.hpainter.display(histofullpath, "divid:" + realid).then(hp => drawCond(hp));

      JSROOT.hpainter.display(histofullpath, "divid:" + realid, drawCond);
      return condpainter;
   }

   GO4.drawCondArray = function(divid, obj, option) {
      let arr = obj.condarr.arr,
          num = obj.fiNumCond,
          first;
      for (var k = 0; k < num; ++k) {
         var p = GO4.drawGo4Cond(divid, arr[k], "");
         if (k == 0) first = p;
      }
      return first; // return first condition as result of drawing
   }

   // =======================================================================

   if (GO4.web_canvas) {
      let jsrp = JSROOT._ ? JSROOT.Painter : JSROOT;
      jsrp.addDrawFunc({ name: "TGo4Marker", func: GO4.drawGo4Marker });
      jsrp.addDrawFunc({ name: "TGo4WinCond", func: GO4.drawGo4Cond });
      jsrp.addDrawFunc({ name: "TGo4PolyCond", func: GO4.drawGo4Cond });
      jsrp.addDrawFunc({ name: "TGo4ShapedCond", func: GO4.drawGo4Cond });
      jsrp.addDrawFunc({ name: "TGo4CondArray", func: GO4.drawCondArray });
   }

   return GO4;

}));
