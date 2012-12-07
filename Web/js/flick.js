$(function() {
	var element = $('#flickable').flickable({
			section: 'li',
     		drag: function(event) {return false;}
		});

	$('#select_box td a').click(
		function() {
			var index = $(this).text() - 1;
			element.flickable('select', index);
			return false;
		});

	$('.toResult').click(
		 function(){
			 element.flickable('select', 0);
			 return false;
		 });

	$('.toBob').click(
		 function(){
			 element.flickable('select', 2);
			 return false;
		 });

	$('.toEve').click(
		 function(){
			 element.flickable('select', 3);
			 return false;
		 });
	$('.toAlice').click(
		 function(){
			 element.flickable('select', 1);
			 return false;
		 });
});