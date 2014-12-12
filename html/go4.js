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
      this.changes = ["dummy", "init"];
      this.ClearChanges();
   }
   
   GO4.ConditionEditor.prototype = Object.create(JSROOT.TBasePainter.prototype);

   
   GO4.ConditionEditor.prototype.DabcCommand = function(cmd, option, callback) {
		var xmlHttp = new XMLHttpRequest();
		// old using special dabc command
//		var pre = "/Go4/Go4Analysis/Status/"; // GetItemName() + "/exe.txt"
//		var suf = "/execute"; // exe.txt?method=UpdateFromUrl
//		var fullcom = pre + cmd + suf + "?" + option;
		
		var pre="";
		if (this.GetItemName()!=null) 
			  pre = this.GetItemName() + "/"; // suppress / if item name is empty
		pre +="exe.txt\?method=";
		var fullcom = pre + cmd + option;
		
		
		console.log(fullcom);
		xmlHttp.open('GET', fullcom, true);
		xmlHttp.onreadystatechange = function() {

			if (xmlHttp.readyState == 4) {
				console.log("DabcCommand completed.");
//				var reply = JSON.parse(xmlHttp.responseText); // this does not work with exe.txt JAM
//				console.log("Reply= %s", reply);
				callback(true); // todo: evaluate return values of reply
			}
		}
		xmlHttp.send(null);
	};
  
   // add identifier of changed element to list, make warning sign visible
	  GO4.ConditionEditor.prototype.MarkChanged = function(key) {
		  // first avoid duplicate keys:
		  var index;
		  for	(index = 0; index < this.changes.length; index++) {
				if (this.changes[index]== key) return;
			} 
		  this.changes.push(key);
		  console.log("Mark changed :%s", key);	
		  var id = "#"+this.divid; 
		  $(id+" button:eq(2)").show(); // show warning sign 
		  // todo: replace with self explaining name
	  }
	  
	  // clear changed elements' ist, make warning sign invisible  
GO4.ConditionEditor.prototype.ClearChanges = function() {
	var index;
	var len=this.changes.length;
	for	(index = 0; index < len ; index++) {
	    var removed=this.changes.pop();
	    console.log("Clear changes removed :%s", removed);	
	} 
	var id = "#"+this.divid; 
	  $(id+" button:eq(2)").hide(); // hide warning sign 
	  // todo: replace with self explaining name  
		  
	  }
	  
// scan changed value list and return optionstring to be send to server
GO4.ConditionEditor.prototype.EvaluateChanges = function(optionstring) {
	var id = "#"+this.divid; 
	var index;
	var len=this.changes.length;
	for	(index = 0; index < len ; index++) {
	    //var cursor=changes.pop();
		var key=this.changes[index];
		console.log("Evaluate change key:%s", key);	
		
		// here mapping of key to editor field:
		if(key=="limits")
			{
			 //var xmin=ParseFloat($(id+" .cond_xmin").value); // need to parse here? no
			 var xmin=$(id+" .cond_xmin")[0].value;
        	 var xmax=$(id+" .cond_xmax")[0].value;
//        	 optionstring +="&xmin=\""+xmin+"\"&xmax=\""+xmax+"\"";
        	 optionstring +="&xmin="+xmin+"&xmax="+xmax;
        	  if (this.cond.fiDim==2) {
        		  var ymin=$(id+" .cond_ymin")[0].value;
        		  var ymax=$(id+" .cond_ymax")[0].value; 
//        		  optionstring +="&ymin=\""+ymin+"\"&ymax="+ymax+"\"";
        		  optionstring +="&ymin="+ymin+"&ymax="+ymax;
        	  }
			}
		// TODO: already encode the keywords in fieldnames and replace everything below by one function
		
		else if (key=="resultmode"){
			var selected=$(id+" .cond_execmode")[0].value;
			optionstring +="&"+key+"="+selected;
		}
		else if (key=="invertmode"){
			var selected=$(id+" .cond_invertmode")[0].value;
			optionstring +="&"+key+"="+selected;
		}
		else if (key=="visible"){
			var checked=$(id+" .cond_visible")[0].checked;
			var arg= (checked ? "1" : "0");
			optionstring +="&"+key+"="+arg;
		}
		else if (key=="labeldraw"){
			var checked=$(id+" .cond_label")[0].checked;
			var arg= (checked ? "1" : "0");
			optionstring +="&"+key+"="+arg;
		}
		else if (key=="limitsdraw"){
			var checked=$(id+" .cond_limits")[0].checked;
			var arg= (checked ? "1" : "0");
			optionstring +="&"+key+"="+arg;
		}
		else if (key=="intdraw"){
			var checked=$(id+" .cond_integr")[0].checked;
			var arg= (checked ? "1" : "0");
			optionstring +="&"+key+"="+arg;
		}
		else if (key=="xmeandraw"){
			var checked=$(id+" .cond_xmean")[0].checked;
			var arg= (checked ? "1" : "0");
			optionstring +="&"+key+"="+arg;
		}
		else if (key=="xrmsdraw"){
			var checked=$(id+" .cond_xrms")[0].checked;
			var arg= (checked ? "1" : "0");
			optionstring +="&"+key+"="+arg;
		}
		else if (key=="ymeandraw"){
			var checked=$(id+" .cond_ymean")[0].checked;
			var arg= (checked ? "1" : "0");
			optionstring +="&"+key+"="+arg;
		}
		else if (key=="yrmsdraw"){
			var checked=$(id+" .cond_yrms")[0].checked;
			var arg= (checked ? "1" : "0");
			optionstring +="&"+key+"="+arg;
		}
		else if (key=="xmaxdraw"){
			var checked=$(id+" .cond_maxx")[0].checked;
			var arg= (checked ? "1" : "0");
			optionstring +="&"+key+"="+arg;
		}
		
		else if (key=="ymaxdraw"){
			var checked=$(id+" .cond_maxy")[0].checked;
			var arg= (checked ? "1" : "0");
			optionstring +="&"+key+"="+arg;
		}
		else if (key=="cmaxdraw"){
			var checked=$(id+" .cond_max")[0].checked;
			var arg= (checked ? "1" : "0");
			optionstring +="&"+key+"="+arg;
		}
		else{
			console.log("Warning: EvaluateChanges found unknown key:%s", key);
				}
		
		
	
	}// for index
	console.log("Resulting option string:%s", optionstring);
	return optionstring;
}


   
   GO4.ConditionEditor.prototype.CheckResize = function() {
      var id = "#"+this.divid;
      var width = $(id).width(); 
      var height = $(id).height();
      
      $(id).children().eq(0).width(width - 5).height(height - 5);
   }
   
  

   
   GO4.ConditionEditor.prototype.refreshEditor = function() {
      var editor=this;
	  var id = "#"+this.divid;
      var cond = this.cond;

      $(id+" .cond_name").text(cond.fName);
      $(id+" .cond_type").text(cond._typename);
      
            
      if(cond.fbEnabled) {
    	  $(id+" .cond_execmode").val(0);
      	}
      else{
    	  if(cond.fbResult)
    		  $(id+" .cond_execmode").val(1);
    	  else
    		  $(id+" .cond_execmode").val(2);
      }
      $(id+" .cond_execmode").selectmenu("refresh");
      $(id+" .cond_execmode").selectmenu("option", "width", "100%"); // workaround for selecmenu refresh problem (increases width each time!)
      if(cond.fbTrue) 
    	  $(id+" .cond_invertmode").val(0);
      else
    	  $(id+" .cond_invertmode").val(1);
      
      $(id+" .cond_invertmode").selectmenu("refresh");
      $(id+" .cond_invertmode").selectmenu("option", "width", "100%"); // workaround for selecmenu refresh problem (increases width each time!)
      
      $(id+" .cond_xmin").val(cond.fLow1).change(function(){ editor.MarkChanged("limits")});
      $(id+" .cond_xmax").val(cond.fUp1).change(function(){ editor.MarkChanged("limits")});
      if (cond.fiDim==2) {
         $(id+" .cond_ymin").val(cond.fLow2).change(function(){editor.MarkChanged("limits")});
         $(id+" .cond_ymax").val(cond.fUp2).change(function(){ editor.MarkChanged("limits")});
      } else {
         $(id+" .cond_ymin").prop('disabled', true);
         $(id+" .cond_ymax").prop('disabled', true);
      }
      
      $(id+" .cond_counts").text(cond.fiCounts);
      $(id+" .cond_true").text(cond.fiTrueCounts);
      $(id+" .cond_percent").text((cond.fiCounts > 0 ? 100. * cond.fiTrueCounts / cond.fiCounts : 0.).toFixed(2) + "%");
      
      $(id+" .cond_visible")
         .prop('checked', cond.fbVisible)
         .click(function() { cond.fbVisible = this.checked; editor.MarkChanged("visible")});
      $(id+" .cond_limits")
         .prop('checked', cond.fbLimitsDraw)
         .click(function() { cond.fbLimitsDraw = this.checked; editor.MarkChanged("limitsdraw")});
         
      $(id+" .cond_label")
         .prop('checked', cond.fbLabelDraw)
         .click(function() { cond.fbLabelDraw = this.checked; editor.MarkChanged("labeldraw")});
   
  
      $(id+" .cond_integr")
      .prop('checked', cond.fbIntDraw)
      .click(function() { cond.fbIntDraw = this.checked; editor.MarkChanged("intdraw")});
      
      $(id+" .cond_maxx")
      .prop('checked', cond.fbXMaxDraw)
      .click(function() { cond.fbXMaxDraw = this.checked; editor.MarkChanged("xmaxdraw")});
      
      $(id+" .cond_max")
      .prop('checked', cond.fbCMaxDraw)
      .click(function() { cond.fbCMaxDraw = this.checked; editor.MarkChanged("cmaxdraw")});
      
      $(id+" .cond_maxy")
      .prop('checked', cond.fbYMaxDraw)
      .click(function() { cond.fbYMaxDraw = this.checked; editor.MarkChanged("ymaxdraw")});
      
      $(id+" .cond_xmean")
      .prop('checked', cond.fbXMeanDraw)
      .click(function() { cond.fbXMeanDraw = this.checked; editor.MarkChanged("xmeandraw")});
      
      $(id+" .cond_xrms")
      .prop('checked', cond.fbXRMSDraw)
      .click(function() { cond.fbXRMSDraw = this.checked; editor.MarkChanged("xrmsdraw")});
      
      $(id+" .cond_ymean")
      .prop('checked', cond.fbYMeanDraw)
      .click(function() { cond.fbYMeanDraw = this.checked; editor.MarkChanged("ymeandraw")});
      
      $(id+" .cond_yrms")
      .prop('checked', cond.fbYRMSDraw)
      .click(function() { cond.fbYRMSDraw = this.checked; editor.MarkChanged("yrmsdraw")});
      
      
      editor.ClearChanges();  
      
      
      $("#QFWModeCombo").selectmenu("option", "width", $('#QFW-table').width());
      
      
  }
  //--------- end refreshEditor
  
  
  ////////////////////////////////////////////////////////// 
   GO4.ConditionEditor.prototype.fillEditor = function() {
      var id = "#"+this.divid;
      var editor=this;
      console.log("GO4.ConditionEditor.prototype.fillEditor " + this.cond.fName);
      // $(id).css("display","table");
      
      this.CheckResize();
      
      $(id+" .cond_tabs").tabs();
      

       $(id + " .cond_execmode").selectmenu({
			change : function(event, ui) {
				editor.MarkChanged("resultmode");
			}
		});
		$(id + " .cond_invertmode").selectmenu({
			change : function(event, ui) {
				editor.MarkChanged("invertmode");
			}
		});      
      var editor = this;
      
      var dabc = DABC.hpainter;
      
      $(id+" button:first")
         .text("")
         .append('<img src="/go4sys/icons/right.png"  height="16" width="16"/>')
         .button()
         .click(function() {
            console.log("update item = " + editor.GetItemName()); 
            if (DABC.hpainter) DABC.hpainter.display(editor.GetItemName()); 
            else  console.log("dabc object not found!"); 
            	
          })
         .next()
         .text("")
         .append('<img src="/go4sys/icons/left.png"  height="16" width="16"/>')
         .button()
         .click(function() { 
//        	 var conny=editor.GetItemName().split('/').pop();
//        	 var options="name="+conny;
        	 var options=""; // do not need to use name here
        	 options=editor.EvaluateChanges(options); // complete option string from all changed elements
        	 console.log("set - condition "+ editor.GetItemName()+ ", options="+options); 
        	 editor.DabcCommand("UpdateFromUrl",options,function(
     				result) {
        		 		console.log(result ? "set condition done. "
    					: "set condition FAILED.");
        		 		if(result) editor.ClearChanges();     			
     		});
         
         })
         .next()
         .text("")
         .append('<img src="/go4sys/icons/info1.png"  height="16" width="16"/>')
         .button()
         //.click(function() { console.log("warn - do nothing"); })
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
      
      //$(document).tooltip();
      //$(id).tooltip(); // NOTE: jquery ui tooltips will change title information, currently conflict with dabc/jsroot!
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
      
      this.cond= JSROOT.clone(obj); // does this also work with polygon condition?
      
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
      
      if ((this.cond['fFillStyle']==1001) && (this.cond['fFillColor']==19)) { 
         this.cond['fFillStyle'] = 3006;
         this.cond['fFillColor'] = 2;
      }
      
      var fill = this.createAttFill(this.cond);
      var line = JSROOT.Painter.createAttLine(this.cond);
      
      this.draw_g.attr("class","cond_container");
      
      var ndim = this.cond.fiDim;
      
      this.draw_g.append("svg:rect")
             .attr("x", x(this.cond.fLow1))
             .attr("y", (ndim==1) ? 0 : y(this.cond.fUp2))
             .attr("width", x(this.cond.fUp1) - x(this.cond.fLow1))
             .attr("height", (ndim==1) ? h : y(this.cond.fLow2) - y(this.cond.fUp2))
             .call(line.func)
             .call(fill.func);
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
      // $('#'+divid).append("Here will be condition " + cond._typename);
      
      cond['Test'] = function(x,y) {
         if (!this.fbEnabled) return this.fbResult;
         
         if (this.fxCut)
            return this.fxCut.IsInside(x,y) ? this.fbTrue : this.fbFalse; 
         
         if ((x < this.fLow1) || (x > this.fUp1)) return this.fbFalse;
         
         if (this.fiDim==2)
            if ((y < this.fLow2) || (y > this.fUp2)) return this.fbFalse;
         
         return this.fbTrue;
      }
      
      if (option=='same') {
         var condpainter = new GO4.ConditionPainter(cond, false);
         condpainter.SetDivId(divid);
         condpainter.drawCondition();
         condpainter.drawLabel();
         return condpainter;
      }
      
      if ((cond.fxHistoName=="") || (option=='editor')) {
         $('#'+divid).append("<br/>Histogram name not specified");
         var painter = new GO4.ConditionEditor(cond);
         painter.drawEditor(divid);
         return painter;
      }
      
      // $('#'+divid).append("<br/>Histogram name is " + cond.fxHistoName);
      
      var dabc = DABC.hpainter;
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

         histofullpath = "../../Histograms/" + cond.fxHistoName;
         
         dabc.Find(histofullpath, true)['_kind'] = "ROOT.TH1I"; 
         
         console.log("Try histogram" + histofullpath);
      } 

      $('#'+divid).append("<br/>Drawing histogram " + histofullpath);
      
      $('#'+divid).empty();
      
      var condpainter = new GO4.ConditionPainter(cond, false);
      
      dabc.display(histofullpath, "divid:" + divid, function(res) {
         if (res==null) return console.log("fail to get histogram " + histofullpath);
         condpainter.SetDivId(divid);
         console.log("Draw condition at " + divid);
         condpainter.drawCondition();
         condpainter.drawLabel();
      });

      return condpainter;
   }
   
   JSROOT.addDrawFunc("TGo4WinCond", GO4.drawGo4Cond);
   JSROOT.addDrawFunc("TGo4PolyCond", GO4.drawGo4Cond);
   

   // ===========================================================================================
   
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
   
   GO4.ParameterEditor.prototype.fillComments = function() {
      
      if (this.GetItemName()==null) return;
      
      var editor = this;
      if (editor.xreq!=null) return; // avoid double requests
      
      editor.xreq = JSROOT.NewHttpRequest(this.GetItemName()+"/h.json?more", 'object', function(res) {
         editor.xreq = null;

         if (res==null) return;
         var id = "#"+editor.divid;

         $(id + " .par_values tbody").find("tr").each( function(i,tr) {
            var name = $(tr).find("td:first").text();
            var title = null;
            for (var i in res._childs) {
               var n = res._childs[i]._name;
               if ((name==n) || (name.indexOf(n)==0)) {
                  title = res._childs[i]._title;
                  break;
               }
            }
            if (title!=null)
               $(tr).find("td:last").text(title).css('white-space', 'nowrap');
            
         });
      });
      editor.xreq.send(null);
   }
   
   GO4.ParameterEditor.prototype.fillEditor = function() {
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
               $(id + " .par_values tbody").append("<tr><td>" + key.toString() + "[" + i + "]</td><td><input type='text' value='" + value[i] + "'/></td><td></td></tr>");
            }
         } else {
            $(id + " .par_values tbody").append('<tr><td>' + key.toString() + "</td><td><input type='text' value='" + value + "'/></td><td></td></tr>");
         }
      }
      console.log("fillEditor finished");

   }
   
   GO4.ParameterEditor.prototype.drawEditor = function(divid) {
      var pthis = this;
       
      $("#"+divid).empty();
      $("#"+divid).load("/go4sys/html/pareditor.htm", "", 
            function() { pthis.SetDivId(divid); pthis.fillEditor(); pthis.fillComments(); });
   }
   
   GO4.ParameterEditor.prototype.SetItemName = function(name) {
      JSROOT.TBasePainter.prototype.SetItemName.call(this, name);
      this.fillComments();
   }

   GO4.drawParameter = function(divid, par, option) {
      var painter = new GO4.ParameterEditor(par);
      painter.drawEditor(divid);
      return painter;
   }
   
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
         
         xreq = JSROOT.NewHttpRequest(itemname+"/get.json", 'object', function(res) {
            xreq = null;
            if (res==null) return;
            
            for (var i in res._childs) {
               var value = res._childs[i].value;
               var name = res._childs[i]._name;
               if (name == 'State') 
                  $('#'+divid + " .event_rate").css('background-color',  value=='Stopped' ? 'red' : 'lightgreen');
               if (name == 'EventRate') $('#'+divid + " .event_rate").text(value);
               if (name == 'AverRate') $('#'+divid + " .aver_rate").text(value); 
               if (name == 'RunTime') $('#'+divid + " .run_time").text(value); 
               if (name == 'RunTime') $('#'+divid + " .total_events").text(value);
            }
            
         });
         
         xreq.send(null);
      }
       
      setInterval(UpdateStatus, 2000);
   }

})();
