function run() {
	var param = 'alice=' + $('#alice').val() + '&bob=' + $('#bob').val() + '&eve=' + $('#eve').val();
	var load_body = "";
	load_body += "<div id=\"wrap\">\n";
	load_body += "<img border=\"0\" src=\"load/load.gif\" style=\"position:relative;top:250px;\"/>\n";
	load_body += "</div>\n";
	$('body').html(load_body);

	$.ajax({
		type:    "POST",
		url:     "output.cgi",
		data:    param,
		cache:   false,
		success: sucFunc,
		error:   errFunc,
		timeout: 60000,
	});


	function sucFunc(data, status, xhr){
		$('body').html(data);
		$(flick);
		$(setRandomAll);
	}

	function errFunc(request, status, err){
		if(status == "timeout"){
			alert('timeout');
		}else{
			alert("error");
		}

		location.reload();
	}
}