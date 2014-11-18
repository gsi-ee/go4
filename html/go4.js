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
   
   GO4.ConditionEditor = function(cond) {
      JSROOT.TBasePainter.call(this, cond);
      this.cond = cond;
   }
   
   GO4.ConditionEditor.prototype = Object.create(JSROOT.TBasePainter.prototype);

   GO4.ConditionEditor.prototype.CheckResize = function() {
      var id = "#"+this.divid;
      var width = $(id).width(); 
      var height = $(id).height();
      
      $(id).children().eq(0).width(width - 5).height(height - 5);
   }
   
   GO4.ConditionEditor.prototype.refreshEditor = function() {
      var id = "#"+this.divid;
      var cond = this.cond;

      $(id+" .cond_name").text(cond.fName);
      $(id+" .cond_type").text(cond._typename);
      
      $(id+" .cond_xmin").val(cond.fLow1).change(function(){ $(id+" button:eq(2)").show(); });
      $(id+" .cond_xmax").val(cond.fUp1).change(function(){ $(id+" button:eq(2)").show(); });
      if (cond.fiDim==2) {
         $(id+" .cond_ymin").val(cond.fLow2).change(function(){ $(id+" button:eq(2)").show(); });
         $(id+" .cond_ymax").val(cond.fUp2).change(function(){ $(id+" button:eq(2)").show(); });
      } else {
         $(id+" .cond_ymin").prop('disabled', true);
         $(id+" .cond_ymax").prop('disabled', true);
      }
      
      $(id+" .cond_counts").text(cond.fiCounts);
      $(id+" .cond_true").text(cond.fiTrueCounts);
      $(id+" .cond_percent").text((cond.fiCounts > 0 ? 100. * cond.fiTrueCounts / cond.fiCounts : 0.).toFixed(2) + "%");
      
      $(id+" .cond_visible")
         .prop('checked', cond.fbVisible)
         .click(function() { cond.fbVisible = this.checked; });
      $(id+" .cond_limits")
         .prop('checked', cond.fbLimitsDraw)
         .click(function() { cond.fbLimitsDraw = this.checked; });
         
      $(id+" .cond_label")
         .prop('checked', cond.fbLabelDraw)
         .click(function() { cond.fbLabelDraw = this.checked; });
   }
   
   GO4.ConditionEditor.prototype.fillEditor = function() {
      var id = "#"+this.divid;
      console.log("GO4.ConditionPainter.prototype.fillEditor " + this.cond.fName);
      // $(id).css("display","table");
      
      this.CheckResize();
      
      $(id+" .cond_tabs").tabs();
      
      $(id+" .cond_execmode").selectmenu();
      $(id+" .cond_invertmode").selectmenu();
      
      var editor = this;
      
      var dabc = JSROOT.H('dabc');
      
      $(id+" button:first")
         .text("")
         .append('<img src="/go4sys/icons/right.png"  height="16" width="16"/>')
         .button()
         .click(function() {
            console.log("get - do nothing item = " + editor._hitemname); 
            if (dabc) dabc.display(editor._hitemname, "update"); 
          })
         .next()
         .text("")
         .append('<img src="/go4sys/icons/left.png"  height="16" width="16"/>')
         .button()
         .click(function() { console.log("set - do nothing"); })
         .next()
         .text("")
         .append('<img src="/go4sys/icons/info1.png"  height="16" width="16"/>')
         .button()
         .click(function() { console.log("warn - do nothing"); })
         .hide()
         .next()
         .text("")
         .append('<img src="/go4sys/icons/clear.png"  height="16" width="16"/>')
         .button()
         .click(function() { console.log("clear - do nothing"); })
         .next()
         .text("")
         .append('<img src="/go4sys/icons/chart.png"  height="16" width="16"/>')
         .button()
         .click(function() { console.log("draw - do nothing"); });
      
      this.refreshEditor();   
   }

   GO4.ConditionEditor.prototype.drawEditor = function(divid) {
      var pthis = this;
       
      $("#"+divid).empty();
      $("#"+divid).load("/go4sys/html/condeditor.htm", "", 
            function() { pthis.SetDivId(divid); pthis.fillEditor();  });
   }
   
   GO4.ConditionEditor.prototype.RedrawPad = function(resize) {
      this.refreshEditor();
   }

   GO4.ConditionEditor.prototype.UpdateObject = function(obj) {
      if (obj._typename != this.cond._typename) return false;
      
      this.cond.fiCounts = obj.fiCounts;
      this.cond.fiTrueCounts = obj.fiTrueCounts;
      return true;
   }

   
   // ==================================================================
   
   GO4.ConditionPainter = function(cond) {
      JSROOT.TObjectPainter.call(this, cond);
      this.cond = cond;
      this.pave = null; // drawing of stat
   }

   GO4.ConditionPainter.prototype = Object.create(JSROOT.TObjectPainter.prototype);

   GO4.ConditionPainter.prototype.GetObject = function() {
      return this.cond;
   }
   
   GO4.ConditionPainter.prototype.isPolyCond = function() {
      return this.cond._typename == "TGo4PolyCond"; 
   }
   
   GO4.ConditionPainter.prototype.drawCondition = function() {
      
      if (this.isPolyCond()) {
         if (this.cond.fxCut != null) {
            this.cond.fxCut.fFillStyle = 3006;
            this.cond.fxCut.fFillColor = 2;
            JSROOT.draw(this.divid, this.cond.fxCut, "LF");
         }
         return;
      }
      
      this.RecreateDrawG(false);
      
      var w = Number(this.svg_frame(true).attr("width")),
          h = Number(this.svg_frame(true).attr("height"));
      
      var x = this.main_painter().x;
      var y = this.main_painter().y;
      
      var line_color = JSROOT.Painter.root_colors[this.cond['fLineColor']];
      var line_width = this.cond['fLineWidth'];
      var line_style = JSROOT.Painter.root_line_styles[this.cond['fLineStyle']];

      if ((this.cond['fFillStyle']==1001) && (this.cond['fFillColor']==19)) { 
         this.cond['fFillStyle'] = 3006;
         this.cond['fFillColor'] = 2;
      }
      
      var fill_color = JSROOT.Painter.createFillPattern(this.svg_canvas(true), this.cond['fFillStyle'], this.cond['fFillColor']);
      
      this.draw_g.attr("class","cond_container");
      
      var ndim = this.cond.fiDim;
      
      this.draw_g.append("svg:rect")
             .attr("x", x(this.cond.fLow1))
             .attr("y", (ndim==1) ? 0 : y(this.cond.fUp2))
             .attr("width", x(this.cond.fUp1) - x(this.cond.fLow1))
             .attr("height", (ndim==1) ? h : y(this.cond.fLow2) - y(this.cond.fUp2))
             .style("fill", fill_color)
             .style("stroke", line_color)
             .style("stroke-width", line_width)
             .style("stroke-dasharray", line_style);
   }
   
   GO4.ConditionPainter.prototype.drawLabel = function() {
      if (!this.cond.fbLabelDraw) return;
      
      var pave_painter = this.FindPainterFor(this.pave);
      
      if (pave_painter == null) {
         this.pave = JSROOT.Create("TPaveStats");
         this.pave.fName = "stat_" + this.cond.fName; 
         jQuery.extend(this.pave, { fX1NDC: 0.1, fY1NDC: 0.4, fX2NDC: 0.4, fY2NDC: 0.65, fBorderSize: 1 });
         jQuery.extend(this.pave, JSROOT.gStyle.StatText);
         jQuery.extend(this.pave, JSROOT.gStyle.StatFill);
      } else {
         this.pave.Clear();
      }
      
      this.pave.AddText(this.cond.fName);

      this.pave.AddText("Counts = " + this.cond.fiCounts);
      
      if (this.cond.fbLimitsDraw)
         if (this.isPolyCond()) {
            var r = this.cond.fxCut.ComputeRange();
            this.pave.AddText("Xmin = " + r.xmin);
            this.pave.AddText("Xmax = " + r.xmax);
            this.pave.AddText("Ymin = " + r.ymin);
            this.pave.AddText("Ymax = " + r.ymax);
         } else {
            this.pave.AddText("Xmin = " + this.cond.fLow1);
            this.pave.AddText("Xmax = " + this.cond.fUp1);
            if (this.cond.fiDim==2) {
               this.pave.AddText("Ymin = " + this.cond.fLow2);
               this.pave.AddText("Ymax = " + this.cond.fUp2);
            }
         }
      
      var cond = this.cond;
      
      var stat = this.main_painter().CountStat(function(x,y) { return cond.Test(x,y); });
      
      if (this.cond.fbIntDraw) this.pave.AddText("Integral = " + JSROOT.gStyle.StatFormat(stat.integral));
      
      if (this.cond.fbXMeanDraw) this.pave.AddText("Mean x = " + JSROOT.gStyle.StatFormat(stat.meanx));
      
      if (this.cond.fbXRMSDraw) this.pave.AddText("RMS x = " + JSROOT.gStyle.StatFormat(stat.rmsx));
      
      if (this.cond.fiDim==2) {
         if (this.cond.fbYMeanDraw) this.pave.AddText("Mean y = " + JSROOT.gStyle.StatFormat(stat.meany));
         if (this.cond.fbYRMSDraw) this.pave.AddText("RMS y = " + JSROOT.gStyle.StatFormat(stat.rmsy));
      }
      
      if (this.cond.fbXMaxDraw) this.pave.AddText("X max = " + JSROOT.gStyle.StatFormat(stat.xmax));
      
      if (this.cond.fiDim==2) 
         if (this.cond.fbYMaxDraw) this.pave.AddText("Y max = " + JSROOT.gStyle.StatFormat(stat.ymax));
      if (this.cond.fbCMaxDraw) this.pave.AddText("C max = " + JSROOT.gStyle.StatFormat(stat.wmax));
      
      if (pave_painter == null) 
         pave_painter = JSROOT.draw(this.divid, this.pave, "");
      else
         pave_painter.Redraw();
   }
   
   GO4.ConditionPainter.prototype.RedrawObject = function(obj) {
      if (this.UpdateObject(obj)) 
         this.Redraw(); // no need to redraw complete pad
   }

   GO4.ConditionPainter.prototype.UpdateObject = function(obj) {
      if (obj._typename != this.cond._typename) return false;
      this.cond.fiCounts = obj.fiCounts;
      return true;
   }
   
   GO4.ConditionPainter.prototype.Redraw = function() {
      this.drawCondition();
      this.drawLabel();
   }
   
   GO4.drawGo4Cond = function(divid, cond, option) {
      $('#'+divid).append("Here will be condition " + cond._typename);
      
      cond['Test'] = function(x,y) {
         if (!this.fbEnabled) return this.fbResult;
         
         if (this.fxCut)
            return this.fxCut.IsInside(x,y) ? this.fbTrue : this.fbFalse; 
         
         if ((x < this.fLow1) || (x > this.fUp1)) return this.fbFalse;
         
         if (this.fiDim==2)
            if ((y < this.fLow2) || (y > this.fUp2)) return this.fbFalse;
         
         return this.fbTrue;
      }
      
      if ((cond.fxHistoName=="") || (option=='editor')) {
         $('#'+divid).append("<br/>Histogram name not specified");
         var painter = new GO4.ConditionEditor(cond);
         painter.drawEditor(divid);
         return painter;
      }
      
      $('#'+divid).append("<br/>Histogram name is " + cond.fxHistoName);
      
      var dabc = JSROOT.H('dabc');
      if (dabc==null) {
         $('#'+divid).append("<br/>Error - did not found dabc painter");
         return;
      }
      
      var histofullpath = null;
      
      dabc.ForEach(function(h) {
         if ((h['_name'] == cond.fxHistoName) && (h['_kind'].indexOf("ROOT.TH")==0)) {
            histofullpath = dabc.itemFullName(h);
            return true;
         }
      });
      
      if (histofullpath == null) {
         $('#'+divid).append("<br/>Error - did not found histogram " + cond.fxHistoName);
         return;
      } 

      $('#'+divid).append("<br/>Drawing histogram " + histofullpath);
      
      $('#'+divid).empty();
      
      var condpainter = new GO4.ConditionPainter(cond, false);
      
      dabc.display(histofullpath, "divid:" + divid, function() {
         condpainter.SetDivId(divid);
         condpainter.drawCondition();
         condpainter.drawLabel();
      });

      return condpainter;
   }
   
   JSROOT.addDrawFunc("TGo4WinCond", GO4.drawGo4Cond);
   JSROOT.addDrawFunc("TGo4PolyCond", GO4.drawGo4Cond);
   
   
   GO4.ParameterEditor = function(par) {
      JSROOT.TBasePainter.call(this);
      this.par = par;
   }

   GO4.ParameterEditor.prototype = Object.create(JSROOT.TBasePainter.prototype);

   GO4.ParameterEditor.prototype.CheckResize = function() {
      var id = "#"+this.divid;
      var width = $(id).width(); 
      var height = $(id).height();
      $(id).children().eq(0).width(width - 4).height(height - 4);
   }
   
   GO4.ParameterEditor.prototype.fillEditor = function() {
      var id = "#"+this.divid;
      var par = this.par;
      
      var id = "#"+this.divid;
      var width = $(id).width(); 
      var height = $(id).height();
      
      $(id+" .par_name").text(par.fName);
      $(id+" .par_type").text(par._typename);
      
      $(id).children().eq(0).width(width - 4).height(height - 4);
      
      $(id+" button:first")
         .text("")
         .append('<img src="/go4sys/icons/right.png"  height="16" width="16"/>')
         .button()
         .click(function() { console.log("get - do nothing"); })
         .next()
         .text("")
         .append('<img src="/go4sys/icons/left.png"  height="16" width="16"/>')
         .button()
         .click(function() { console.log("set - do nothing"); })
         .next()
         .text("")
         .append('<img src="/go4sys/icons/info1.png"  height="16" width="16"/>')
         .button()
         .click(function() { console.log("warn - do nothing"); })
         .hide();
      
      var found_title = false;
      
      for (var key in par) {
         if (typeof par[key] == 'function') continue;
         if (key == 'fTitle') { found_title = true; continue; } 
         if (!found_title) continue;
         var value = (par[key]!=null ? (par[key] instanceof Array ? par[key] : par[key].toString()): "null");

         if (value instanceof Array) {
            for(i = 0; i < value.length; i++) {
               $(id + " .par_values tbody").append("<tr><th>" + key.toString() + "[" + i + "]</th><th><input type='text' value='" + value[i] + "'/></th></tr>");
            }
         } else {
            $(id + " .par_values tbody").append('<tr><th>' + key.toString() + "</th><th><input type='text' value='" + value + "'/></th></tr>");
         }
      }
   }
   
   GO4.ParameterEditor.prototype.drawEditor = function(divid) {
      var pthis = this;
       
      $("#"+divid).empty();
      $("#"+divid).load("/go4sys/html/pareditor.htm", "", 
            function() { pthis.SetDivId(divid); pthis.fillEditor(); });
   }

   GO4.drawParameter = function(divid, par, option) {
      var painter = new GO4.ParameterEditor(par);
      painter.drawEditor(divid);
      return painter;
   }

})();
