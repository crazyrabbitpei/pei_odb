var page=1;    
$(document).ready(function(){
            createFileBlock("LIST","POST","4","4","","normal",page,function(result){
                page = result;
            });
});

$("#add_dir").click(function(){
    var dir = document.createElement("input");
    dir.setAttribute("class","files dir");
    dir.setAttribute("type","textarea");
    document.getElementById("file_block").appendChild(dir);
    
    $("input.files.dir").focusout(function(){
        var name = $(this).val();
        console.log("dir name:"+name);
        
        sendCommand("CDIR",name,"filename","","POST",function(result){
            console.log("result:"+result);
        });
        
    });
});

function sendCommand(command,filename,par,page,method,callback){
         var formData = new FormData();
         formData.append(par,filename);
         formData.append('command',command);
         formData.append('page',page);
        $.ajax({
            'url':'/pei/odb.cgi',
            'data':formData,
            'processData': false,  // tell jQuery not to process the data
            'contentType': false,  // tell jQuery not to set contentType
            'type':method,
            'success':function(data){
                callback(data);
            },
            'error':function(xhr,ajaxOptions, thrownError){
                        console.log(xhr.status);
                        console.log(thrownError);
            }
        });
}

function createFileBlock(command,method,row,column,sfilename,test,page,callback){
        $.ajax({
            'url':'/pei/odb.cgi',
            'data': 'command='+command+'&search='+sfilename+'&page='+page,
            'type': method,
            'success':function(data){
                if(test!="demo"){
                    var arr = data.split("</br>");
                    console.log(arr);
                    if(arr.length==1){
                        if(test=="down"){
                            page = page-1;
                            callback(page);
                            return ;
                        }
                        else if(test=="up"){
                            page = page+1;
                            callback(page);
                            return;
                        }
                    }
                    $(".nothing").remove();
                    $(".file").remove();
                    if(arr[0]=="-1,-1,-1,-1"){
                        return -1;
                    }
                }
                else if(test=="demo"){
                    var arr=[];
                    for(i=0;i<=33;i++){
                        arr.push(i+","+i+","+i+","+i);
                    }
                }
                var filename="";

                var detail=[];
                var key=[];
                var offset=[];

                for(i=0;i<arr.length-1;i++){
                     var data = arr[i].split(",");
                     key.push(data[0]);
                     offset.push(data[1]);
                     detail.push(data[2]);
                     var filename = data[2].match(/@name:.*/);
                     filename = filename[0].replace("@name:","");
                     var type = data[2].match(/@type:.*/);
                     type = type[0].replace("@type:","");
                    newfile(filename,type);

                }
            },
            'error':function(xhr,ajaxOptions, thrownError){
            console.log(xhr.status);
            console.log(thrownError);
            }
        });
}

function newfile(filename,type){
        var div = document.createElement("div");
        console.log("type:"+type);
        switch(type){
            case "dir":
                    type = "files dir";
                    break;
            case "application/pdf":
                    type = "files pdf";
                    break;
            case "application/vnd.ms-powerpoint":
            case "application/vnd.openxmlformats-officedocument.presentationml.presentation":
                    type = "files ppt";
                    break;
            case "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet":
                    type = "files xls";
                    break;
            case "application/msword":
            case "application/vnd.openxmlformats-officedocument.wordprocessingml.document":
                    type = "files doc";
                    break;
            case "application/zip":
            case "application/x-gzip":
                    type = "files zip";
                    break;
            case "application/json":
                    type = "files json";
                    break;
            case "image/png":
            case "image/jpeg":
                    type = "files image";
                    break;
            case "audio/mp3":
                    type = "files audio";
                    break;
            default:
                    type = "files default";
                    break;
        }

        div.setAttribute("class",type);
        document.getElementById("file_block").appendChild(div);
        div.innerHTML = filename;
}

function list(filename){
    $("#post_filename").val(filename);
    document.getElementById('post_form').submit();
}
function del(filename){
    $("#del_filename").val(filename);
    document.getElementById('del_form').submit();
}
function rename(filename,newfilename){
    $("#o_filename").val(filename);
    $("#re_filename").val(newfilename);

    $("input[name='rename_input']").val(newfilename);
    if(filename!=newfilename){
        document.getElementById('re_form').submit();
    }
}

