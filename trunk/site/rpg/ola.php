<html>
  <head>
    <title>PHP Teste</title>
  </head>
  <body>
    <?php echo "<p>Olah mundo!</p>"; ?>
    <?php echo $_SERVER["HTTP_USER_AGENT"]; ?>
    <?php echo phpinfo(); ?>

<form action="acao.php" method="POST">
 Seu nome <input type="text" name="nome" />
  Sua idade: <input type="text" name="idade" />
   <input type="submit">
   </form>

  </body>
</html>
