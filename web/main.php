<!DOCTYPE HTML>
<html>
    <head>
        <meta http-equiv="content-type" content="text/html; charset=utf-8" />
        <link rel="stylesheet" type="text/css" href="css/main.css?<?php echo date()?>">
        <!--<link rel="stylesheet" type="text/css" href="css/main.css?<?php echo date()?>">-->
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
                <div id="file_image"></div>
                <div id="file_content"></div>
                <div id="file_descrip"></div>
                <div id="file_tag"></div>
            </section>
            <div id="upload_status">
            </div>
            
        </aside>

        <main id="center">
            

            <nav id="file_config">
                <section id="search">
                    <select id=searchby>
                        <option value="@all"> ----All---- </option>
                        <option value="@filename">Name</option>
                        <option value="@ds">Description</option>
                        <option value="@tag">Tag</option>
                        <option value="@type">Type</option>
                    </select>
                    <input type="text">
                    </input>
                    <input id="search_file" type="button" value="search" onclick="search()"></input>
                    
                    <input type="radio" name="type" value="all">全部</input>
                    <input type="radio" name="type" value="file">檔案</input>
                    <input type="radio" name="type" value="dir">資料夾</input>

                </section>
                <input type="button" id="add_dir" value="+"/>
                <!--<form id="upload" method="POST" action="/pei/odb.cgi" enctype="multipart/form-data">-->
                    <input type="hidden" name="command" value="PUT" />
                    <input type="hidden" name="path" value="" />
                    <div id="upload">
                        <input type="file" name="filename" value=""/>
                        <input type="submit" value="Upload"/>
                    </div>
                <!--</form>-->
                <select id=sortby>
                    <option value="@ctime">Upload Time</option>
                    <option value="@filename">Name</option>
                    <option value="@type">Type</option>
                </select>
            </nav>

            <section id="file_block">
            </section>
            <nav id="page">
            </nav>
            
        </main>

        <aside id="right">
            <section id="search">       
            </section>
            <section id="dir_list">
            </section>
            
        </aside>

    </main>
    
    <form method="post" id="get_form" action="<?php echo $value; ?>">
        <input id="command" name="command" value="GET">
        <input id="get_filename" name="filename" value="">
        <input id="get_fileid" name="getid" value="">
        <input type="submit" value="Submit">
    </form>
    
    <!--<script src="https://code.jquery.com/jquery-1.10.2.js"></script>-->
    <script src="jquery/jquery-1.10.2.js"></script>
    <script src="jquery/jquery.min.js"></script>
    <!--<script src="https://ajax.googleapis.com/ajax/libs/jquery/1.8.3/jquery.min.js"></script>-->
    <script src="drag_drop.js?test=<?php echo date()?>"></script>
    <script src="command.js?test=<?php echo date()?>"></script>
    </body>
</html>

