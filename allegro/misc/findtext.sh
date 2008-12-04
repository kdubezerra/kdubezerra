#! /bin/sh
#
#  Shell script to scan the source and find any text strings that need
#  to be translated. It creates an entext.cfg file containing the
#  English version, and if given a commandline argument, will then
#  check the named file (which should be a translation into some other
#  language) to see if it is missing any of the strings.


echo "Scanning for translation strings..."

if [ -e resource/language/entext.cfg ]; then
   cat resource/language/entext.cfg > entext.cfg
else
   echo "language_name = English" > entext.cfg
   echo >> entext.cfg
   echo "[language]" >> entext.cfg
fi

grep get_config_text src/*.c src/*/*.c | \
   sed -n -e "s/.*get_config_text *(\"\(.*\)\").*/\1/p" \
	  -e "/ascii_name/d" -e "/gfx_mode_data/d" \
	  -e "/depth/d" -e "/char \*/d" \
	  -e "s/\(.*\):.*get_config_text *(\([^\"].*\)).*/\1 generates \2/p" | \
      \
   sed -e "p" -e "s/[ =#]/_/g" | \
   sed -e "N" -e "s/\(.*\)\n\(.*\)/\2 = \1/" | \
   sed -e "s/.* = \([a-zA-Z0-9_\/]*\.c\) generates \(.*\)/warning: \1 uses generated string \2/" | \
   sort -d | \
   uniq >> entext.cfg

if [ $# -gt 0 ]; then
   echo "Comparing entext.cfg with $1..."
   sed -e "s/=.*//" -e "s/ //" entext.cfg $1 | \
      sort | \
      uniq -u
else
   echo "Translation strings written into entext.cfg"
fi