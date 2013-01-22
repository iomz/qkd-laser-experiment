function changeBit(id){
	var button = document.getElementById(id);
	if(button.value == "0"){
		button.value = "1";
		button.innerHTML = "<img src='./button/1.png'>";
	}else{
		button.value = "0";
		button.innerHTML = "<img src='./button/0.png'>";
	}
}

function changeBase(id){
	var button = document.getElementById(id);
	if(button.value == "+"){
		button.value = "×"
			button.innerHTML = "<img src='./button/t.png'>";
	}else{
		button.value = "+";
		button.innerHTML = "<img src='./button/p.png'>";
	}
}

var bufID;
function downimg(id){
	var button = document.getElementById(id);
	if(button.value == "0")			button.innerHTML = "<img src='./button/0p.png'>";
	else if(button.value == "1")	button.innerHTML = "<img src='./button/1p.png'>";
	else if(button.value == "+")	button.innerHTML = "<img src='./button/pp.png'>";
	else if(button.value == "×")   button.innerHTML = "<img src='./button/tp.png'>";
	bufID = id;
}
function upimg(id, isUp){
	var button = document.getElementById(id);
	
	if(isUp == 1 && id == bufID){
		if(button.value == "0" || button.value == "1")	changeBit(id);
		else											changeBase(id);
	}else if(isUp == 0){
		if(button.value == "0")			button.innerHTML = "<img src='./button/0.png'>";
		else if(button.value == "1")	button.innerHTML = "<img src='./button/1.png'>";
		else if(button.value == "+")	button.innerHTML = "<img src='./button/p.png'>";
		else if(button.value == "×")	button.innerHTML ="<img src='./button/t.png'>";
	}
}