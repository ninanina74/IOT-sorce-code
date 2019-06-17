<!DOCTYPE html>
<html>
     <head>
        <meta charset="utf-8">
        <title>查詢</title>
        <meta name="description" content="">
        <meta name="author" content="">
        <?php include"_html/_html_head.php"; ?>
    </head>
    <!-- #BODY -->

    <body class="desktop-detected pace-done smart-style-3 fixed-navigation fixed-header fixed-ribbon">

        <!-- HEADER -->
       
        <!-- END HEADER -->

        <!-- #NAVIGATION -->
        <!-- Left panel : Navigation area -->
        <!-- Note: This width of the aside area can be adjusted through LESS variables -->
        

        <!-- MAIN PANEL -->
        <div id="main" role="main">

            <!-- RIBBON -->
            <div id="ribbon">
                <!-- breadcrumb -->
                <ol class="breadcrumb">
                    <li>物品查詢</li>
                </ol>
                <!-- end breadcrumb -->

            </div>
            <!-- END RIBBON -->



            <!-- MAIN CONTENT -->
            <div id="content">

                <!-- row -->
                <!-- end row -->

                <!--
                        The ID "widget-grid" will start to initialize all widgets below
                        You do not need to use widgets if you dont want to. Simply remove
                        the <section></section> and you can use wells or panels instead
                -->

                <!-- widget grid -->
                <section id="widget-grid" class="">

                    <!-- row -->
                    <div class="row">

                        <!-- NEW WIDGET START -->
                        <article class="col-xs-12 col-sm-12 col-md-12 col-lg-12">

                            <!-- Widget ID (each widget will need unique ID)-->

                            <!-- Widget ID (each widget will need unique ID)-->
                            <div class="jarviswidget" id="wid-id-8" data-widget-colorbutton="false" data-widget-editbutton="false" data-widget-togglebutton="false" data-widget-deletebutton="false" data-widget-fullscreenbutton="false" data-widget-custombutton="false" data-widget-sortable="false">

                                <!-- widget div-->
                                <div style="padding:2px 2px 0;">

                                    <!-- widget edit box -->
                                    <div class="jarviswidget-editbox">
                                        <!-- This area used as dropdown edit box -->

                                    </div>
                                    <!-- end widget edit box -->

                                    <!-- widget content -->
                                    <div class="widget-body" style="padding-bottom: 0px">
                                        <!--<div class="tab-content">-->
                                        <div class="tab-pane active" id="hb000">

                                            <div class="table-responsive">
                                                <form id="message_form" method="POST" >
                                                    <table class="table table-bordered table-striped" style="background-color: #f9f9f9; margin-bottom: 1px;">
                                                        <tbody>
                                                            <tr>
                                                            
                                                            </tr>

                                                        </tbody>
                                                    </table>
                                                </form>
                                            </div>
                                            
                                            <div id="infoBar" class="alert alert-info no-margin fade in">
                                                <i class="fa-fw fa fa-info"></i>
                                                紀錄查詢 <code>(<span id="rowcount"></span>)</code>
                                            </div>
                                            <div id="errorBar" class="alert alert-danger no-margin fade in" style="display: none;">
                                                <i class="fa-fw fa fa-times"></i>
                                                紀錄查詢錯誤 !
                                            </div>
                                            <div class="table-responsive">
                                                <table id="tableResult" class="table table-bordered table-striped">
                                                    <thead>
                                                        <tr>
                                                            <th>總共水洗次數</th>
                                                            <th>最後水洗時間</th>
                                                        </tr>
                                                    </thead>
                                                    <tbody id="resultTbody">
                                                    </tbody>
                                                </table>
                                            </div>
                                        </div>
                              <!--</div>-->
                                    </div>
                                    <!-- end widget content -->
                                </div>
                                <!-- end widget div -->
                                <!-- end widget -->
                        </article>
                        <!-- WIDGET END -->
                    </div>
                    <!-- end row -->
                    <!-- row -->
                    <div class="row">
                        <!-- a blank row to get started -->
                        <div class="col-sm-12">
                            <!-- your contents here -->

                            <div class="widget-body">

                            </div>
                        </div>

                    </div>

                    <!-- end row -->

                </section>
                <!-- end widget grid -->

            </div>
            <!-- END MAIN CONTENT -->

        </div>
        <!-- END MAIN PANEL -->

        <!-- PAGE FOOTER -->
        <!-- <?php include "_html/_footer.php"?>-->
        <!-- END PAGE FOOTER -->

        <!-- ================================================== -->

        <?php include "_html/_js.php"?>
        <!-- PAGE RELATED PLUGIN(S)
        <script src="..."></script>-->

        <?php include "_html/_init_script.php"?>
        <script>
            // 網頁載入的預設動作
            $(document).ready(function () {
              
                // 開始查詢
                //$('#queryBtn').click(function () {
                //$("#resultTbody").empty();
                //$("#itemSerial").val();
                  //$('#infoBar').append("<h1 id='loadingloading' class='ajax-loading-animation'><i class='fa fa-cog fa-spin'></i> Loading...</h1></div>");
                  $.ajax({
 //   url: "getlinebotnina.php",
  //  type: "GET",
   // data: {
   //     itemSerial: '<?= $_GET['itemSerial']; ?>'
   // },
    


   // success: function(response) {
    //    console.log(response);
   // }
  //  });
                    
                        
                      url:'getlinebotnina.php?', 
                         
                         
                         data: {
                            
                             itemSerial: '<?= $_GET['itemSerial']; ?>'
                            
                          },
                          type:"GET",  
                         success: function (data) {
                             $('#errorBar').hide();
                             $('#infoBar').show();

                //             // data.length = 總共洗幾次
                //             // data[0]['create_time'] = 最後水洗時間
                             var result = "";
                //            // for (var i = 0; i < data.length; i++) { 
                             result = result + "<tr>\n";                         
                             result = result + "<td>" + data.length + "</td>\n";
                             result = result + "<td>" + data[0]['create_time'] + "</td>\n";
                             result = result + "</tr>\n";
                               

                             $('#resultTbody').html(result);
                             $('#rowcount').text(data.length);
                         },
                         error: function () {
                           $('#errorBar').show();
                           $('#infoBar').hide();
                         },
                         complete: function(){
                           $('#loadingloading').remove();
                         },
                         dataType: "json"
                     });

                });
            
            
          
            
        </script>

    </body>

</html>
