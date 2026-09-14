KYE_C_NAMES:=wndproc globals1 main unk003 unk004 unk005 unk005a unk006 unk007 unk008 unk009
KYE_C_SRCS:=$(patsubst %,${SRCDIR}%.c,${KYE_C_NAMES})
KYE_C_OBJS:=$(patsubst %,${BUILDDIR}%.OBJ,${KYE_C_NAMES^^})
KYE_C_HEADERS:=$(wildcard ${SRCDIR}*.h)

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

${BUILDDIR}BUILDDOS.BAT: ${KYE_C_SRCS} ${KYE_C_HEADERS} src/kye.rc src/kye.def $(wildcard ./bsys/prepdos-*.sh) | ${BUILDDIR}
	rm $@ || true
	echo "" > $@.tmp || (rm $@.tmp && false)
	./bsys/prepdos-init.sh >> $@.tmp || (rm $@.tmp && false)
	./bsys/prepdos-obj.sh ${KYE_C_NAMES} >> $@.tmp || (rm $@.tmp && false)
	./bsys/prepdos-exe.sh ${KYE_C_NAMES} >> $@.tmp || (rm $@.tmp && false)
	./bsys/prepdos-final.sh >> $@.tmp || (rm $@.tmp && false)
	mv $@.tmp $@

.PHONY: build-kye-win32
build-kye-win32: ${OUTDIR}kye32.exe ${OUTDIR}kye95.exe
BUILDDIR32::=${BUILDDIR}32/
${BUILDDIR32}:
	install -D -d $@

${OUTDIR}kye32.exe: ${KYE_C_SRCS} ${BUILDDIR32}kye32.res.o ${KYE_C_HEADERS} | ${OUTDIR}
	i386-win32-tcc -o $@ ${KYE_C_SRCS} ${BUILDDIR32}kye32.res.o

${OUTDIR}kye95.exe: ${KYE_C_SRCS} src/compat/libc95.c ${BUILDDIR32}kye32.res.o ${KYE_C_HEADERS} | ${OUTDIR}
	i386-win32-tcc -nostdlib -o $@ ${KYE_C_SRCS} src/compat/libc95.c ${BUILDDIR32}kye32.res.o -lkernel32 -lgdi32 -luser32

${BUILDDIR32}kye32.res.o: ${BUILDDIR32}kye32.res | ${BUILDDIR32}
	i686-w64-mingw32-windres -O coff -o $@ $<

${BUILDDIR32}kye32.res: ${SRCDIR}kye.rc | ${BUILDDIR32}
	zig rc /fo $@ $<
