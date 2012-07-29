<?PHP
$fileName="../server/stats";	// File with statistics
?>

<html>
    <head>
        <title>SeaCraft</title>
        <link rel="stylesheet" type="text/css" href="style.css">
    </head>
    <body>
        <div id="content">
            <div id="left"></div>
            <div id="center">
                <div id="logo"></div>
                <div id="document">
                    <p><b>SeaCraft</b> - simple sea battle game.<br/>Download, play, program your own AI and get to the top of the score!</p>
                    <p><a href="https://github.com/bakwc/SeaCraft/downloads">Download</a> client binary release or get the latest sources from <a href="https://github.com/bakwc/SeaCraft">github</a>.<br/>
                    Information about network protocol is <a href="https://github.com/bakwc/SeaCraft/blob/master/PROTOCOL.txt">here</a>.
                    <h2>Screenshots<h2>
                    <a href="images/screen.png" target="_blank"><img width=320 src="images/screen.png"></img></a>
                    <h2>Top Score</h2>
                    <table>
						<thead>
							<tr>
								<td>Player</td><td>Wins</td><td>Loses</td>
							</tr>
						</thead>
						<tbody>
							<?
							$f = fopen($fileName,'r');
							while (!feof($f))
							{
								$line = fgets($f, 999);
								$arr = explode (":", $line);
								if (count($arr)==4)
								{
									echo "<tr><td>".$arr[0].
										"</td><td>".$arr[1].
										"</td><td>".$arr[2].
										"</td></tr>";
								}
							}
							fclose($f);
							?>
						</tbody>
					</table>
                </div>
            </div>
            <div id="right"></div>
        </div>
    </body>
</html>
