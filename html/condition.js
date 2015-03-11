(function(){

   if (typeof JSROOT != "object") {
      var e1 = new Error("condition.js requires JSROOT to be already loaded");
      e1.source = "condition.js";
      throw e1;
   }

   if (typeof GO4 != "object") {
	      var e1 = new Error("condition.js requires GO4 to be already loaded");
	      e1.source = "condition.js";
	      throw e1;
	   }
   
   // =========================================================================================
   
   GO4.ConditionEditor = function(cond) {
      JSROOT.TBasePainter.call(this, cond);
      this.cond = cond;
      this.changes = ["dummy", "init"];
      this.ClearChanges();
   }
   
   GO4.ConditionEditor.prototype = Object.create(JSROOT.TBasePainter.prototype);

   
   GO4.ConditionEditor.prototype.isPolyCond = function() {
	      return ((this.cond._typename == "TGo4PolyCond") || (this.cond._typename == "TGo4ShapedCond")); 
	   }
   
   GO4.ConditionEditor.prototype.isEllipseCond = function() {
	      return (this.cond._typename == "TGo4ShapedCond"); 
	   }
   
   
   GO4.ConditionEditor.prototype.DabcCommand = function(cmd, option, callback) {
		var xmlHttp = new XMLHttpRequest();
		var pre="";
		if (this.GetItemName()!="") { // note: check against !=null does not work here!
			  pre = this.GetItemName() + "/"; // suppress / if item name is empty
				//console.log("Found non null itemname= -"+this.GetItemName()+"-");
		}
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
		  
		  $(id+" .buttonChangeLabel").show();// show warning sign 
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
	$(id+" .buttonChangeLabel").hide(); // hide warning sign 
		  
	  }
	  
// scan changed value list and return optionstring to be send to server
GO4.ConditionEditor.prototype.EvaluateChanges = function(optionstring) {
	var id = "#"+this.divid;
	var index;
	var len=this.changes.length;
	for	(index = 0; index < len ; index++) {
	    //var cursor=changes.pop();
		var key=this.changes[index];
		//console.log("Evaluate change key:%s", key);	
		
		// here mapping of key to editor field:
		if(key=="limits")
			{
			 var xmin=$(id+" .cond_xmin")[0].value;
        	 var xmax=$(id+" .cond_xmax")[0].value;
        	 optionstring +="&xmin="+xmin+"&xmax="+xmax;
        	 this.cond.fLow1 = xmin;
        	 this.cond.fUp1 = xmax;
        	  if (this.cond.fiDim==2) {
        		  var ymin=$(id+" .cond_ymin")[0].value;
        		  var ymax=$(id+" .cond_ymax")[0].value; 
        		  this.cond.fLow2 = xmin;
             	  this.cond.fUp2 = xmax;
        		  optionstring +="&ymin="+ymin+"&ymax="+ymax;
        	  }
			}
		else if(key=="polygon")
		{  
		 var npoints=$(id+" .cut_points")[0].value;		 
		 	optionstring +="&npolygon="+npoints;
		 	// TODO: set display of polygon points
		 	for(i=0; i<npoints; ++i)
		 		{
		 			var x=$(id + " .cut_values input").eq(2*i)[0].value;
		 			var y=$(id + " .cut_values input").eq(2*i+1)[0].value;
		 			
		 			
		 			optionstring +="&x"+i+"="+x+"&y"+i+"="+y;
		 		}
		}
		else if (key=="ellinpts"){
			var val=$(id+" .cond_ellipse_points")[0].value;
			optionstring +="&"+key+"="+val;
		}
		else if (key=="ellicx"){
			var val=$(id+" .cond_ellipse_cx")[0].value;
			optionstring +="&"+key+"="+val;
		}
		else if (key=="ellicy"){
			var val=$(id+" .cond_ellipse_cy")[0].value;
			optionstring +="&"+key+"="+val;
		}
		else if (key=="ellia1"){
			var val=$(id+" .cond_ellipse_a1")[0].value;
			optionstring +="&"+key+"="+val;
		}
		else if (key=="ellia2"){
			var val=$(id+" .cond_ellipse_a2")[0].value;
			optionstring +="&"+key+"="+val;
		}
		else if (key=="ellishape"){
			var arg=$(id+" .cond_ellipse_iscircle")[0].value;
			optionstring +="&"+key+"="+arg;			
		}
		else if (key=="ellith"){
			var val=$(id+" .cond_ellipse_theta")[0].value;
			optionstring +="&"+key+"="+val;
		}
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
			this.cond.fbLabelDraw=arg;
			optionstring +="&"+key+"="+arg;
		}
		else if (key=="limitsdraw"){
			var checked=$(id+" .cond_limits")[0].checked;
			var arg= (checked ? "1" : "0");
			this.cond.fbLimitsDraw=arg;
			optionstring +="&"+key+"="+arg;
		}
		else if (key=="intdraw"){
			var checked=$(id+" .cond_integr")[0].checked;
			var arg= (checked ? "1" : "0");
			this.cond.fbIntDraw=arg;
			optionstring +="&"+key+"="+arg;
		}
		else if (key=="xmeandraw"){
			var checked=$(id+" .cond_xmean")[0].checked;
			var arg= (checked ? "1" : "0");
			this.cond.fbXMeanDraw=arg;
			optionstring +="&"+key+"="+arg;
		}
		else if (key=="xrmsdraw"){
			var checked=$(id+" .cond_xrms")[0].checked;
			var arg= (checked ? "1" : "0");
			this.cond.fbXRMSDraw=arg;
			optionstring +="&"+key+"="+arg;
		}
		else if (key=="ymeandraw"){
			var checked=$(id+" .cond_ymean")[0].checked;
			var arg= (checked ? "1" : "0");
			this.cond.fbYMeanDraw=arg;
			optionstring +="&"+key+"="+arg;
		}
		else if (key=="yrmsdraw"){
			var checked=$(id+" .cond_yrms")[0].checked;
			var arg= (checked ? "1" : "0");
			this.cond.fbYRMSDraw=arg;
			optionstring +="&"+key+"="+arg;
		}
		else if (key=="xmaxdraw"){
			var checked=$(id+" .cond_maxx")[0].checked;
			var arg= (checked ? "1" : "0");
			this.cond.fbXMaxDraw=arg;
			optionstring +="&"+key+"="+arg;
		}
		
		else if (key=="ymaxdraw"){
			var checked=$(id+" .cond_maxy")[0].checked;
			var arg= (checked ? "1" : "0");
			this.cond.fbYMaxDraw=arg;
			optionstring +="&"+key+"="+arg;
		}
		else if (key=="cmaxdraw"){
			var checked=$(id+" .cond_max")[0].checked;
			var arg= (checked ? "1" : "0");
			this.cond.fbCMaxDraw=arg;
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
//      var id = "#"+this.divid;
//      var width = $(id).width(); 
//      var height = $(id).height();
//      
//      $(id).children().eq(0).width(width - 25).height(height - 25);
   }
   
  
   GO4.ConditionEditor.prototype.ChangePolygonDimension = function() {
	   // this only changes display of condition, not condition itself!
	   // note that condition is still changed in analysis only by EvaluateChanges
	   // local condition copy is unchanged until we can display it somewhere.
	   
	   if(!this.isPolyCond()) return;
	   var id = "#"+this.divid;
	   var oldpoints=this.cond.fxCut.fNpoints;
	   var npoints=$(id+" .cut_points")[0].value;
	   console.log("ChangePolygonDimension with numpoints="+npoints+", oldpoints="+oldpoints);
	   //if(npoints==oldpoints) return; // no dimension change, do nothing - disabled, error if we again go back to original condition dimension
	   if (this.cond.fxCut != null) {
		   
		   $(id + " .cut_values tbody").html(""); // clear old contents
			   if (npoints > oldpoints) {
				// insert last but one point into table:
				// first points are unchanged:
				for (i = 0; i < oldpoints - 1; i++) {
					var x = this.cond.fxCut.fX[i];
					var y = this.cond.fxCut.fY[i];
					$(id + " .cut_values tbody")
							.append(
									"<tr><td><input type='text' value='"
											+ x
											+ "'/></td>  <td> <input type='text' value='"
											+ y + "'/>  </td></tr>");
					console.log("i:" + i + ", X=" + x + " Y=" + y);
				}
				// inserted points will reproduce values of last but one point:
				var insx = this.cond.fxCut.fX[oldpoints - 2];
				var insy = this.cond.fxCut.fY[oldpoints - 2];
				for (i = oldpoints - 1; i < npoints - 1; i++) {
					$(id + " .cut_values tbody")
							.append(
									"<tr><td><input type='text' value='"
											+ insx
											+ "'/></td>  <td> <input type='text' value='"
											+ insy + "'/>  </td></tr>");
					console.log("i:" + i + ", X=" + insx + " Y=" + insy);
				}
				// final point is kept as last point of old polygon, should
				// match first point for closed tcutg:
				var lastx = this.cond.fxCut.fX[oldpoints - 1];
				var lasty = this.cond.fxCut.fY[oldpoints - 1];
				$(id + " .cut_values tbody").append(
						"<tr><td><input type='text' value='" + lastx
								+ "'/></td>  <td> <input type='text' value='"
								+ lasty + "'/>  </td></tr>");
				console.log("i:" + npoints - 1 + ", X=" + lastx + " Y=" + lasty);

			}
		   else
			   {
			   		// remove last but one point from table:
			   for (i = 0; i < npoints - 1; i++) {
					var x = this.cond.fxCut.fX[i];
					var y = this.cond.fxCut.fY[i];
					$(id + " .cut_values tbody")
							.append(
									"<tr><td><input type='text' value='"
											+ x
											+ "'/></td>  <td> <input type='text' value='"
											+ y + "'/>  </td></tr>");
					console.log("i:" + i + ", X=" + x + " Y=" + y);
				}
			   // final point is kept as last point of old polygon, should
			   // match first point for closed tcutg:
			   var lastx = this.cond.fxCut.fX[oldpoints - 1];
			   var lasty = this.cond.fxCut.fY[oldpoints - 1];
			   $(id + " .cut_values tbody").append(
						"<tr><td><input type='text' value='" + lastx
								+ "'/></td>  <td> <input type='text' value='"
								+ lasty + "'/>  </td></tr>");
				console.log("i:" + npoints - 1 + ", X=" + lastx + " Y=" + lasty);
			   
			   }		   
	   }
	   
	   
   
	   this.MarkChanged("polygon");
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
      
      if(this.isPolyCond()) {
    	  	$(id+" .cond_tabs").tabs( "disable", 0 ); // enable/disable by tab index
    		$(id+" .cond_tabs").tabs( "enable", 1 ); // enable/disable by tab index
    		if (this.cond.fxCut != null) {
    			var numpoints=this.cond.fxCut.fNpoints;
    			console.log("refreshEditor: npoints="+numpoints);
    			 $(id+" .cut_points").val(numpoints); //.change(function(){ editor.MarkChanged("polygon")});
    			 $(id + " .cut_values tbody").html("");
    			 
    			 
    			 for(i = 0; i < numpoints; i++) {
    				   var x=	this.cond.fxCut.fX[i];
    				   var y=	this.cond.fxCut.fY[i];
    	               $(id + " .cut_values tbody").append("<tr><td><input type='text' value='" + x + "'/></td>  <td> <input type='text' value='" + y + "'/>  </td></tr>");
    	               console.log("i:"+i+", X="+x+" Y="+y);
    			 }
    			 $(id + " .cut_values tbody").change(function(){ editor.MarkChanged("polygon")});
    			 
    		}
    		if(this.isEllipseCond()) { 
    			$(id+" .cond_tabs").tabs( "enable", 2 ); // enable/disable by tab index
    			var numpoints=this.cond.fiResolution;
    			 $(id+" .cond_ellipse_points").val(numpoints); 
    			 $(id+" .cond_ellipse_cx").val(cond.fdCenterX).change(function(){ editor.MarkChanged("ellicx")});
    			 $(id+" .cond_ellipse_cy").val(cond.fdCenterY).change(function(){ editor.MarkChanged("ellicy")});
    			 $(id+" .cond_ellipse_a1").val(cond.fdRadius1).change(function(){ editor.MarkChanged("ellia1")});
    			 $(id+" .cond_ellipse_a2").val(cond.fdRadius2).prop('disabled', cond.fbIsCircle).change(function(){ editor.MarkChanged("ellia2")});
    			 $(id+" .cond_ellipse_theta").val(cond.fdTheta).prop('disabled', cond.fbIsCircle).change(function(){
    				 editor.MarkChanged("ellith");
    				 $(id+" .cond_ellipse_theta_slider").slider( "option", "value", $(this)[0].value % 360);
    				 console.log("ellipse theta value="+$(this)[0].value);
    			 });
    			 
 			 
    			 var options = $(id+" .cond_ellipse_iscircle")[0].options;
 				for ( var i = 0; i < options.length; i++){
 					options[i].selected = (options[i].value == cond.fiShapeType);
 					}
 				$(id+" .cond_ellipse_iscircle").selectmenu('refresh', true).selectmenu("option", "width", "80%");;
 					
    			 
    			
    			
    			 $(id + " .cond_ellipse_theta_slider")
    			 	.slider({
						min : 0,
						max : 360,
						step : 1,
						value: cond.fdTheta,
						disabled: cond.fbIsCircle,
						change : function(event, ui) {
							editor.MarkChanged("ellith");
							$(id + " .cond_ellipse_theta").val(ui.value);
							console.log("slider changed to" + ui.value);
						},
						stop : function(event, ui) {
							editor.MarkChanged("ellith");
							console.log("sliderstopped.");
						}
					})
					
					;
    			 
    		}
          
      }
      else
    	  {
    	  	console.log("refreshEditor: - NO POLYGON CUT");
    	  	$(id+" .cond_tabs").tabs( "enable", 0 ); 
    	  	$(id+" .cond_tabs").tabs( "disable", 1 ); // enable/disable by tab index
    	  	$(id+" .cond_tabs").tabs( "disable", 2 ); // enable/disable by tab index
    	  }
      
      
      
      
      
      $(id+" .cond_counts").text(cond.fiCounts);
      $(id+" .cond_true").text(cond.fiTrueCounts);
      $(id+" .cond_percent").text((cond.fiCounts > 0 ? 100. * cond.fiTrueCounts / cond.fiCounts : 0.).toFixed(2) + "%");
      
      
      // todo: get keywords from condition class definition
      // problem: static variables are not streamed by default
      
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
      
     
      
      
      
      
      
  }
  //--------- end refreshEditor
  
  
  ////////////////////////////////////////////////////////// 
   GO4.ConditionEditor.prototype.fillEditor = function() {
      var id = "#"+this.divid;
      var editor = this;
      var cond = this.cond;
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
      
		if(this.isEllipseCond()) { 
		 $(id+" .cond_ellipse_iscircle").selectmenu({
				change : function(event, ui) {
					cond.fiShapeType = ui.item.value;
					if(cond.fiShapeType == 2)
						{
						// circle
							$(id+" .cond_ellipse_a2").prop('disabled', true);
							$(id+" .cond_ellipse_theta").prop('disabled', true);
							$(id + " .cond_ellipse_theta_slider").slider("disable");
							$(id + "cond_ellipse_points").prop('disabled', false);
						}
					else if(cond.fiShapeType == 3)
						{
							// ellipse
						$(id+" .cond_ellipse_a2").prop('disabled', false);
						$(id+" .cond_ellipse_theta").prop('disabled', false);
						$(id + " .cond_ellipse_theta_slider").slider("enable");
						$(id + "cond_ellipse_points").prop('disabled', false);
						
						
						}
					else if(cond.fiShapeType == 4)
					{
						// box
						$(id+" .cond_ellipse_a2").prop('disabled', false);
						$(id+" .cond_ellipse_theta").prop('disabled', false);
						$(id + " .cond_ellipse_theta_slider").slider("enable");
						$(id + "cond_ellipse_points").prop('disabled', true);
					}
					else
						{
							// free style
							$(id+" .cond_ellipse_a2").prop('disabled', true);
							$(id+" .cond_ellipse_theta").prop('disabled', true);
							$(id + " .cond_ellipse_theta_slider").slider("disable");
							$(id + "cond_ellipse_points").prop('disabled', false);
						}
					
					
					editor.MarkChanged("ellishape");
				}
			})
		 
		}
		
		
      var dabc = JSROOT.hpainter;
      
      
      $(id+" .buttonGetCondition")
      .button({text: false, icons: { primary: "ui-icon-arrowthick-1-e MyButtonStyle"}}).click(function() {
    	  console.log("update item = " + editor.GetItemName()); 
          if (JSROOT.hpainter) JSROOT.hpainter.display(editor.GetItemName()); 
          else  console.log("dabc object not found!"); 
          	
        }
      );

      
      $(id+" .buttonSetCondition")
      .button({text: false, icons: { primary: "ui-icon-arrowthick-1-w MyButtonStyle"}}).click(function() {
      	 var options=""; // do not need to use name here
     	 options=editor.EvaluateChanges(options); // complete option string from all changed elements
     	 console.log("set - condition "+ editor.GetItemName()+ ", options="+options); 
     	 editor.DabcCommand("UpdateFromUrl",options,function(
  				result) {
     		 		console.log(result ? "set condition done. "
 					: "set condition FAILED.");
     		 		if(result) editor.ClearChanges();     			
          	
        });
      });
      
      
    $(id+" .buttonChangeLabel")
         .button({text: false, icons: { primary: "ui-icon-alert MyButtonStyle"}}).click();
     
    $(id+" .buttonDrawCondition")
    .button({text: false, icons: { primary: "ui-icon-image MyButtonStyle"}}).click(function() {
    	// TODO: implement correctly after MDI is improved, need to find out active frame and location of bound histogram
       
       if (dabc) {
          editor.EvaluateChanges("");

          if (JSROOT.hpainter.updateOnOtherFrames(editor, editor.cond)) return;
          
          JSROOT.hpainter.drawOnSuitableHistogram(editor, editor.cond, editor.cond.fiDim==2);
          
          return;
       }
    	
    	//if (JSROOT.hpainter){ 
    		//var onlineprop = JSROOT.hpainter.GetOnlineProp(editor.GetItemName()); 
    		//var baseurl = onlineprop.server + onlineprop.itemname + "/"; 
    		var baseurl = editor.GetItemName() + "/"; 
    		var drawurl = baseurl + "draw.htm", editorurl = baseurl + "draw.htm?opt=editor";
    		console.log("draw condition to next window with url="+drawurl);
    		//window.open(drawurl);
    		window.open(drawurl,'_blank');
//    	}
//    	else
//    		{
//    		console.log("dabc object not found!"); 
//    		}
// problem: we do not have method to get currently selected pad...         
//    	var nextid="#"+(editor.divid + 1); // does not work, id is string and not number here
//    	console.log("draw condition to id="+nextid);
//    	GO4.drawGo4Cond(nextid, editor.cond, "");
//    	
    	
      }
    ); 
    
    $(id+" .buttonClearCondition")
    .button({text: false, icons: { primary: "ui-icon-trash MyButtonStyle"}}).click(function() {
    	console.log("clearing counters..."); 
    	var options="&resetcounters=1";
    	 editor.DabcCommand("UpdateFromUrl",options,function(
   				result) {
      		 		console.log(result ? "reset condition counters done. "
  					: "reset condition counters FAILED.");
      		 		if (result) { 
      		 			if(JSROOT.hpainter) JSROOT.hpainter.display(editor.GetItemName()); 
      		 			else  console.log("dabc object not found!"); 
      		 			} 
           	
         });
    	
    	
    	
      }
    );  
     
    
    
    $(id+" .cut_points").spinner({
        min: 0,
        max: 1000,
        step: 1,
        //spin: function( event, ui ) {console.log("cut spin has value:"+ui.value);},
        change: function( event, ui ) {editor.ChangePolygonDimension();//console.log("cut changed.");
        },
        stop: function( event, ui ) {editor.ChangePolygonDimension();//console.log("cut spin stopped.");
        }        
    });
      
    $(id+" .cond_ellipse_points").spinner({
        min: 0,
        max: 1000,
        step: 1,
        //spin: function( event, ui ) {console.log("cut spin has value:"+ui.value);},
        change: function( event, ui ) {editor.MarkChanged("ellinpts");console.log("ellipse points changed.");
        },
        stop: function( event, ui ) {editor.MarkChanged("ellinpts");console.log("ellipse points stopped.");
        }        
    });
      
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

   GO4.ConditionPainter.prototype.Test = function(x,y) {
	   // JAM: need to put this here, since condition object will lose internal definition after cloning it again!
	   var cond=this.cond;
	   if (!cond.fbEnabled) return cond.fbResult;
       
       if (cond.fxCut)
          return cond.fxCut.IsInside(x,y) ? cond.fbTrue : cond.fbFalse; 
       
       if ((x < cond.fLow1) || (x > cond.fUp1)) return cond.fbFalse;
       
       if (cond.fiDim==2)
          if ((y < cond.fLow2) || (y > cond.fUp2)) return cond.fbFalse;
       
       return cond.fbTrue;
    }
   
   
   GO4.ConditionPainter.prototype.GetObject = function() {
      return this.cond;
   }
   
   GO4.ConditionPainter.prototype.isPolyCond = function() {
      return ((this.cond._typename == "TGo4PolyCond") || (this.cond._typename == "TGo4ShapedCond")); 
   }
   
   GO4.ConditionPainter.prototype.isEllipseCond = function() {
      return (this.cond._typename == "TGo4ShapedCond"); 
   }
   
   
   GO4.ConditionPainter.prototype.drawCondition = function() {
	   
      if (this.isPolyCond()) {
         if (this.cond.fxCut != null) {
            // look here if cut is already drawn in divid:
            var cutfound=false;
            var cut=this.cond.fxCut;
            this.ForEachPainter(function(p) {
                if (p.obj_typename != "TCutG") return;
                console.log("Find TCutG painter");
                p.UpdateObject(cut);
                p.Redraw();
                cutfound=true;
             });
            if(cutfound) return;
            // only redraw if previous cut display was not there:
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
      
      var painter=this;
      var cond = this.cond;
      
      var stat = this.main_painter().CountStat(function(x,y) { return painter.Test(x,y); });
      
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
      
      this.cond = JSROOT.clone(obj);
      
      return true;
   }
   
   GO4.ConditionPainter.prototype.Redraw = function() {
      this.drawCondition();
      this.drawLabel();
   }
   
   GO4.drawGo4Cond = function(divid, cond, option) {
      
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
      
      var dabc = JSROOT.hpainter;
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
         
         dabc.Find({ name: histofullpath, force: true})['_kind'] = "ROOT.TH1I"; 
         
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
   
   JSROOT.addDrawFunc("TGo4WinCond", GO4.drawGo4Cond, ";editor");
   JSROOT.addDrawFunc("TGo4PolyCond", GO4.drawGo4Cond, ";editor");
   JSROOT.addDrawFunc("TGo4ShapedCond", GO4.drawGo4Cond, ";editor");


})(); // function
