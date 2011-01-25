<html>
<?php
include("head.html");
?>

<body>

<div id="wrap">

<?php
include("top.html");
?>

<div style="text-align:justify" class="content">

<h2 class="meuh2">Research</h2><br>

My research interests include distributed systems, specially large-scale distributed simulation of networked virtual environments. More specifically, my research is focused on decentralized network support for massively multiplayer online games, which brings a significant set of problems yet to solve, such as: synchronization and consistency among the many simultaneous players, load balancing when using a multi-server architecture using a geographically distributed system, latency minimization in scenarios where a powerful central server is not available and cheat detection and/or prevention.<br>
<br>
I have been doing research in this area since the last year of my Bachelor course in Computer Science, when I was actively participating of the creation and development of the <i>InGE</i> game library and I was one of the main programmers of the <i>We are the champignons</i> game. My Bachelor thesis was on the topic of QoS for multiplayer games using wireless networks, such as IEEE 802.11. After that, during my Master course, I worked on the P2PSE project, whose objective was to create a peer-to-peer game architecture for massively multiplayer games, which we achieved.<br>
<br>
My Master's thesis had as objective to create a decentralized multi-server architecture, where the server nodes were actually given by volunteers - therefore, presenting very limited resources to serve the game. I had several publication during my Master, whose focus was on saving network bandwidth from the servers, through a fine-tuned interest management scheme, and on balancing the load between the game server at the same time that a minimum inter-server communication was pursued, in order to save resources of the system and to minimize tha communication latency of the players.<br>
<br>
In my PhD course, which is bein done at this moment, my objective is to create a decentralized multi-server architecture composed of volunteer nodes, and adding fault-tolerance to it. In a first moment, a crash-stop failure model is being considered, but the ultimate goal of my thesis is to create a system which is tolerant to byzantine faults - whose purpose is to minimize the cheating issue of decentralized multiplayer game architectures, while also covering faulty, but non-malicious, behaviors of the system.
<br>
<br>
<br>
<p align=center><img src="images/wordclouds/research.jpg" /></p>
<!--p align="center">
<img src="images/underconstruction2.jpg" alt="Under Construction" />
</p-->

</div>

<?php
include("leftside.html");
?>

<div style="clear: both;">
</div>

<?php
include("bottom.html");
?>
</body>
</html>
