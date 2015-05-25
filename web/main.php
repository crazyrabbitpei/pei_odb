<!DOCTYPE HTML>
<html>
    <head>
        <meta http-equiv="content-type" content="text/html; charset=utf-8" />
        <link rel="stylesheet" type="text/css" href="css/main.css">
        <title>My cloud</title>
    </head>
    <body>
    <?php

    // Use fopen function to open a file
    //$file = fopen("address", "r");
    //$value = fgets($file);
    // Close the file that no longer in use
    //fclose($file);
    $value = "/pei/odb.cgi";
    ?>
    <header>
        <section id="user">
            <img>
            <nav id="user_config">
            </nav>
        </section>

        <section id="info">
            <section id="dir">
                <header id="dir_name">
                </header>
                <nav id="dir_config">
                </nav>
            </section>
            
            <section id="db">
                <header id="db_name">
                </header>
                <span id="db_capacity">
                </span>
            </section>
            
            <nav id="path">
            </nav>
            
        </section>
        
        <summary id="dir_note">
        </summary>
    </header>



    <main id="main_block">
        <aside id="left">
            
            <section id="file_info">
                <img>
                </img>
                <detail id="descrip">
                    <article>
                    </article>
                </detail>
                <nav>
                </nav>
            </section>
            <div id="upload_status">
            </div>
            
        </aside>

        <main id="center">
            

            <nav id="file_config">
                <section id="search">
                    <input type="textarea">
                    </input>
                    <select id=searchby>
                        <option value="name">Name</option>
                        <option value="type">Type</option>
                    </select>
                </section>
                <input type="button" id="add_dir" value="+"/>
                <!--<form id="upload" method="POST" action="/pei/odb.cgi" enctype="multipart/form-data">-->
                    <input type="hidden" name="command" value="PUT" />
                    <input type="hidden" name="path" value="" />
                    <input type="file" name="filename" value=""/>
                    <input id="upload" type="submit" value="Upload"/>
                <!--</form>-->
                <select id=sortby>
                    <option value="upload">Upload Time</option>
                    <option value="name">Name</option>
                    <option value="type">Type</option>
                </select>
            </nav>

            <section id="file_block">
            </section>
            <nav id="page">
            </nav>
            
        </main>

        <aside id="right">
            <section id="search">       
                 <input type="texterea">
                </input>
            </section>
            <section id="dir_list">
            </section>
            
        </aside>

    </main>
    <!--
    <form method="post" id="get_form" action="<?php echo $value; ?>">
        <input id="command" name="command" value="GET">
        <input id="post_filename" name="filename" value="">
        <input type="submit" value="Submit">
    </form>
    <form method="post" id="del_form" action="<?php echo $value; ?>">
        <input id="command" name="command" value="DEL">
        <input id="del_filename" name="filename" value="">
        <input type="submit" value="Submit">
    </form>
    <form method="post" id="re_form" action="<?php echo $value; ?>">
        <input id="command" name="command" value="RENAME">
        <input id="o_filename" name="filename" value="">
        <input id="re_filename" name="newfilename" value="">
        <input type="submit" value="Submit">
    </form>
    -->
    <!--<script src="https://code.jquery.com/jquery-1.10.2.js"></script>-->
    <script src="jquery/jquery-1.10.2.js"></script>
    <script src="jquery/jquery.min.js"></script>
    <!--<script src="https://ajax.googleapis.com/ajax/libs/jquery/1.8.3/jquery.min.js"></script>-->
    <script src="drag_drop.js"></script>
    <script src="command.js"></script>
    </body>
</html>
