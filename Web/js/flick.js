$(function() {
	var element = $('#flickable').flickable({
			section: 'li',
		});

	$('#select_box td a').click(
		function() {
			var index = $(this).text() - 1;
			element.flickable('select', index);
			return false;
		});

	$('#send').click(
		 function(){
			 element.flickable('select', 4);
			 return false;
		 });

	$('#toEve').click(
		 function(){
			 element.flickable('select', 5);
			 return false;
		 });
	$('#again').click(
		 function(){
			 element.flickable('select', 3);
			 return false;
		 });
});