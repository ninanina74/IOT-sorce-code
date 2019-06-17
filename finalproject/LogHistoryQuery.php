<?php include "CheckLogin.php";?>
<!DOCTYPE html>
<html>
    <head>
        <meta charset="utf-8">
        <title>歷史紀錄查詢</title>
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
                    <li>歷史紀錄查詢</li>
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
                                                                <td class="bg-color-blue txt-color-white  text-align-right" style="width: 15%;">查詢日期</td>
                                                                <td style="width: 35%;">
                                                                    <input class="form-control input-xs" id="from" name="fromDate" type="text" placeholder="開始日期" style="width: 40%;display: inline;position: relative; z-index: 100000;">
                                                                    &nbsp;~&nbsp;
                                                                    <input class="form-control input-xs" id="to" name="toDate" type="text" placeholder="結束日期" style="width: 40%;display: inline;position: relative; z-index: 100000;">
                                                                    <br>
                                                                    <label class="checkbox-inline">
                                                                        <input type="checkbox" class="checkbox style-0" id="disabledPeriod" name="disabledPeriod" value="Yes">
                                                                        <span>禁用日期</span>
                                                                    </label>
                                                                </td>
                                                                <td class="bg-color-blue txt-color-white text-align-right" style="width: 15%;">地點</td>
                                                                <td class="input-padding" style="width: 35%;">
                                                                    <select class="form-control input-xs" id="selectLocation" name="selectLocation">
                                                                        <option value="ALL">全部</option>
                                                                    </select>
                                                                </td>
                                                            </tr>
                                                            <tr>
                                                                <td class="bg-color-blue txt-color-white  text-align-right">狀態</td>
                                                                <td class="input-padding">
                                                                    <select class="form-control input-xs" id="selectStatus" name="selectStatus">
                                                                        <option value="ALL">全部</option>
                                                                    </select>
                                                                </td>
                                                                <td class="bg-color-blue txt-color-white text-align-right">裝置</td>
                                                                <td class="input-padding">
                                                                    <select class="form-control input-xs" id="selectDevice" name="selectDevice">
                                                                        <option value="ALL">全部</option>
                                                                    </select>
                                                                </td>
                                                            </tr>

                                                        </tbody>
                                                    </table>
                                                </form>
                                            </div>
                                            <div class="row">
                                                <div class="col-md-12 text-center">
                                                    <a class="btn btn-primary btn-sm" href="javascript:void(0);" id="queryBtn">查詢</a>
                                                </div>
                                            </div>
                                            <div id="infoBar" class="alert alert-info no-margin fade in">
                                                <i class="fa-fw fa fa-info"></i>
                                                歷史紀錄查詢 <code>(<span id="rowcount"></span>)</code>
                                            </div>
                                            <div id="errorBar" class="alert alert-danger no-margin fade in" style="display: none;">
                                                <i class="fa-fw fa fa-times"></i>
                                                歷史紀錄查詢錯誤 !
                                            </div>
                                            <div class="table-responsive">
                                                <table id="tableResult" class="table table-bordered table-striped">
                                                    <thead>
                                                        <tr>
                                                            <th>No</th>
                                                            <th>物品</th>
                                                            <th>地點</th>
                                                            <th>狀態</th>
                                                            <th>裝置</th>
                                                            <th>紀錄時間</th>
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
                // 修改登出標題
                // $("#show-shortcut").text();

                // 日曆
                $("#from").datepicker({
                    dateFormat: 'yymmdd',
                    defaultDate: "+1w",
                    changeMonth: true,
                    numberOfMonths: 1,
                    prevText: '<i class="fa fa-chevron-left"></i>',
                    nextText: '<i class="fa fa-chevron-right"></i>',
                    onClose: function (selectedDate) {
                        $("#to").datepicker("option", "minDate", selectedDate);
                    }
                });

                $("#to").datepicker({
                    dateFormat: 'yymmdd',
                    defaultDate: "+1w",
                    changeMonth: true,
                    numberOfMonths: 1,
                    prevText: '<i class="fa fa-chevron-left"></i>',
                    nextText: '<i class="fa fa-chevron-right"></i>',
                    onClose: function (selectedDate) {
                        $("#from").datepicker("option", "maxDate", selectedDate);
                    }
                });                

                // 取得地點
                getLocation();

                // 取得狀態
                getStatus();

                // 取得裝置
                getDevice();

                // 開始查詢
                $('#queryBtn').click(function () {
                    $("#resultTbody").empty();
                    $('#infoBar').append("<h1 id='loadingloading' class='ajax-loading-animation'><i class='fa fa-cog fa-spin'></i> Loading...</h1></div>");
                    $.ajax({
                        url: "GetQueryData.php",
                        data: {
                            dateFrom: $("#from").val(),
                            dateTo: $("#to").val(),
                            disabledPeriod: $("#disabledPeriod").prop("checked") ? "Yes" : "No",
                            location: $("#selectLocation").val(),
                            status: $("#selectStatus").val(),
                            device: $("#selectDevice").val()
                        },
                        success: function (data) {
                            $('#errorBar').hide();
                            $('#infoBar').show();

                            var result = "";
                            for (var i = 0; i < data.length; i++) {
                                result = result + "<tr>\n";
                                result = result + "<td>" + (i + 1) + "</td>\n";
                                result = result + "<td>" + data[i]['item_serial'] + "</td>\n";
                                result = result + "<td>" + data[i]['location_name'] + "</td>\n";
                                result = result + "<td>" + data[i]['status_name'] + "</td>\n";
                                result = result + "<td>" + data[i]['device_name'] + "</td>\n";
                                result = result + "<td>" + data[i]['create_time'] + "</td>\n";
                                result = result + "</tr>\n";
                            }

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
                        dataType: 'json'
                    });
                });
            });

            function getLocation()
            {
                $.ajax({
                    url: "GetLocation.php",
                    data: {},
                    success: function (data) {
                        for (var i = 0; i < data.length; i++) {
                            if (data[i]['enable'] == 1) {
                                $("#selectLocation").append("<option value='" + data[i]['location_id'] + "'>" + data[i]['location_name'] + "</option>");
                            }                            
                        }
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

            function getStatus()
            {
                $.ajax({
                    url: "GetStatus.php",
                    data: {},
                    success: function (data) {
                        for (var i = 0; i < data.length; i++) {
                            if (data[i]['enable'] == 1) {
                                $("#selectStatus").append("<option value='" + data[i]['status_id'] + "'>" + data[i]['status_name'] + "</option>");    
                            }
                        }
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

            function getDevice()
            {
                $.ajax({
                    url: "GetDevice.php",
                    data: {},
                    success: function (data) {
                        for (var i = 0; i < data.length; i++) {
                            if (data[i]['enable'] == 1) {
                                $("#selectDevice").append("<option value='" + data[i]['device_id'] + "'>" + data[i]['device_name'] + "</option>");
                            }
                        }
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
        </script>

    </body>

</html>
