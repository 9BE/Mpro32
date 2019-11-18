
#ifndef PAGES_H_
#define PAGES_H_

const char ota[] PROGMEM = R"=====(
	<style>
	.example1 {
	  font: 12px/22px Courier;
	  color: DarkRed;
	  }
	.bold {
	  font-weight: bold;
	  }
	.italic {
	  font-style: italic;
	  }
	</style>

	<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script>
	<form method='POST' action='#' enctype='multipart/form-data' id='upload_form'>
		<input type='file' name="update">
		<input type='submit' value='Update'>
		<a href="/reboot">Reboot</a>
	</form>
	<div id='prg'></div>
	<div id="dd" class="example1"></div>
	<script>
	var busy = 0;
	var timer;
	$('form').submit(function(e){
		e.preventDefault();
		var form = $('#upload_form')[0];
		var data = new FormData(form);
		$.ajax({
			url: '/update',
			type: 'POST',
			data: data,
			contentType: false,
			processData:false,
			xhr: function() {
				var xhr = new window.XMLHttpRequest();
				xhr.upload.addEventListener('progress', function(evt) {
				if (evt.lengthComputable) {
					var per = evt.loaded / evt.total;
					busy = 1;
					$('#prg').html('progress: ' + Math.round(per*100) + '%   &nbsp;&nbsp;&nbsp;&nbsp;' + evt.loaded + '/' + evt.total);
				}
	  		}, false);
	  		return xhr;
	  		},
	  		success:function(d, s) {
	  			console.log('success!');
				busy = 0;
				tengokData();
				$('#prg').html('progress: <b>done</b>');
	 		},
	 		error: function (a, b, c) {
				$('#prg').html('progress: <b>fail</b>');
	 		}
	 	});
	 });
	function tengokData(){
		if(busy == 0){

			$.ajax({
				type: "GET",
				url: "/stat",
				data: "",
				dataType: "text",
				async:   true,
				timeout: 600,
				success: function(data) {
					timer = new Timer(function() {	tengokData();}, 3000);
					$('#dd').html(data);
				},
				error: function() {
					timer = new Timer(function() {	tengokData();}, 3000);
	//				$('#dd').html("n/a");
				}
			});

		}



	}

	$(document).ready(function(){
		tengokData();
	});


	function Timer(callback, delay) {
		var timerId, start, remaining = delay;
	
		this.pause = function() {
			window.clearTimeout(timerId);
			remaining -= new Date() - start;
		};
		
		this.resume = function() {
			start = new Date();
			window.clearTimeout(timerId);
			timerId = window.setTimeout(callback, remaining);
		};
		
		this.resume();
	}
	 </script>)=====";

#endif

