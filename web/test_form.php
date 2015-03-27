<html>
<head>
    <meta http-equiv="content-type" content="text/html; charset=utf-8" />

    <title>Upload</title>
    
</head>
<body>
<?php
// Use fopen function to open a file
$file = fopen("address", "r");
$value = fgets($file);
// Close the file that no longer in use
fclose($file);
?>
<FORM METHOD="POST" ACTION="<?php echo $value; ?>" enctype="multipart/form-data"> 
    Upload<input type="radio" name="command" value="PUT" />
    <INPUT TYPE="file" NAME="file" VALUE=""></br>
    List<input type="radio" name="command" value="LIST" /></br>
    Detail<input type="radio" name="command" value="DETAIL" /></br>
    <INPUT TYPE="submit" VALUE="go"> 
</FORM>

</body>
</html>

