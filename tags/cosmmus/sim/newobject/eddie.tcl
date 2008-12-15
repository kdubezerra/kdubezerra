# Jae Chung  7-13-99

# Create MyAgent (This will give two warning messages that 
# no default vaules exist for my_var1_otcl and my_var2_otcl)
set eddd [new Eddie]
set ed2 [new Eddie]

# Set configurable parameters of MyAgent
#$eddd set my_var1_otcl 2
#$eddd set my_var2_otcl 3.14

# Give a command to MyAgent
#$eddd call-my-priv-func
#$eddd call-my-priv-func
$eddd call-brother $ed2
$ed2 call-brother $eddd

$ed2 iskedule
