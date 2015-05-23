
var fileUpload = document.querySelectorAll('body').item(0);
fileUpload.addEventListener('drop', uploadDrop , false);
fileUpload.addEventListener('dragover', handleDragOver, false);
//fileUpload.addEventListener('dragenter', handleDragEnter, false);
//fileUpload.addEventListener('dragleave', handleDragLeave, false);

function uploadDrop(e) {
    e.stopPropagation(); // Stops some browsers from redirecting.
    e.preventDefault();
    if(e.dataTransfer.types.indexOf('Files') == -1){
        message("not Files");
        return;
    }
    var files = e.dataTransfer.files;
    upload(files,0,files.length);
}
function handleDragOver(e) {
      e.stopPropagation(); // Stops some browsers from redirecting.
      e.preventDefault();
}
function handleDragEnter(e) {
      e.stopPropagation(); // Stops some browsers from redirecting.
        e.preventDefault();
}
function handleDragLeave(e) {
      e.stopPropagation(); // Stops some browsers from redirecting.
        e.preventDefault();
}
function upload(files,cnt,len){ 
    // Read the File objects in this FileList.
                  console.log("files:"+files[cnt].name);
                  var formData = new FormData();
                  formData.append('filename',files[cnt]);
                  formData.append('command',"PUT");
                  $.ajax({
                    'url':'/pei/odb.cgi',
                    'data':formData,
                    'processData': false,  // tell jQuery not to process the data
                    'contentType': false,  // tell jQuery not to set contentType
                    'type':'POST',
                    'success':function(data){
                        //console.log(data);
                        var div = document.getElementById("upload_status");
                        div.innerHTML+=data;
                        var type = data.match(/The alleged content type of the file was:.*/);
                        type = type[0].replace("The alleged content type of the file was:","");
                        type = type.replace("<p>","");
                        type = type.replace(" ","");
                        newfile(files[cnt].name,type);
                        cnt++;
                        if(cnt==len){return;}
                        upload(files,cnt,len);
                    },
                    'error':function(xhr,ajaxOptions, thrownError){
                        console.log(xhr.status);
                        console.log(thrownError);
                    }
                  })

    
}
