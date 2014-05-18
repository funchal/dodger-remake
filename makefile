#!/usr/bin/make -f

.SUFFIXES:
.SECONDARY:
SHELL := /bin/sh

makefile := $(MAKEFILE_LIST)
rm := rm -f
exists := test -e
originalgame := dodger-1.1/dodger.exe
srcdir := src
imgdir := img
icodir := ico
snddir := snd
objdir := obj
resdirs := $(imgdir) $(icodir) $(snddir) $(objdir)
toupper = $(shell echo $(1) | tr '[:lower:]' '[:upper:]')

SFML := ../SFML

cxx := g++
cppflags := \
	-Wall \
	-Wextra \
	-Wshadow \
	-Wmissing-declarations \
	-MMD \
	-MP \
	-I $(SFML)/include

cxxflags := \
	-g \
	-fmessage-length=0 \
	-O0

ld := g++
ldflags := \
	-rdynamic \
	--enable-new-dtags \
	-Wl,-rpath $(SFML)/lib \
	-L $(SFML)/lib \
	-lsfml-window \
	-lsfml-graphics \
	-lsfml-system \
	-lsfml-audio

target := dodger-remake

objs := \
	$(addprefix $(objdir)/, \
		dodger.o \
		level.o \
	)

resources := \
	$(addprefix $(icodir)/, \
		dodger.ico \
		dodger.bmp \
	) \
	$(addprefix $(imgdir)/, \
		apple.bmp \
		blank.bmp \
		c.bmp \
		cherry.bmp \
		colon.bmp \
		dead3.bmp \
		dead4.bmp \
		dead5.bmp \
		dead6.bmp \
		gmandc.bmp \
		gmando.bmp \
		gmanlc.bmp \
		gmanlo.bmp \
		gmanrc.bmp \
		gmanro.bmp \
		gmanuc.bmp \
		gmanuo.bmp \
		grid.bmp \
		h.bmp \
		i.bmp \
		laserd.bmp \
		laserl.bmp \
		laserr.bmp \
		laseru.bmp \
		l.bmp \
		lives.bmp \
		n0.bmp \
		n1.bmp \
		n2.bmp \
		n3.bmp \
		n4.bmp \
		n5.bmp \
		n6.bmp \
		n7.bmp \
		n8.bmp \
		n9.bmp \
		sberry.bmp \
		s.bmp \
		shotah.bmp \
		shotav.bmp \
		shotbh.bmp \
		shotbv.bmp \
		shotch.bmp \
		shotcv.bmp \
		shotdh.bmp \
		shotdv.bmp \
		shotgh.bmp \
		shotgv.bmp \
		shotsh.bmp \
		shotsv.bmp \
		shotwh.bmp \
		shotwv.bmp \
		skull.bmp \
		v.bmp \
		wall.bmp \
	) \
	$(addprefix $(snddir)/, \
		chomp.wav \
		dead.wav \
		gover.wav \
		wdone.wav \
	)

.PHONY: check
check: all
	./$(target)

.PHONY: all
all: resources $(target)

.PHONY: resources
resources: $(resources)

.PHONY: clean
clean:
	$(rm) -R $(resdirs)

$(target): $(objs)
	$(ld) $^ -o $@ $(cxxflags) $(ldflags)

$(objdir)/%.o: $(srcdir)/%.cpp $(makefile) | $(objdir)
	$(cxx)  -c $< -o $@ $(cppflags) $(cxxflags)

$(imgdir)/%.bmp: $(originalgame) $(makefile) | $(imgdir)
	$(rm) $@
	wrestool -x $< -t bitmap -n $(call toupper, $*)L -o $@
	$(exists) $@

$(icodir)/%.ico: $(originalgame) $(makefile) | $(icodir)
	$(rm) $@
	wrestool -x $< -t group_icon -n $(call toupper, $*) -o $@
	$(exists) $@

$(icodir)/%.bmp: $(icodir)/%.ico
	convert $< BMP3:$@

$(snddir)/%.wav: $(originalgame) $(makefile) | $(snddir)
	$(rm) $@
	wrestool -x $< -t WAVE -R -n $(call toupper, $*) -o $@
	$(exists) $@

$(resdirs):
	mkdir -p $@

-include $(objs:%.o=%.d)
