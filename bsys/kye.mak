KYE_C_NAMES:=WNDPROC GLOBALS1 MAIN UNK003 UNK004 UNK005 UNK006 UNK007 EXTRA_DS
KYE_C_SRCS:=$(patsubst %,${SRCDIR}%.CPP,${KYE_C_NAMES})
KYE_C_OBJS:=$(patsubst %,${BUILDDIR}%.OBJ,${KYE_C_NAMES})
KYE_C_HEADERS:=$(wildcard ${SRCDIR}*.H)

# fastbioslogo, startbanner: remove ads at startup
# joysticktype=none: SDL gamepad detection is a notorious source of startup delay, skip it
# output=surface: avoiding OpenGL for the most part helps... but for some reason it seems to initialise an OpenGL window and then close it in favour of a fresh window? still speeds stuff up
# LIBGL_ALWAYS_SOFTWARE=yes: hardware OpenGL is a notoriously source of startup delay, use software OpenGL
# SDL_VIDEODRIVER=dummy: skips making the window entirely
# nomenu: the menu is slightly jank, disable it just in case
# parallel1: this is so we can output onto stdout... also LPTx is way faster than COMx in DOSBox-X
# core, cycles: gotta go fast

DBXFLAGS:=-nogui \
	-nomenu \
	-set fastbioslogo=true \
	-set startbanner=false \
	-set joysticktype=none \
	-set output=surface \
	-set "parallel1=file file:/dev/stdout" \
	-set core=dynamic \
	-set cycles=max \
	#

.PHONY: build-kye
build-kye: ${OUTDIR}KYE.EXE

${BUILDDIR} ${OUTDIR}: | prepare-bcp30af
	install -D -d $@

${OUTDIR}KYE.EXE: ${BUILDDIR}BUILDDOS.BAT | ${OUTDIR} ${BUILDDIR}
	env SDL_VIDEODRIVER=dummy LIBGL_ALWAYS_SOFTWARE=yes dosbox-x ${DBXFLAGS} -c "ctty lpt1" -c "mount d ." -c "D:\\BUILD\\BUILDDOS.BAT" -exit 2>/dev/null
	[ -e $@ ]

${BUILDDIR}BUILDDOS.BAT: ${KYE_C_SRCS} ${KYE_C_HEADERS} $(wildcard ./bsys/prepdos-*.sh) | ${BUILDDIR}
	rm $@ || true
	echo "" > $@.tmp || (rm $@.tmp && false)
	./bsys/prepdos-init.sh >> $@.tmp || (rm $@.tmp && false)
	./bsys/prepdos-obj.sh ${KYE_C_NAMES} >> $@.tmp || (rm $@.tmp && false)
	./bsys/prepdos-exe.sh ${KYE_C_NAMES} >> $@.tmp || (rm $@.tmp && false)
	./bsys/prepdos-final.sh >> $@.tmp || (rm $@.tmp && false)
	mv $@.tmp $@
