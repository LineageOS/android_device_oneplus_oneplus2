#!/system/bin/sh

# $1: wave file to read
# $2: volume(0-15)
# $3: device for output
#     0: current
#     1: speaker
#    12: earpiece
#    -1: raw speaker
#    -2: raw earpiece
#    -3: headphone-48khz-16bit

# tinyplay file.wav [-D card] [-d device] [-p period_size] [-n n_periods]
# sample usage: playback_audio.sh 2000.wav  15 -1

function enable_speaker {
	echo "enabling speaker"
	tinymix 'QUAT_MI2S_RX Audio Mixer MultiMedia1' 1
	tinymix 'left Profile' 'music'
}

function disable_speaker {
	echo "disabling speaker"
	tinymix 'QUAT_MI2S_RX Audio Mixer MultiMedia1' 0
}

echo "Volume is ignored by this script for now"

if [ "$3" -eq "1" -o "$3" -eq "-1" ]; then
	enable_speaker
fi

tinyplay $1

if [ "$3" -eq "1" -o "$3" -eq "-1" ]; then
	disable_speaker
fi

exit 0
