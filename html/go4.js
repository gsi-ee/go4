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
   
   GO4.ConditionPainter = function(cond, aseditor) {
      JSROOT.TObjectPainter.call(this, cond);
      this.cond = cond;
      this.aseditor = aseditor;
   }

   GO4.ConditionPainter.prototype = Object.create(JSROOT.TObjectPainter.prototype);

   GO4.ConditionPainter.prototype.GetObject = function() {
      return this.cond;
   }
   
   GO4.ConditionPainter.prototype.isPolyCond = function() {
      return this.cond._typename == "TGo4PolyCond"; 
   }
   
   GO4.ConditionPainter.prototype.refreshEditor = function() {
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
   
   GO4.ConditionPainter.prototype.fillEditor = function() {
      var id = "#"+this.divid;
      console.log("GO4.ConditionPainter.prototype.fillEditor " + this.cond.fName);
      $(id).css("display","table");
      
      $(id+" .cond_tabs").tabs();
      
      $(id+" .cond_execmode").selectmenu();
      $(id+" .cond_invertmode").selectmenu();
      
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
   
   GO4.ConditionPainter.prototype.drawEditor = function() {
      var pthis = this;
       
      $("#"+this.divid).empty();
      $("#"+this.divid).load("/go4sys/html/condeditor.htm", "", 
            function() { pthis.fillEditor(); });
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
      
      this.pave = JSROOT.Create("TPaveStats");
      
      jQuery.extend(this.pave, { fX1NDC: 0.1, fY1NDC: 0.4, fX2NDC: 0.4, fY2NDC: 0.65, fBorderSize: 1 });
      jQuery.extend(this.pave, JSROOT.gStyle.StatText);
      jQuery.extend(this.pave, JSROOT.gStyle.StatFill);
      
      this.pave.AddText(this.cond.fName);
      
      if (this.cond.fbLimitsDraw)
         if (this.isPolyCond()) {
            this.pave.AddText("Xmin = " + Math.min.apply(Math, this.cond.fxCut.fX));
            this.pave.AddText("Xmax = " + Math.max.apply(Math, this.cond.fxCut.fX));
            this.pave.AddText("Ymin = " + Math.min.apply(Math, this.cond.fxCut.fY));
            this.pave.AddText("Ymax = " + Math.max.apply(Math, this.cond.fxCut.fY));
         } else {
            this.pave.AddText("Xmin = " + this.cond.fLow1);
            this.pave.AddText("Xmax = " + this.cond.fUp1);
            if (this.cond.fiDim==2) {
               this.pave.AddText("Ymin = " + this.cond.fLow2);
               this.pave.AddText("Ymax = " + this.cond.fUp2);
            }
         }
      
      JSROOT.draw(this.divid, this.pave, ""); 
   }

   GO4.ConditionPainter.prototype.Redraw = function() {
      if (this.aseditor) 
         $("#" + this.divid).append("<br/>Redraw not implemented");
      else 
         this.drawCondition();
   }

   
   GO4.drawGo4Cond = function(divid, cond, option) {
      $('#'+divid).append("Here will be condition " + cond._typename);
      
      if ((cond.fxHistoName=="") || (option=='editor')) {
         $('#'+divid).append("<br/>Histogram name not specified");
         var painter = new GO4.ConditionPainter(cond, true);
         painter.SetDivId(divid);
         painter.drawEditor();
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

      $('#'+divid).append("<br/>Loading histogram " + histofullpath);
      
      var painter = new GO4.ConditionPainter(cond, false);
      
      dabc.get(histofullpath, function(item, obj) {
         $('#'+divid).empty();
         JSROOT.draw(divid, obj, /* obj.fDimension==2 ? "col" : */ "");
         
         painter.SetDivId(divid);
         painter.drawCondition();
         
         painter.drawLabel();
         
      });

      return painter;
   }
   
   JSROOT.addDrawFunc("TGo4WinCond", GO4.drawGo4Cond);
   JSROOT.addDrawFunc("TGo4PolyCond", GO4.drawGo4Cond);
   
   
   GO4.ParameterPainter = function(par, aseditor) {
      JSROOT.TObjectPainter.call(this, par);
      this.par = par;
      this.aseditor = aseditor;
   }

   GO4.ParameterPainter.prototype = Object.create(JSROOT.TObjectPainter.prototype);

   GO4.ParameterPainter.prototype.GetObject = function() {
      return this.par;
   }

   GO4.ParameterPainter.prototype.fillEditor = function() {
      var id = "#"+this.divid;
      var par = this.par;
      
      $(id).css("display","table");
      $(id+" .par_name").text(par.fName);
      $(id+" .par_type").text(par._typename);

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
         var value = (par[key]!=null ? par[key].toString() : "null");
         $(id + " .par_values tbody").append('<tr><td>' + key.toString() + "</td><td><input type='text' value='" + value + "'/></td></tr>");
      }
   }
   
   GO4.ParameterPainter.prototype.drawEditor = function() {
      var pthis = this;
       
      $("#"+this.divid).empty();
      $("#"+this.divid).load("/go4sys/html/pareditor.htm", "", 
            function() { pthis.fillEditor(); });
   }

   GO4.drawParameter = function(divid, par, option) {
      var painter = new GO4.ParameterPainter(par, true);
      painter.SetDivId(divid);
      painter.drawEditor();
      return painter;
   }

})();
