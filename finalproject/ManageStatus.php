<?php include "CheckLogin.php";?>
<!DOCTYPE html>
<html>
    <head>
        <meta charset="utf-8">
        <title>管理狀態</title>
        <meta name="description" content="">
        <meta name="author" content="">
        <?php include"_html/_html_head.php"; ?>
    </head>

    <!-- #BODY -->

    <body class="desktop-detected pace-done smart-style-3 fixed-navigation fixed-header fixed-ribbon">

        <!-- HEADER -->
        <header id="header">

            <?php include "_html/_header_logo-group.php";?>

            <!-- projects dropdown -->
            <?php include "_html/_header_project-dropdown.php";?>
            <!-- end projects dropdown -->

            <!-- pulled right: nav area -->
            <?php include "_html/_header_right_nav.php";?>
            <!-- end pulled right: nav area -->

        </header>
        <!-- END HEADER -->

        <!-- #NAVIGATION -->
        <!-- Left panel : Navigation area -->
        <!-- Note: This width of the aside area can be adjusted through LESS variables -->
        <?php include "_html/_navigation.php"?>
        <!-- END NAVIGATION -->

        <!-- MAIN PANEL -->
        <div id="main" role="main">

            <!-- RIBBON -->
            <div id="ribbon">
                <!-- breadcrumb -->
                <ol class="breadcrumb">
                    <li>管理狀態</li>
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
                                <div style="padding:1px 1px 0;">

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
                                                <form id="form" method="POST" >
                                                    <table class="table table-bordered table-striped" style="background-color: #f9f9f9; margin-bottom: 1px;">
                                                        <tbody>                                                            
                                                            <tr>
                                                                <td class="bg-color-blue txt-color-white text-align-right">新增狀態</td>
                                                                <td class="input-padding">
                                                                    <input class="form-control input-xs" placeholder="狀態名稱" type="text" id="statusName" name="statusName" required="required">
                                                                </td>
                                                                <td class="bg-color-blue txt-color-white  text-align-right">使用情況</td>
                                                                <td class="input-padding">
                                                                    <select class="form-control input-xs" id="selectEnable" name="selectEnable">
                                                                        <option value="1">啟用</option>
                                                                        <option value="0">停用</option>
                                                                    </select>
                                                                </td>                                                                
                                                            </tr>

                                                        </tbody>
                                                    </table>
                                                </form>
                                            </div>
                                            <div class="row">
                                                <div class="col-md-12 text-center">
                                                    <a class="btn btn-primary btn-sm" href="javascript:void(0);" id="addBtn">新增</a>
                                                </div>
                                            </div>
                                            <div id="infoBar" class="alert alert-info no-margin fade in">
                                                <i class="fa-fw fa fa-info"></i>
                                                管理狀態 <code>(<span id="rowcount"></span>)</code>
                                            </div>
                                            <div id="errorBar" class="alert alert-danger no-margin fade in" style="display: none;">
                                                <i class="fa-fw fa fa-times"></i>
                                                管理狀態錯誤 !
                                            </div>
                                            <div class="table-responsive">
                                                <table id="tableResult" class="table table-bordered table-striped">
                                                    <thead>
                                                        <tr>
                                                            <th></th>
                                                            <th>狀態id</th>
                                                            <th>狀態名稱</th>
                                                            <th>使用情況</th>
                                                            <th>新增時間</th>
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
        <?php include "_html/_footer.php"?>
        <!-- END PAGE FOOTER -->

        <!-- ================================================== -->

        <?php include "_html/_js.php"?>
        <!-- PAGE RELATED PLUGIN(S)
        <script src="..."></script>-->

        <?php include "_html/_init_script.php"?>
        <script>
            // 網頁載入的預設動作
            $(document).ready(function () {                
                getStatus();

                // 開始新增
                $('#addBtn').click(function () {
                    if ($.trim($("#statusName").val()) == "") {
                        alert("必須填入狀態名稱");
                        return;
                    }

                    $('#infoBar').append("<h1 id='loadingloading' class='ajax-loading-animation'><i class='fa fa-cog fa-spin'></i> Loading...</h1></div>");
                    $.ajax({
                        url: "AddStatus.php",
                        data: {
                            statusName: $("#statusName").val(),
                            selectEnable: $("#selectEnable").val()
                        },
                        success: function (data) {
                            alert("新增成功");
                            getStatus();
                        },
                        error: function (xhr, ajaxOptions, thrownError) {
                            alert("新增失敗 !");
                            console.log(xhr.responseText);
                        },
                        complete: function(){
                            $('#loadingloading').remove();
                        },
                        dataType: 'json'
                    });
                });                
            });

            function getStatus()
            {
                $("#resultTbody").empty();
                $.ajax({
                    url: "GetStatus.php",
                    data: {},
                    success: function (data) {
                        $('#errorBar').hide();
                        $('#infoBar').show();

                        var result = "";
                        for (var i = 0; i < data.length; i++) {
                            result = result + "<tr>\n";
                            result = result + "<td><div class='col-md-12 text-center'><a class='btn btn-primary btn-sm' onClick='changeStatus(" + i + ")' id='changeBtn" + i + "'>修改</a></div></td>\n";
                            result = result + "<td id='status_id" + i + "'>" + data[i]['status_id'] + "</td>\n";
                            result = result + "<td><input class='form-control input-xs' type='text' id='changeStatus" + i + "' name='changeStatus" + i + "' value='" + data[i]['status_name'] + "'></td>\n";   

                            if (data[i]['enable'] == 1) {
                                result = result + "<td><select class='form-control input-xs' id='changeEnable" + i + "' name='changeEnable" + i + "'>";
                                result = result + "<option value='1' selected>啟用</option>";
                                result = result + "<option value='0'>停用</option>";
                                result = result + "</select></td>\n";
                            } else {
                                result = result + "<td><select class='form-control input-xs' id='changeEnable" + i + "' name='changeEnable" + i + "'>";
                                result = result + "<option value='1'>啟用</option>";
                                result = result + "<option value='0' selected>停用</option>";
                                result = result + "</select></td>\n";
                            }                            
                            result = result + "<td>" + data[i]['create_time'] + "</td>\n";
                            result = result + "</tr>\n";
                        }

                        $('#resultTbody').html(result);
                        $('#rowcount').text(data.length);
                    },
                    error: function (xhr, ajaxOptions, thrownError) {
                        alert("發生錯誤 !");
                        console.log(xhr.responseText);
                    },
                    complete: function(){
                        $('#loadingloading').remove();
                    },
                    dataType: 'json'
                });
            }

            function changeStatus(index)
            {
                if ($.trim($("#changeStatus" + index).val()) == "") {
                    alert("必須填入狀態名稱");
                    return;
                }

                $('#infoBar').append("<h1 id='loadingloading' class='ajax-loading-animation'><i class='fa fa-cog fa-spin'></i> Loading...</h1></div>");
                $.ajax({
                    url: "ChangeStatus.php",
                    data: {
                        statusId: $("#status_id" + index).html(),
                        changeStatus: $("#changeStatus" + index).val(),
                        changeEnable: $("#changeEnable" + index).val()
                    },
                    success: function (data) {
                        alert("修改成功");
                        getStatus();
                    },
                    error: function (xhr, ajaxOptions, thrownError) {
                        alert("修改失敗 !");
                        console.log(xhr.responseText);
                    },
                    complete: function(){
                        $('#loadingloading').remove();
                    },
                    dataType: 'json'
                });
            }
        </script>

    </body>

</html>
