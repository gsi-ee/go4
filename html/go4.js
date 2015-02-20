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
   

   GO4.AnalysisStatusEditor = function(stat) {
      JSROOT.TBasePainter.call(this, stat);
      this.stat = stat; 
      this.changes = ["dummy", "init"];
      this.ClearChanges();
   }
   
   GO4.AnalysisStatusEditor.prototype = Object.create(JSROOT.TBasePainter.prototype);

   
   GO4.AnalysisStatusEditor.prototype.DabcCommand = function(cmd, option, callback) {
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
				callback(true); // todo: evaluate return values of reply
			}
		}
		xmlHttp.send(null);
	};
 
  GO4.AnalysisStatusEditor.prototype.MarkChanged = function(key) {
		  // first avoid duplicate keys:
		  var index;
		  for	(index = 0; index < this.changes.length; index++) {
				if (this.changes[index]== key) return;
			} 
		  this.changes.push(key);
		  console.log("Mark changed :%s", key);	
		  var id = "#"+this.divid; 
		  
		  $(id+" .buttonAnaChangeLabel").show();// show warning sign 
	  }
	  
	  // clear changed elements' ist, make warning sign invisible  
GO4.AnalysisStatusEditor.prototype.ClearChanges = function() {
	var index;
	var len=this.changes.length;
	for	(index = 0; index < len ; index++) {
	    var removed=this.changes.pop();
	    console.log("Clear changes removed :%s", removed);	
	} 
	var id = "#"+this.divid; 
	$(id+" .buttonAnaChangeLabel").hide(); // hide warning sign 
		  
	  }
	  
//scan changed value list and return optionstring to be send to server
GO4.AnalysisStatusEditor.prototype.EvaluateChanges = function(optionstring) {
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
//			 var xmin=$(id+" .cond_xmin")[0].value;
//       	 var xmax=$(id+" .cond_xmax")[0].value;
//       	 optionstring +="&xmin="+xmin+"&xmax="+xmax;
//       	 this.cond.fLow1 = xmin;
//       	 this.cond.fUp1 = xmax;
//       	  if (this.cond.fiDim==2) {
//       		  var ymin=$(id+" .cond_ymin")[0].value;
//       		  var ymax=$(id+" .cond_ymax")[0].value; 
//       		  this.cond.fLow2 = xmin;
//            	  this.cond.fUp2 = xmax;
//       		  optionstring +="&ymin="+ymin+"&ymax="+ymax;
//       	  }
			}
		else if(key=="polygon")
		{  
			
//		 var npoints=$(id+" .cut_points")[0].value;		 
//		 	optionstring +="&npolygon="+npoints;
//		 	// TODO: set display of polygon points
//		 	for(i=0; i<npoints; ++i)
//		 		{
//		 			var x=$(id + " .cut_values input").eq(2*i)[0].value;
//		 			var y=$(id + " .cut_values input").eq(2*i+1)[0].value;
//		 			
//		 			
//		 			optionstring +="&x"+i+"="+x+"&y"+i+"="+y;
//		 		}
		
		}
		else{
			console.log("Warning: EvaluateChanges found unknown key:%s", key);
				}
		
		
	
	}// for index
	console.log("Resulting option string:%s", optionstring);
	return optionstring;
}


   
   
   

   
   
   GO4.AnalysisStatusEditor.prototype.refreshEditor = function()
   {
      var id = "#"+this.divid;
      var editor=this;
      var stat=this.stat;
      var names = "";
      
      ///////////// ANALYSIS STEPS:
      
      $(id+" .steptabs").tabs( "option", "disabled", [0, 1, 2, 3, 4, 5, 6, 7] ); 
      stat.fxStepArray.arr.forEach(function(element, index, array) {
    	  $(id+" .steptabs").tabs("enable",index);
    	  $(id +" .steptabs ul:first li:eq("+index+") a").text(element.fName);
    	  
    	  console.log("refreshEditor finds step name:"+ element.fName);
    	  names += " " + element.fName;
    	  var theIndex=index;
    		var theElement=element;
    	  $(id +" .steptabs div:eq("+index+")").load("/go4sys/html/stepeditor.htm", "", 
    	            function(responseTxt, statusTxt, xhr) { 
    		  		console.log("here we fill stepeditor for "  + element.fName + " to index="+index);
//    		  		console.log("statusTxt: "  + statusTxt);
//    		  		console.log("responseTxt: "  + responseTxt);
    		  		
//    		  		console.log("found checkbox: "  + $(id+" .steptabs div:eq("+index+")").find(" .step_box_step_enab").prop('name')
//    		  				+ "this is: "+$(this).find(" .step_box_step_enab").prop('name')+ " of index: "+index);
//    		  		//$(id+" .steptabs div:eq("+index+")").find(" .step_box_step_enab")
    		  		
    		  		var pthis=$(this);
    		  		console.log("process enabled="+theElement.fbProcessEnabled + "for theElement: "+theElement.fName);   	
    		  		console.log("source enabled="+theElement.fbSourceEnabled + "for theElement: "+theElement.fName);
    		  		console.log("store enabled="+theElement.fbStoreEnabled + "for theElement: "+theElement.fName);
    		  		
    		  		var storetable=pthis.find(" .step_store");
    		  		var sourcetable=pthis.find(" .step_source");
    		  		var enablebox=pthis.find(" .step_box_step_enab");
    		  		var sourcebox=pthis.find(" .step_box_source_enab");
    		  		var storebox=pthis.find(" .step_box_store_enab");
    		  		
    		  		
    		  		// here step control checkboxes and source/store visibility:
					if (theElement.fbProcessEnabled) {
							sourcebox.prop('disabled',false);
							storebox.prop('disabled',false);
							if (theElement.fbSourceEnabled) {
								sourcetable.show();
							} else {
								sourcetable.hide();
							}
							if (theElement.fbStoreEnabled) {
								storetable.show();
							} else {
								storetable.hide();
							}
						} else {
							sourcebox.prop('disabled',true);
							storebox.prop('disabled',true);
							sourcetable.hide();
							storetable.hide();
						}

					
					
    		  		
    		  		enablebox.prop('checked', theElement.fbProcessEnabled)
    	 	         .click(function() 
    	 	        		 { 
    	 	        	 		editor.MarkChanged("stepenabled_"+ theIndex);
    	 	        	 		if ($(this).prop('checked')) {
    								sourcetable.show();
    								storetable.show();
    							} else {
    								sourcetable.hide();
    								storetable.hide();
    							}
    	 	        	 		
    	 	        	 		
    	 	        	 		if ($(this).prop('checked')) {
    								sourcebox.prop('disabled',false);
    								storebox.prop('disabled',false);
    								if (sourcebox.prop('checked')) {
    									sourcetable.show();
    								} else {
    									sourcetable.hide();
    								}
    								if (storebox.prop('checked')){
    									storetable.show();
    								} else {
    									storetable.hide();
    								}
    							} else {
    								sourcebox.prop('disabled',true);
    								storebox.prop('disabled',true);
    								sourcetable.hide();
    								storetable.hide();
    							}
    	 	        	 		
    	 	        	 		
    	 	        	 
   	 	         }); // clickfunction
    		  		
    		  		//$(this).find(" .step_box_source_enab")
   	 	         sourcebox.prop('checked', theElement.fbSourceEnabled)
   	 	         .click(function() 
   	 	        		 { 
   	 	        	 		editor.MarkChanged("sourceenabled_"+ theIndex);
   	 	        	 		if($(this).prop('checked')) 
	 	        	 			sourcetable.show();
	 	        	 		else
	 	        	 			sourcetable.hide();
   	 	        	 		
  	 	         }); // clickfunction	
    		  		
    		  		
    		  		//$(this).find(" .step_box_store_enab")
      	 	         storebox.prop('checked', theElement.fbStoreEnabled)
      	 	         .click(function() 
      	 	        		 { 
      	 	        	 		editor.MarkChanged("storeenabled_"+ theIndex);
      	 	        	 		if($(this).prop('checked')) 
      	 	        	 			storetable.show();
      	 	        	 		else
      	 	        	 			storetable.hide();
      	 	        	 		
     	 	         }); // clickfunction	
    		  		
    		  		
      	 	      $(id+" .steptabs").tabs("refresh");
      	 	      console.log("refreshed tabs: " + $(id+" .steptabs").attr('title'));
    		  		
    	  });// load
    	  
    	  
    	  
      }); // for each
      
      //$(id+" .steptabs").tabs("refresh");
      
      

      
      
      
      
      
      /////////////////// AUTO SAVE FILE:      
      $(id + " .anaASF_name").val(stat.fxAutoFileName);
      $(id+" .anaASF_enabled")
    	         .prop('checked', stat.fbAutoSaveOn)
    	         .click(function() { editor.MarkChanged("asfenabled")});
      
      $(id +" .anaASF_time").val(stat.fiAutoSaveInterval);
      $(id +" .anaASF_compression").val(stat.fiAutoSaveCompression);
      
      $(id+" .anaASF_overwrite")
 	         .prop('checked', stat.fbAutoSaveOverwrite)
 	         .click(function() { editor.MarkChanged("asfoverwrite")});
      
      ////////////////// PREFS FILE:
      $(id + " .anaprefs_name").val(stat.fxConfigFileName);
      
      
      console.log("analysis editor: refreshEditor");
      editor.ClearChanges();  
      
   }
   
   GO4.AnalysisStatusEditor.prototype.fillEditor = function()
   {
	   var id = "#"+this.divid;
	   var editor=this;
	   
	   
	   $(id +" .steptabs").tabs({
	  		activate : function(event, ui) {
	  			//updateElementsSize();
	  			console.log("analysis editor: activated tab: "+ ui.newTab.text());
	  		}
	      
	  	});
	   
	   
	   
	   
	   
	   
	   
	   
	   $(id+" .buttonGetAnalysis")
	      .button({text: false, icons: { primary: "ui-icon-arrowthick-1-e MyButtonStyle"}}).click(function() {
	    	  console.log("update item = " + editor.GetItemName()); 
	    	  
	    	  
	          if (DABC.hpainter) DABC.hpainter.display(editor.GetItemName()); 
	          else  console.log("dabc object not found!"); 
	          	
	        }
	      );

	      
	      $(id+" .buttonSetAnalysis")
	      .button({text: true, icons: { primary: "ui-icon-arrowthick-1-w MyButtonStyle"}}).click(function() {
	      	 var options=""; // do not need to use name here
	     	 options=editor.EvaluateChanges(options); // complete option string from all changed elements
	     	 console.log("submit analysis "+ editor.GetItemName()+ ", options="+options); 
	     	 editor.DabcCommand("UpdateFromUrl",options,function(
	  				result) {
	     		 		console.log(result ? "setting analyis configuration done. "
	 					: "set analysis FAILED.");
	     		 		if(result) editor.ClearChanges();     			
	          	
	        });
	      });
	      
	      
	    $(id+" .buttonAnaChangeLabel")
	         .button({text: false, icons: { primary: "ui-icon-alert MyButtonStyle"}}); 
	   
	   
	    $(id+" .buttonSetStartAnalysis")
	      .button({text: true, icons: { primary: "ui-icon-play MyButtonStyle"}}).click(function() {
	      	 var options=""; // do not need to use name here
	     	 options=editor.EvaluateChanges(options); // complete option string from all changed elements
	     	 console.log("submit and start analysis "+ editor.GetItemName()+ ", options="+options); 
	     	 editor.DabcCommand("UpdateFromUrl",options,function(
	  				result) {
	     		 		console.log(result ? "submit and start analyis configuration done. "
	 					: "set analysis FAILED.");
	     		 		if(result) editor.ClearChanges(); 
	     		 		
	     		 		// todo: start analysis only after submission was successful?
	          	
	        });
	      });
	    
	    $(id+" .buttonCloseAnalysis")
	      .button({text: true, icons: { primary: "ui-icon-closethick MyButtonStyle"}}).click(function() {
	      	 var options="close"; 
	     //	 options=editor.EvaluateChanges(options); // complete option string from all changed elements
	     	 console.log("close analysis "+ editor.GetItemName()+ ", options="+options); 
	     	 editor.DabcCommand("UpdateFromUrl",options,function(
	  				result) {
	     		 		console.log(result ? "closing down analyis done. "
	 					: "set analysis FAILED.");
	     		 		//if(result) editor.ClearChanges();     			
	          	
	        });
	      });
	    
	    
	    
	    $(id+" .buttonSaveAnaASF")
        .button({text: false, icons: { primary: "ui-icon-disk MyButtonStyle"}}); 
	    
	    $(id + " .anaASF_form").submit(
				function(event) {
					var content= $(id + " .anaASF_name")[0].value;
					var requestmsg = "Really Write autosave file : "+ content;
					var response = confirm(requestmsg);
					if (!response){
						event.preventDefault();
						return;
						}

					// todo: put here command to write asf filepreferences
					//					
					console.log("Writing autosave file: "+content); 
					
					event.preventDefault(); // do not send automatic request to server!
					
				});
	    
	    
	    $(id +" .anaASF_time").spinner({
	        min: 0,
	        max: 100000,
	        step: 10,
	    	stop: function( event, ui ) {editor.MarkChanged("asftime");console.log("asftime stop.")}
	    });
	    
	    $(id +" .anaASF_compression").spinner({
	        min: 0,
	        max: 9,
	        step: 1,
	    	stop: function( event, ui ) {editor.MarkChanged("asfcomp");console.log("asfcomp stop.")}
	    });
	    
	    
	    $(id+" .buttonSaveAnaConf")
        .button({text: false, icons: { primary: "ui-icon-disk MyButtonStyle"}}); 
	    
	    $(id+" .buttonLoadAnaConf")
	      .button({text: false, icons: { primary: "ui-icon-folder-open MyButtonStyle"}}).click(function() {
	    	  
	    	  var content= $(id + " .anaprefs_name")[0].value;
	    	  var requestmsg = "Really save analysis preferences: "+ content;
				var response = confirm(requestmsg);
				if (!response){
					event.preventDefault();
					return;
					}
	    	  console.log("Loading analysis Prefs from "+content); 
	    	  
//	      	 var options=""; // do not need to use name here
//	     	 options=editor.EvaluateChanges(options); // complete option string from all changed elements
//	     	 console.log("set - condition "+ editor.GetItemName()+ ", options="+options); 
//	     	 editor.DabcCommand("UpdateFromUrl",options,function(
//	  				result) {
//	     		 		console.log(result ? "set condition done. "
//	 					: "set condition FAILED.");
//	     		 		if(result) editor.ClearChanges();     				          	
//	        });
	      });
	    
	    
	    
	    $(id + " .anaprefs_form").submit(
				function(event) {
					var content= $(id + " .anaprefs_name")[0].value;
					var requestmsg = "Really save analysis preferences: "+ content;
					var response = confirm(requestmsg);
					if (!response){
						event.preventDefault();
						return;
						}
					
					
					
					console.log("Saving analysis Prefs to "+content); 
					
					// todo: put here command to save preferences
					event.preventDefault(); // do not send automatic request to server!
				});
	   
	   
	   
      this.refreshEditor();
   }
   
   GO4.AnalysisStatusEditor.prototype.drawEditor = function(divid) {
      var pthis = this;
       
      $("#"+divid).empty();
      $("#"+divid).load("/go4sys/html/analysiseditor.htm", "", 
            function() { pthis.SetDivId(divid); pthis.fillEditor();  });
      //console.log("analysis editor: drawEditor");
   }
   
   GO4.AnalysisStatusEditor.prototype.RedrawPad = function(resize) {
	   //console.log("analysis editor: RedrawPad");
      this.refreshEditor();
   }

   GO4.AnalysisStatusEditor.prototype.UpdateObject = function(obj) {
      if (obj._typename != this.stat._typename) return false;
      this.stat = JSROOT.clone(obj); 
      return true;
   }
   
   
   GO4.drawGo4AnalysisStatus = function(divid, stat, option) {
      //console.log("Draw analysis status");
      
      var painter = new GO4.AnalysisStatusEditor(stat);
      painter.drawEditor(divid);
      return painter;
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
		console.log("Evaluate change key:%s", key);	
		
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
		
		
      var dabc = DABC.hpainter;
      
      
      $(id+" .buttonGetCondition")
      .button({text: false, icons: { primary: "ui-icon-arrowthick-1-e MyButtonStyle"}}).click(function() {
    	  console.log("update item = " + editor.GetItemName()); 
          if (DABC.hpainter) DABC.hpainter.display(editor.GetItemName()); 
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

          if (DABC.hpainter.updateOnOtherFrames(editor, editor.cond)) return;
          
          DABC.hpainter.drawOnSuitableHistogram(editor, editor.cond, editor.cond.fiDim==2);
          
          return;
       }
    	
    	//if (DABC.hpainter){ 
    		//var onlineprop = DABC.hpainter.GetOnlineProp(editor.GetItemName()); 
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
      		 			if(DABC.hpainter) DABC.hpainter.display(editor.GetItemName()); 
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
   JSROOT.addDrawFunc("TGo4AnalysisStatus", GO4.drawGo4AnalysisStatus, ";editor");
   

   // ===========================================================================================
   
   GO4.ParameterEditor = function(par) {
      JSROOT.TBasePainter.call(this);
      this.par = par;
      this.changes = ["dummy", "init"]; // TODO: put to common "base class" of condition and parameter editor 
   }

   GO4.ParameterEditor.prototype = Object.create(JSROOT.TBasePainter.prototype);

   GO4.ParameterEditor.prototype.CheckResize = function() {
//      var id = "#"+this.divid;
//      var width = $(id).width(); 
//      var height = $(id).height();
//      $(id).children().eq(0).width(width - 4).height(height - 4);
   }
   
   
   // TODO: put to common "base class" of condition and parameter editor 
   GO4.ParameterEditor.prototype.DabcCommand = function(cmd, option, callback) {
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
	
	
	 // TODO: put to common "base class" of condition and parameter editor 
	 // add identifier of changed element to list, make warning sign visible
	  GO4.ParameterEditor.prototype.MarkChanged = function(key) {
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
	// TODO: put to common "base class" of condition and parameter editor 
	GO4.ParameterEditor.prototype.ClearChanges = function() {
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
	GO4.ParameterEditor.prototype.EvaluateChanges = function(optionstring) {
		var id = "#"+this.divid;
		var index;
		var len=this.changes.length;
		for	(index = 0; index < len ; index++) {
		    //var cursor=changes.pop();
			var key=this.changes[index];
			console.log("Evaluate change key:%s", key);				
			// here mapping of key to editor field:
			// key will be name of variable which is class name of input field:
			var val=$(id+" ."+key.toString())[0].value;
			//var opt= key.replace(/_/g, "[").replace(/-/g, "]");	// old with other placeholders		
       	 	var arraysplit=key.split("_");
       	 	var opt="";
       	 	if(arraysplit.length>1)
       	 		{
       	 			// found array with index after separator, reformat it:
       	 		opt=arraysplit[0];
       	 		if(arraysplit.length>2)
       	 		{
       	 			if(arraysplit.length>3)
       	 			{
       	 				// 3dim array:
       	 				var ix=arraysplit[arraysplit.length -3]; //
	 					var iy=arraysplit[arraysplit.length -2]; //
	 					var iz=arraysplit[arraysplit.length -1]; //
	 					opt+="["+ix+"]["+iy+"]["+iz+"]";       	 				
       	 			}
       	 			else
       	 				{
       	 					// 2dim array:
       	 					var ix=arraysplit[arraysplit.length -2]; //
       	 					var iy=arraysplit[arraysplit.length -1]; //
       	 					opt+="["+ix+"]["+iy+"]";       	 					
       	 				}
       	 			var iy=arraysplit[arraysplit.length -2]; //       	 		
       	 		}
       	 		else
       	 			{
       	 			// 1dim array:
       	 				opt=arraysplit[0];
       	 				var ix=arraysplit[arraysplit.length -1]; //
       	 				opt+="["+ix+"]";
       	 			}
       	 		}
       	 	else
       	 		{
       	 			opt=key;
       	 		}
       	 	
       	 	optionstring +="&"+opt+"="+val;
		}// for index
		console.log("Resulting option string:%s", optionstring);
		return optionstring;
	}

   
   
   
   
   GO4.ParameterEditor.prototype.fillComments = function() {
      var editor = this;
      if (editor.xreq!=null) return; // avoid double requests
      var pre="";
      if (this.GetItemName()!="") { // note: check against !=null does not work here!
		  pre = this.GetItemName() + "/"; // suppress / if item name is empty
			//console.log("Found non null itemname= -"+this.GetItemName()+"-");
	}
      
      
      editor.xreq = JSROOT.NewHttpRequest(pre+"h.json?more", 'object', function(res) {
         editor.xreq = null;
         if (res==null) return;
         var id = "#"+editor.divid;
         $(id + " .par_values tbody").find("tr").each( function(i,tr) {
            var name = $(tr).find("td:first").text();
            var title = null;
            var arrayinfo=null;
            var typeinfo=null;
            for (var i in res._childs) {
               var n = res._childs[i]._name;
               var arsplit=name.split("["); // remove array information at the end, if any
               if (arsplit[0]==n) {
               //if ((name==n) || (name.indexOf(n)==0)) { 
                  title = res._childs[i]._title;
                  arrayinfo=res._childs[i]._arraydim;
                  typeinfo=res._childs[i]._typename;
                  //console.log("found title="+title+", arrayinfo="+arrayinfo);
                  break;
               }
               
               
               
            }
            if (title!=null)
               $(tr).find("td.par_comment").text(title).css('white-space', 'nowrap'); // comments from class member declaration
            if (typeinfo!=null)
            	{
            	 	$(tr).find("td.par_class").text(typeinfo).css('white-space', 'nowrap'); // member type
            	 	
            	    $(tr).parents('table.par_arraytable') .find('td.par_comment:first').text("Array").css('white-space', 'nowrap');
     		         // if we are inside array table, indicate that we are an array
            	    
            	 	if (arrayinfo!=null)
		                $(tr).parents('table.par_arraytable') .find('td.par_class:first').text(typeinfo+ " [" + arrayinfo+"]").css('white-space', 'nowrap');
		            else
		            	$(tr).parents('table.par_arraytable') .find('td.par_class:first').text(typeinfo).css('white-space', 'nowrap');
		          	
            	 	// put type information of array to subtable header
            		}
            
         });
         
         
         
         
      });
      editor.xreq.send(null);
   }
   GO4.ParameterEditor.prototype.fillMemberTable = function() {
	   var editor=this;
	   var id = "#"+this.divid;
	   var par = this.par; 
	   $(id + " .par_values tbody").html("");
	   var found_title = false;	      
	      for (var key in par) {
	         if (typeof par[key] == 'function') continue;
	         if (key == 'fTitle') { found_title = true; continue; } 
	         if (!found_title) continue;
	         var value = (par[key]!=null ? (par[key] instanceof Array ? par[key] : par[key].toString()): "null");
	         var classname="";
	         if (value instanceof Array) {
	        	// here put array table with clickable header:
 				// (thanks to Jonathan at http://mediaformations.com/accordion-tables-with-jquery/ for this idea!)
	        	 var arraytableclass=key.toString()+"_array";
	        	 var isTooBig=false;
	        	 $(id + " .par_values > tbody").append("<tr><td style='width:100%; padding:0px' colspan='4' > <table class='"+arraytableclass+" par_arraytable'><thead><tr><td class='par_key'> <bf>[+]</bf> "+ key.toString()+"</td><td class='par_class'></td><td class='par_value' >Click to expand</td><td class='par_comment'></td></tr></thead><tbody></tbody></table></td></tr>");
	            for(i = 0; i < value.length; i++) {
	            		if(value[i] instanceof Array)
	            			{
	            				subvalue=value[i];
	            				for (j = 0; j < subvalue.length; j++) {
	            					if (subvalue[j] instanceof Array) {
	            						subsubvalue = subvalue[j];
	            						// here supress display of 3d arrays if too
	            						// large:
	            						if (subsubvalue.length * subvalue.length * value.length > 1000) {
										isTooBig=true;
										break;
	            						}
	            						else {
	            							for (k = 0; k < subsubvalue.length; k++) {
										// decode 3dim array
										classname = key.toString() + "_" + i
												+ "_" + j + "_" + k;
										// $(id + " .par_values
										// tbody").append("<tr><td>" +
										// key.toString() + "[" + i +
										// "]["+j+"]["+k+"]</td><td><input
										// type='text' value='" + subsubvalue[k]
										// + "' class='"+ classname
										// +"'/></td><td></td></tr>");
										$(id + " ." + arraytableclass + " tbody").append(
														"<tr><td class='par_key'>"
																+ key
																		.toString()
																+ "["
																+ i
																+ "]["
																+ j
																+ "]["
																+ k
																+ "]</td><td class='par_class'></td><td class='par_value'><input type='text' size='10'  value='"
																+ subsubvalue[k]
																+ "' class='"
																+ classname
																+ "'/></td><td class='par_comment'></td></tr>");

	            								} // for k
	            							}
	     	            				}
	            					 else
	            						{
	            						 	// decode 2dim array
	            						   	classname=key.toString()+"_"+ i+"_"+j;
	            			            	//$(id + " .par_values tbody").append("<tr><td>" + key.toString() + "[" + i + "]["+j+"]</td><td><input type='text' value='" + subvalue[j] + "' class='"+ classname +"'/></td><td></td></tr>");
	            							$(id + " ."+arraytableclass+" tbody").append("<tr><td class='par_key'>" + key.toString() + "[" + i + "]["+j+"]</td><td class='par_class'></td><td class='par_value'><input type='text' size='10' value='" + subvalue[j] + "' class='"+ classname +"'/></td><td class='par_comment'></td></tr>");
		            						
	            						}	            					 
	            				 } // for j	            				
	            			}
	            		else
	            			{
	            			// decode 1dim array
	            			//classname=key.toString()+"_"+ i+"-"; // old with placeholders instead brackets
	            	
	            			classname=key.toString()+"_"+ i;
	            			//$(id + " .par_values tbody").append("<tr><td>" + key.toString() + "[" + i + "]</td><td><input type='text' value='" + value[i] + "' class='"+ classname +"'/></td><td></td></tr>");
	            			$(id + " ."+arraytableclass+" tbody").append("<tr><td class='par_key'>" + key.toString() + "[" + i + "]</td><td class='par_class'></td><td class='par_value'><input type='text' size='10' value='" + value[i] + "' class='"+ classname +"'/></td><td class='par_comment'></td></tr>");
	    	            	
	            			}
	            } // for i
//	            
	            if(isTooBig)
	            	{
						$(id + " ." + arraytableclass + " tbody")
								.append(
										"<tr><td class='par_key'>" + key.toString()+ "</td><td colspan='3'> Sorry, Array dimension ["
												+ value.length
												+ "]["
												+ subvalue.length
												+ "]["
												+ subsubvalue.length
												+ "] too big to display!</td></tr>");
	            	}
	            
	            
	            $(id + " table."+arraytableclass+" thead tr").click(
	            		function() {
	            			$(this) .parents('table.par_arraytable') .children('tbody') .toggle();
	            			$(this) .parents('table.par_arraytable') .find('td:first').text(
	            					function(i,origText){	            						
	            						var changed=origText;
	            						if(origText.indexOf("[+]")!= -1)
	            							changed=origText.replace("[+]","[-]");
	            						if(origText.indexOf("[-]")!= -1)
	            							changed=origText.replace("[-]","[+]");
	            						//console.log("original text= "+origText+", changed="+changed);
	            						return changed;
	            					  });
	            			$(this) .parents('table.par_arraytable') .find('td.par_value:first').text(
	            					function(i,origText){	            						
	            						var changed=origText;
	            						if(origText.indexOf("expand")!= -1)
	            							changed=origText.replace("expand","shrink");
	            						if(origText.indexOf("shrink")!= -1)
	            							changed=origText.replace("shrink","expand");
	            						//console.log("original text= "+origText+", changed="+changed);
	            						return changed;
	            					  });
	            			
	            			
	            			//console.log("Clicked on table header");
	            		}
	            	);
	            $(id + " table."+arraytableclass).children('tbody').hide();
	            
	            
	            
	         } else {
	        	 classname=key.toString();
	            $(id + " .par_values > tbody").append("<tr><td class='par_key'>" + key.toString() + "</td><td class='par_class'></td><td class='par_value'><input type='text' size='10' value='" + value + "' class='"+classname+"'/></td><td class='par_comment'></td></tr>");
	         }
	       
	        
	      }
	   // here set callbacks; referred classname must be evaluated dynamically in function!:
         $(id + " .par_values tbody input").change(function(){ editor.MarkChanged($(this).attr('class'))}); 
         $(id + " .par_values tbody td").addClass("par_membertable_style");
         $(id + " .par_values > thead th").addClass("par_memberheader_style");
         $(id + " .par_arraytable thead td").addClass("par_arrayheader_style");
         
	     this.ClearChanges(); 
	     
	     
	     
   }
   GO4.ParameterEditor.prototype.fillEditor = function() {
      var editor=this;
	  var par = this.par;
      var id = "#"+this.divid;
      var width = $(id).width(); 
      var height = $(id).height();
      
      $(id+" .par_name").text(par.fName);
      $(id+" .par_type").text(par._typename);
      
      $(id).children().eq(0).width(width - 4).height(height - 4);
      
      
      $(id+" .buttonGetParameter")
      .button({text: false, icons: { primary: "ui-icon-arrowthick-1-e MyButtonStyle"}}).click(function() {
    	  console.log("update item = " + editor.GetItemName()); 
          if (DABC.hpainter) DABC.hpainter.display(editor.GetItemName()); 
          else  console.log("dabc object not found!"); 
          	
        }
      );

      
      $(id+" .buttonSetParameter")
      .button({text: false, icons: { primary: "ui-icon-arrowthick-1-w MyButtonStyle"}}).click(function() {
      	 var options=""; // do not need to use name here
     	 options=editor.EvaluateChanges(options); // complete option string from all changed elements
     	 console.log("set - condition "+ editor.GetItemName()+ ", options="+options); 
     	 editor.DabcCommand("UpdateFromUrl",options,function(
  				result) {
     		 		console.log(result ? "set parameter done. "
 					: "set parameter FAILED.");
     		 		if(result) editor.ClearChanges();     			
          	
        });
      });
      
      
    $(id+" .buttonChangeLabel")
         .button({text: false, icons: { primary: "ui-icon-alert MyButtonStyle"}}).click();
      
          
      this.fillMemberTable();
      console.log("fillEditor finished");

   }
   
   
   GO4.ParameterEditor.prototype.RedrawObject = function(obj) {
	   console.log("ParemeterEditor RedrawObject...");
	      if (this.UpdateObject(obj))
	         this.Redraw(); // no need to redraw complete pad
	   }

	   GO4.ParameterEditor.prototype.UpdateObject = function(obj) {
	      if (obj._typename != this.par._typename) return false;
	      console.log("ParemeterEditor UpdateObject...");	      
	      this.par= JSROOT.clone(obj); 
	      return true;
	   }
	   
	   GO4.ParameterEditor.prototype.Redraw = function() {
		   console.log("ParemeterEditor Redraw...");
		   this.fillMemberTable();
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
               if (name == 'EventCount') $('#'+divid + " .total_events").text(value);
            }
            
         });
         
         xreq.send(null);
      }
       
      setInterval(UpdateStatus, 2000);
   }

})();
