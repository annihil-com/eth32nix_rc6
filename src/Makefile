CC=g++
CFLAGS=-Wall -fPIC -march=i686 -fno-strict-aliasing -fvisibility=hidden
LDFLAGS=-shared -lm
OBJS=  CAimbot.o \
       CCam.o \
       CDebug.o \
       CDetour.o \
       CDrawtools.o \
       CEngine.o \
       CGNetwork.o \
       CGui.o \
       CHook.o \
       CIrc.o \
       CNetwork.o \
       CPortal.o \
       CSettings.o \
       CSyscall.o \
       CTools.o \
       CXCommands.o \
       eth32.o \
       tools/StringTokenizer/StringTokenizer.o \
       gui/CBallisticWindow.o \
       gui/CButtons.o \
       gui/CCameraWindow.o \
       gui/CChatWindow.o \
       gui/CCheckbox.o \
       gui/CClientWindow.o \
       gui/CColorPicker.o \
       gui/CControl.o \
       gui/CEspConfigWindow.o \
       gui/CEspWindow.o \
       gui/CIrcWindow.o \
       gui/CPlayerWindow.o \
       gui/CRadarWindow.o \
       gui/CRespawnWindow.o \
       gui/CScrollBar.o \
       gui/CSliders.o \
       gui/CStatsWindow.o \
       gui/CStatusWindow.o \
       gui/CTextControls.o \
       gui/CUsersWindow.o \
       gui/CWeaponWindow.o \
       gui/CWindow.o \
       gui/CWindowPicker.o
HEADERS = CAimbot.h \
       CCam.h \
       CDebug.h \
       CDetour.h \
       CDraw.h \
       CEngine.h \
       CGNetwork.h \
       CGui.h \
       CHook.h \
       CIrc.h \
       CNetwork.h \
       CPortal.h \
       CSettings.h \
       CSyscall.h \
       CTools.h \
       CXCommands.h \
       eth32.h \
       tools/StringTokenizer/StringTokenizer.h \
       gui/CBallisticWindow.h \
       gui/CButtons.h \
       gui/CCameraWindow.h \
       gui/CChatWindow.h \
       gui/CCheckbox.h \
       gui/CClientWindow.h \
       gui/CColorPicker.h \
       gui/CControl.h \
       gui/CEspConfigWindow.h \
       gui/CEspWindow.h \
       gui/CIrcWindow.h \
       gui/CPlayerWindow.h \
       gui/CRadarWindow.h \
       gui/CRespawnWindow.h \
       gui/CScrollBar.h \
       gui/CSliders.h \
       gui/CStatsWindow.h \
       gui/CStatusWindow.h \
       gui/CTextControls.h \
       gui/CUsersWindow.h \
       gui/CWeaponWindow.h \
       gui/CWindow.h \
       gui/CWindowPicker.h
PROG = eth32.so

# Default
all: $(PROG)

# Compile
%.o: %.c
	$(CC) $(CFLAGS) $(shell pkg-config --cflags libelf 2> /dev/null) -c -o $@ $<

# Link
$(PROG): $(OBJS)
	 $(CC) $(OBJS) -o $(PROG) $(LDFLAGS) $(shell pkg-config --libs libelf 2> /dev/null) -lelf -lbfd -lopcodes

clean:
	rm -rf $(PROG) $(OBJS)
