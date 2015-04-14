var fileUpload = document.querySelectorAll('#body').item(0);
fileUpload.addEventListener('drop', uploadDrop , false);
fileUpload.addEventListener('dragover', handleDragOver, false);
//fileUpload.addEventListener('dragenter', handleDragEnter, false);
//fileUpload.addEventListener('dragleave', handleDragLeave, false);

function uploadDrop(e) {
      e.stopPropagation(); // Stops some browsers from redirecting.
        e.preventDefault();
          var files = e.dataTransfer.files;
          for (var i = 0; i<files.length; i++) {
                  // Read the File objects in this FileList.
                  console.log("files:"+files[i].name);
                  var formData = new FormData();
                  formData.append('filename',files[i]);
                  formData.append('command',"PUT");
                  $.ajax({
                    'url':'/pei/odb.cgi',
                    'data':formData,
                    'processData': false,  // tell jQuery not to process the data
                    'contentType': false,  // tell jQuery not to set contentType
                    'type':'POST',
                    'success':function(data){
                        console.log(data);
                    },
                    'error':function(xhr,ajaxOptions, thrownError){
                        console.log(xhr.status);
                        console.log(thrownError);
                    }
                  })
          }
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

