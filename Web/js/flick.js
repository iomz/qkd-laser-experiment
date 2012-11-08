$(function() {
	var element = $('#flickable').flickable({
			section: 'li',
			friction: 0
	});

	$('#select_box td a').click(
		function() {
			var index = $(this).text() - 1;
			element.flickable('select', index);
			return false;
		});

	$('#send').click(
		 function(){
			 element.flickable('select', 1);
			 return false;
		 });
});