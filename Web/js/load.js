function run() {
	var param = 'alice=' + $('#alice').val() + '&bob=' + $('#bob').val() + '&eve=' + $('#eve').val();
	var load_body = "";
	load_body += "<div id=\"wrap\">\n";
	load_body += "<img border=\"0\" src=\"load/load.gif\" style=\"position:relative;top:250px;\"/>\n";
	load_body += "</div>\n";
	$('body').html(load_body);

	$.post(
		   "output.cgi",
		   param,
		   getData
		   );


	function getData(data){
		$('body').html(data);
		$(flick);
		$(setRandomAll);
	}
}