<!DOCTYPE html>
<html>
<head>
  <title>Users Logs</title>
<link rel="stylesheet" type="text/css" href="css/userslogs.css">
<script>
  $(window).on("load resize ", function() {
    var scrollWidth = $('.tbl-content').width() - $('.tbl-content table').width();
    $('.tbl-header').css({'padding-right':scrollWidth});
}).resize();
</script>
<script src="https://code.jquery.com/jquery-3.3.1.js"
        integrity="sha1256-2Kok7MbOyxpgUVvAk/HJ2jigOSYS2auK4Pfzbm7uH60="
        crossorigin="anonymous">
</script>
<script src="js/jquery-2.2.3.min.js"></script>
<script src="js/user_log.js"></script>
<script>
  $(document).ready(function(){
      $.ajax({
        url: "user_log_up.php",
        type: 'POST',
        data: {
            'select_date': 1,
        }
      });
    setInterval(function(){
      $.ajax({
        url: "user_log_up.php",
        type: 'POST',
        data: {
            'select_date': 0,
        }
        }).done(function(data) {
          $('#userslog').html(data);
        });
    },5000);
  });
</script>
</head>
<body>
<?php include'header.php'; ?> 
<main>
  <section>
  <!--User table-->
  <h1 class="slideInDown animated">Daily Log</h1>
  	<div class="form-style-5 slideInDown animated">
  		<form method="POST">
  			<input type="date" name="date_sel" id="date_sel">
        <button type="button" name="user_log" id="user_log">Select Date</button>
  		</form>
  	</div>
  <div class="tbl-header slideInRight animated">
    <table cellpadding="0" cellspacing="0" border="0">
      <thead>
        <tr>
          <th>ID</th>
          <th>Name</th>
          <th>Serial Number</th>
          <th>Fingerprint ID</th>
          <th>Date</th>
          <th>Time In</th>
          <th>Time Out</th>
        </tr>
      </thead>
    </table>
  </div>
  <div class="tbl-content slideInRight animated">
    <div id="userslog"></div>
  </div>
</section>
</main>
</body>
</html>