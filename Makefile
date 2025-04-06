.PHONY: all
all: extract-tools prepare-bcp30af build-kye

SRCDIR:=src/
BUILDDIR:=build/
OUTDIR:=out/

include bsys/diskfiles.mak

ALL_OUT_OSDISKS:=\
	$(EXTRACT_BCP30AF_DISKS) \
	#
ALL_MERGED_OSDISKS:=\
	$(MERGE_BCP30AF_DISKS) \
	$(MERGE_BCP30AF_ARCHIVES) \
	#

.PHONY: extract-tools
extract-tools: $(ALL_OUT_OSDISKS) $(ALL_MERGED_OSDISKS)

include bsys/kye.mak
