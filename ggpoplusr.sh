##
## Place this in the same directory as the +R exe and change your launch settings in Steam to
## ./ggpoplusr.sh %command%
##

#set this yourself
GGPOPLUSR_DIR=

export GGPOPLUSR_LINUX_DIR="$PWD"
cd "$GGPOPLUSR_DIR"
"$1" waitforexitandrun Launcher.exe
