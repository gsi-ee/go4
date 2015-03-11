(function(){

   if (typeof JSROOT != "object") {
      var e1 = new Error("pareditor.js requires JSROOT to be already loaded");
      e1.source = "pareditor.js";
      throw e1;
   }

   if (typeof GO4 != "object") {
	   var e1 = new Error("pareditor.js requires GO4 to be already loaded");
	   e1.source = "pareditor.js";
	   throw e1;
	}
   
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
          if (JSROOT.hpainter) JSROOT.hpainter.display(editor.GetItemName()); 
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
      $("#"+divid).load( GO4.source_dir + "pareditor.htm", "", 
            function() { pthis.SetDivId(divid); pthis.fillEditor(); pthis.fillComments(); });
   }
   
   GO4.ParameterEditor.prototype.SetItemName = function(name) {
      JSROOT.TBasePainter.prototype.SetItemName.call(this, name);
      this.fillComments();
   }

   GO4.drawParameter = function(divid, par, option, painter) {
      painter = JSROOT.extend(painter, new GO4.ParameterEditor(par));
      painter.drawEditor(divid);
      return painter.DrawingReady();
   }
   
})();
