var length = 20;
function setRandomAlice(){
	var randNum;
	for(var i=0;i<length;i++){
		randNum = Math.floor( Math.random() * 2 );
		if(randNum == 1)	changeBit("abit"+i);
		randNum = Math.floor( Math.random() * 2 );
		if(randNum == 1)	changeBase("abase"+i);
	}
}

function setRandomBob(){
	var randNum;
	for(var i=0;i<length;i++){
		randNum = Math.floor( Math.random() * 2 );
		if(randNum == 1)	changeBase("bbase"+i);
	}
}

function setValAlice(){
	var val = document.getElementById("alice");
	val.value = "";
	for(var i=0;i<length;i++){
		val.value += document.getElementById("abit"+i).value;
	}
	for(var i=0;i<length;i++){
		var bit;
		if(document.getElementById("abase"+i).value == "+") bit = "0";
		else bit = "1";
		val.value += bit;
	}
}

function setValBobAndSubmit(){
	var val = document.getElementById("bob");
	val.value = "";
	for(var i=0;i<length;i++){
		var bit;
		if(document.getElementById("bbase"+i).value == "+") bit = "0";
		else bit = "1";
		val.value += bit;
	}

	document.frm.submit();
}
