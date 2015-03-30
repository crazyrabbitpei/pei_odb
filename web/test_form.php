<html>
<head>
<meta http-equiv="content-type" content="text/html; charset=utf-8" />

<title>Upload</title>

<script src="https://code.jquery.com/jquery-1.10.2.js"></script>
<script src="//ajax.googleapis.com/ajax/libs/jquery/1.8.3/jquery.min.js"></script>
</head>
<body>
<?php
// Use fopen function to open a file
$file = fopen("address", "r");
$value = fgets($file);
// Close the file that no longer in use
fclose($file);
?>
<FORM id="form1" METHOD="POST" ACTION="<?php echo $value; ?>" enctype="multipart/form-data"> 
Upload<input type="radio" name="command" value="PUT" />
<INPUT TYPE="file" NAME="file" VALUE=""></br>
List<input type="radio" name="command" value="LIST" checked/></br>
Detail<input type="radio" name="command" value="DETAIL" /></br>
<INPUT TYPE="submit" VALUE="go"> 
</FORM>



</body>

<script type="text/javascript" charset="utf-8">
$(document).ready(function(){
    $.ajax({
         'url':'http://140.123.101.181:3636/pei/odb.cgi',
        'data': $('form').serialize(),
        'type': 'POST',
        'success':function(data){console.log(data)},
        'error':function(xhr,ajaxOptions, thrownError){
            console.log(xhr.status);
            console.log(thrownError);
                }
    });
});
</script>
</html>

