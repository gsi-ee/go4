// $Id$

"use strict";

JSROOT.require("painter").then(jsrp => {
   // cannot use JSROOT.define syntax here

   if (typeof GO4 == 'undefined')
      globalThis.GO4 = { version: "6.1.4", web_canvas: true, id_counter: 1 };

   function findPainter(painter, obj, name, typ) {
      let pp = painter.getPadPainter();
      return pp ? pp.findPainterFor(obj, name, typ) : null;
   }

   class MarkerPainter extends JSROOT.ObjectPainter{
      constructor(dom, marker) {
         super(dom, marker);
         this.pave = null; // drawing of stat
      }

      moveDrag(dx,dy) {
         this.grx += dx;
         this.gry += dy;
         this.draw_g.select('path').attr("d",this.markeratt.create(this.grx, this.gry));
      }

      moveEnd() {
         let marker = this.getObject();
         marker.fX = this.svgToAxis("x", this.grx);
         marker.fY = this.svgToAxis("y", this.gry);
         let exec = `SetXY(${marker.fX},${marker.fY})`;
         this.submitCanvExec(exec);
         this.drawLabel();
      }

      drawMarker() {
         let g = this.createG(); // can draw in complete pad

         let marker = this.getObject();

         this.createAttMarker({ attr: marker });

         this.grx = this.axisToSvg("x", marker.fX);
         this.gry = this.axisToSvg("y", marker.fY);

         let path = this.markeratt.create(this.grx, this.gry);

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

      fillLabels(marker) {
         let lbls = [],
             rect = this.getFramePainter().getFrameRect(),
             main = this.getMainPainter(), hint = null;

         if (main && typeof main.processTooltipEvent == 'function')
            hint = main.processTooltipEvent({ enabled: false, x: this.grx - rect.x, y: this.gry - rect.y });

         lbls.push(marker.fxName + ((hint && hint.name) ? (" : " + hint.name) : ""));

         if (marker.fbXDraw)
             lbls.push("X = " + jsrp.floatToString(marker.fX, "6.4g"));

         if (marker.fbYDraw)
            lbls.push("Y = " + jsrp.floatToString(marker.fY, "6.4g"));

         if (hint && hint.user_info) {
            if (marker.fbXbinDraw) {
               let bin = "<undef>";
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

      drawLabel() {

         let marker = this.getObject();

         if (!marker.fbHasLabel) return;

         let pave_painter = findPainter(this, this.pave);

         if (!pave_painter) {
            this.pave = JSROOT.create("TPaveStats");
            this.pave.fName = "stats_" + marker.fName;

            let pp = this.getPadPainter(),
                pad_width = pp.getPadWidth(),
                pad_height = pp.getPadHeight();

            let px = this.grx / pad_width + 0.02,
                py = this.gry / pad_height - 0.02;
            JSROOT.extend(this.pave, { fX1NDC: px, fY1NDC: py - 0.15, fX2NDC: px + 0.2, fY2NDC: py, fBorderSize: 1, fFillColor: 0, fFillStyle: 1001 });

            let st = JSROOT.gStyle;
            JSROOT.extend(this.pave, { fFillColor: st.fStatColor, fFillStyle: st.fStatStyle, fTextAngle: 0, fTextSize: st.fStatFontSize,
                                       fTextAlign: 12, fTextColor: st.fStatTextColor, fTextFont: st.fStatFont });
         } else {
            this.pave.Clear();
         }

         let lbls = this.fillLabels(marker);
         for (let k = 0; k < lbls.length; ++k)
            this.pave.AddText(lbls[k]);

         if (pave_painter)
            pave_painter.redraw();
         else
            JSROOT.draw(this.divid, this.pave, "").then(p => { if (p) p.$secondary = true; });
      }

      redrawObject(obj) {
         if (!this.updateObject(obj)) return false;
         this.redraw(); // no need to redraw complete pad
         return true;
      }

      cleanup(arg) {
         if (this.pave) {
            let pp = findPainter(this, this.pave);
            if (pp) {
               pp.removeFromPadPrimitives();
               pp.cleanup();
            }
            delete this.pave;
         }

         super.cleanup(arg);
      }

      redraw() {
         this.drawMarker();
         this.drawLabel();
      }

   //   fillContextMenu(menu) {
   //      let marker = this.getObject();
   //      menu.add("header:"+ marker._typename + "::" + marker.fxName);
   //      function select(name,exec) {
   //         let marker = this.getObject();
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

      processTooltipEvent(pnt) {
         if (!pnt) return null;

         let marker = this.getObject(),
             rect = this.getFramePainter().getFrameRect(),
             fx = rect.x,
             fy = rect.y,
             marker_sz = this.markeratt.getFullSize();

         let hint = { name: marker.fxName,
                      title: marker.fxName,
                      painter: this,
                      menu: true,
                      x: this.grx - fx,
                      y: this.gry - fy,
                      color1: this.markeratt.color };

         let dist = Math.sqrt(Math.pow(pnt.x - hint.x, 2) + Math.pow(pnt.y - hint.y, 2));

         hint.menu_dist = dist;

         if (dist < 2.5 * marker_sz) hint.exact = true;

         if (hint.exact)
            hint.lines = this.fillLabels(marker);

         // res.menu = res.exact; // activate menu only when exactly locate bin
         // res.menu_dist = 3; // distance always fixed

         return hint;
      }

      ShowTooltip(hint) {}
   }

   GO4.drawGo4Marker = function(dom, obj /*, option */) {
      let painter = new MarkerPainter(dom, obj);
      painter.drawMarker();
      painter.drawLabel();
      painter.addToPadPrimitives();
      return Promise.resolve(painter);
   }

   // =========================================================================

   class ConditionPainter extends JSROOT.ObjectPainter {
      constructor(dom, cond) {
         super(dom, cond);
         this.pave = null; // drawing of stat
      }

      Test(x,y) {
         //  JAM: need to put this here, since condition object will lose internal definition after cloning it again!
         let cond = this.getObject();
         if (!cond.fbEnabled)
            return cond.fbResult;

         if (cond.fxCut)
            return cond.fxCut.IsInside(x,y) ? cond.fbTrue : cond.fbFalse;

         if ((x < cond.fLow1) || (x > cond.fUp1)) return cond.fbFalse;

         if ((cond.fiDim==2) && ((y < cond.fLow2) || (y > cond.fUp2))) return cond.fbFalse;

         return cond.fbTrue;
      }


      isPolyCond() {
         return this.matchObjectType("TGo4PolyCond") || this.matchObjectType("TGo4ShapedCond");
      }

      isEllipseCond() {
         return this.matchObjectType("TGo4ShapedCond");
      }

      afterCutDraw(p) {
         if (!p || !this.snapid || p._oldexec) return;
         p.snapid = this.snapid + "#member_fxCut";
         p._condpainter = this;

         // catch TCutG exec and mark condition as modified
         p._oldexec = p.submitCanvExec;
         p.submitCanvExec = function(exec, arg) {
            this._oldexec(exec, arg);
            p._condpainter.submitCanvExec("SetChanged()");
         }
      }

      drawCondition(interactive) {

         let cond = this.getObject();

         if (!cond || !cond.fbVisible) return;

         if (this.isPolyCond()) {
            if (cond.fxCut) {
               // look here if cut is already drawn in divid:
               let cutpaint = findPainter(this, null, cond.fName, 'TCutG');

               if (cutpaint) {
                  if (cutpaint.updateObject(cond.fxCut)) cutpaint.redraw();
                  this.afterCutDraw(cutpaint);
               } else {
                  cond.fxCut.fFillStyle = 3006;
                  cond.fxCut.fFillColor = 2;
                  JSROOT.draw(this.divid, cond.fxCut, "LF").then(p => this.afterCutDraw(p));
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
            this.gry2 = this.getFramePainter().getFrameHeight();
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

      moveStart(x,y) {
         this.swapx = this.swapy = false;
         this.dx1 = Math.abs(x-this.grx1) < 5;
         this.dx2 = Math.abs(x-this.grx2) < 5;
         this.dy1 = Math.abs(y-this.gry1) < 5;
         this.dy2 = Math.abs(y-this.gry2) < 5;
         if (!this.dx1 && !this.dx2 && !this.dy1 && !this.dy2)
            this.dx1 = this.dx2 = this.dy1 = this.dy2 = true;
         if (!this.candy) this.dy1 = this.dy2 = false;
      }

      moveDrag(dx,dy) {
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

      moveEnd() {
         let cond = this.getObject(), exec = "";
         if (this.dx1 || this.swapx) { cond.fLow1 = this.svgToAxis("x", this.grx1); exec += "SetXLow(" + cond.fLow1 + ");;"; }
         if (this.dx2 || this.swapx) { cond.fUp1 = this.svgToAxis("x", this.grx2); exec += "SetXUp(" + cond.fUp1 + ");;"; }
         if (this.dy2 || this.swapy) { cond.fLow2 = this.svgToAxis("y", this.gry2); exec += "SetYLow(" + cond.fLow2 + ");;"; }
         if (this.dy1 || this.swapy) { cond.fUp2 = this.svgToAxis("y", this.gry1); exec += "SetYUp(" + cond.fUp2 + ");;"; }
         if (exec) {
            this.submitCanvExec(exec + "SetChanged()");
            this.drawLabel();
         }
      }

      drawLabel() {

         let cond = this.getObject(), painter = this, stat = {};

         if (!cond.fbLabelDraw || !cond.fbVisible) return;

         let pave_painter = findPainter(this, this.pave);

         if (!pave_painter) {
            this.pave = JSROOT.create("TPaveStats");
            this.pave.fName = "stats_" + cond.fName;
            JSROOT.extend(this.pave, { fX1NDC: 0.1, fY1NDC: 0.4, fX2NDC: 0.4, fY2NDC: 0.65, fBorderSize: 1, fFillColor: 0, fFillStyle: 1001 });

            let st = JSROOT.gStyle;
            JSROOT.extend(this.pave, { fFillColor: st.fStatColor, fFillStyle: st.fStatStyle, fTextAngle: 0, fTextSize: st.fStatFontSize,
                                       fTextAlign: 12, fTextColor: st.fStatTextColor, fTextFont: st.fStatFont});
         } else {
            this.pave.Clear();
         }

         this.pave.AddText(cond.fName);

         this.pave.AddText("Counts = " + cond.fiCounts);

         if (cond.fbLimitsDraw)
            if (this.isPolyCond()) {
               let res = { xmin: 0, xmax: 0, ymin: 0, ymax: 0 };
               if (cond.fxCut.fNpoints > 0) {
                  res.xmin = res.xmax = cond.fxCut.fX[0];
                  res.ymin = res.ymax = cond.fxCut.fY[0];
                  for (let i=1; i<cond.fxCut.fNpoints; i++) {
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

         stat = this.getMainPainter().countStat((x,y) => painter.Test(x,y));

         if (cond.fbIntDraw) this.pave.AddText("Integral = " + jsrp.floatToString(stat.integral, "14.7g"));

         if (cond.fbXMeanDraw) this.pave.AddText("Mean x = " + jsrp.floatToString(stat.meanx, "6.4g"));

         if (cond.fbXRMSDraw) this.pave.AddText("RMS x = " + jsrp.floatToString(stat.rmsx, "6.4g"));

         if (cond.fiDim==2) {
            if (cond.fbYMeanDraw) this.pave.AddText("Mean y = " + jsrp.floatToString(stat.meany, "6.4g"));
            if (cond.fbYRMSDraw) this.pave.AddText("RMS y = " + jsrp.floatToString(stat.rmsy, "6.4g"));
         }

         if (cond.fbXMaxDraw) this.pave.AddText("X max = " + jsrp.floatToString(stat.xmax, "6.4g"));

         if (cond.fiDim==2)
            if (cond.fbYMaxDraw) this.pave.AddText("Y max = " + jsrp.floatToString(stat.ymax, "6.4g"));
         if (cond.fbCMaxDraw) this.pave.AddText("C max = " + jsrp.floatToString(stat.wmax, "14.7g"));

         if (!pave_painter)
            JSROOT.draw(this.divid, this.pave, "");
         else
            pave_painter.redraw();
      }

   //   fillContextMenu(menu) {
   //      let cond = this.getObject();
   //      menu.add("header:"+ cond._typename + "::" + cond.fName);
   //      function select(name,exec) {
   //         let cond = this.getObject();
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

      processTooltipEvent(pnt) {
         if (!pnt) return null;

         let cond = this.getObject(),
             hint = { name: cond.fName,
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

      redrawObject(obj) {
         if (!this.updateObject(obj)) return false;
         this.redraw(); // no need to redraw complete pad
         return true;
      }

      cleanup(arg) {
         if (this.pave) {
            let pp = findPainter(this, this.pave);
            if (pp) {
               pp.removeFromPadPrimitives();
               pp.cleanup();
            }
            delete this.pave;
         }
         super.cleanup(arg);
      }

      redraw() {
         this.drawCondition();
         this.drawLabel();
      }

      getDomId() {
         let elem = this.selectDom();
         if (elem.empty()) return "";
         let id = elem.attr("id");
         if (!id) {
            id = "go4_element_" + GO4.id_counter++;
            elem.attr("id", id);
         }
         return id;
      }

   }

   GO4.drawGo4Cond = function(dom, cond, option) {

      if (!option) option = "";

      let condpainter = new ConditionPainter(dom, cond),
          elem = condpainter.selectDom();

      if (GO4.web_canvas || (option.indexOf('same') >= 0) || condpainter.getMainPainter()) {
         condpainter.drawCondition();
         condpainter.drawLabel();
         condpainter.addToPadPrimitives();
         return Promise.resolve(condpainter);
      }

      // from here normal code for plain THttpServer
      if ((option=='editor') || !cond.fxHistoName) {
         // failure, should never happens!
         if (!GO4.source_dir) return null;

         let rect = elem.node().getBoundingClientRect();
         if ((rect.height < 10) && (rect.width > 10)) elem.style("height", Math.round(rect.width*0.4) + "px");
         return JSROOT.require(GO4.source_dir + 'html/condition.js').then(() => {
            let editor = new GO4.ConditionEditor(dom, cond);
            return editor.drawEditor();
         });
      }

      if (!JSROOT.hpainter) return;

      let histofullpath = null;

      JSROOT.hpainter.forEachItem(h => {
         if ((h._name == cond.fxHistoName) && h._kind && (h._kind.indexOf("ROOT.TH")==0))
            histofullpath = JSROOT.hpainter.itemFullName(h);
      });

      if (histofullpath === null) {
         histofullpath = "../../Histograms/" + cond.fxHistoName;

         let hitem = JSROOT.hpainter.findItem({ name: histofullpath, force: true });

         hitem._kind = "ROOT.TH1I";
      }

      function drawCond(hpainter) {
         if (!hpainter) return console.log("fail to draw histogram " + histofullpath);
         condpainter.drawCondition();
         condpainter.drawLabel();
         condpainter.addToPadPrimitives();
         return condpainter;
      }

      return JSROOT.hpainter.display(histofullpath, "divid:" + condpainter.getDomId()).then(hp => drawCond(hp));
   }

   GO4.drawCondArray = function(divid, obj, option) {
      let arr = obj.condarr.arr,
          num = obj.fiNumCond,
          first;
      for (let k = 0; k < num; ++k) {
         let p = GO4.drawGo4Cond(divid, arr[k], "");
         if (k == 0) first = p;
      }
      return first; // return first condition as result of drawing
   }

   // =======================================================================

   if (GO4.web_canvas) {
      jsrp.addDrawFunc({ name: "TGo4Marker", func: GO4.drawGo4Marker });
      jsrp.addDrawFunc({ name: "TGo4WinCond", func: GO4.drawGo4Cond });
      jsrp.addDrawFunc({ name: "TGo4PolyCond", func: GO4.drawGo4Cond });
      jsrp.addDrawFunc({ name: "TGo4ShapedCond", func: GO4.drawGo4Cond });
      jsrp.addDrawFunc({ name: "TGo4CondArray", func: GO4.drawCondArray });
   }

});
