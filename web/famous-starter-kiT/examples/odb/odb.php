<!DOCTYPE HTML>
<html>
    <head>
        <meta http-equiv="content-type" content="text/html; charset=utf-8" />
        <title>famo.us App</title>
        <meta name="viewport" content="width=device-width, maximum-scale=1, initial-scale=1, user-scalable=no" />
        <meta name="mobile-web-app-capable" content="yes" />
        <meta name="apple-mobile-web-app-capable" content="yes" />
        <meta name="apple-mobile-web-app-status-bar-style" content="black" />
        <link rel="stylesheet" type="text/css" href="../../src/core/famous.css" />
        <link rel="stylesheet" type="text/css" href="../assets/css/famous_styles.css" />
        <link rel="stylesheet" type="text/css" href="./css/style.css" />
    </head>
    <body>
    <?php
    // Use fopen function to open a file
    $file = fopen("/var/www/cgi-bin/pei_odb/web/address", "r");
    $value = fgets($file);
    // Close the file that no longer in use
    fclose($file);
    ?>
    <FORM id="form1" METHOD="POST" ACTION="<?php echo $value; ?>" enctype="multipart/form-data">
    Upload<input type="radio" name="command" value="PUT" />
    <INPUT TYPE="file" NAME="filename" VALUE=""></br>
    List<input type="radio" name="command" value="LIST" checked/></br>
    Detail<input type="radio" name="command" value="DETAIL" /></br>
    <INPUT TYPE="submit" VALUE="go">
    </FORM>
    <form method="post" id="post_form" style="visibility:hidden;" action="<?php echo $value; ?>">
        <input type="hidden" id="command" name="command" value="GET">
        <input type="hidden" id="post_filename" name="filename" value="">
        <input type="submit" value="Submit">
    </form>
    <form method="post" id="del_form" style="visibility:hidden;" action="<?php echo $value; ?>">
        <input type="hidden" id="command" name="command" value="DEL">
        <input type="hidden" id="del_filename" name="filename" value="">
        <input type="submit" value="Submit">
    </form>
    <form method="post" id="re_form" style="visibility:hidden;" action="<?php echo $value; ?>">
        <input type="hidden" id="command" name="command" value="RENAME">
        <input type="hidden" id="o_filename" name="filename" value="">
        <input type="hidden" id="re_filename" name="newfilename" value="">
        <input type="submit" value="Submit">
    </form>

    <div id='download'></div>
    <script src="https://code.jquery.com/jquery-1.10.2.js"></script>
    <script src="//ajax.googleapis.com/ajax/libs/jquery/1.8.3/jquery.min.js"></script>
    <script type="text/javascript" src="../assets/lib/functionPrototypeBind.js"></script>
    <script type="text/javascript" src="../assets/lib/classList.js"></script>
    <script type="text/javascript" src="../assets/lib/requestAnimationFrame.js"></script>
    <script type="text/javascript" src="../assets/lib/require.js"></script>
    <script type="text/javascript">
    require.config({
        paths: {
        famous: '../../src'
        }
    });
    require(['fam_test']);
    </script>

    </body>
</html>
