DAEMON_VERSION=$(shell cat VERSION)

CC=g++
PKGCONFIG=`pkg-config --cflags --libs liblog4cxx dbus-c++-1 gtkmm-3.0`
CFLAGS=-pedantic -Wall -W -g $(PKGCONFIG) -DDOUANE_VERSION=\"$(DAEMON_VERSION)\"
LDFLAGS=$(PKGCONFIG) -lboost_signals -lboost_system -lboost_filesystem -lboost_regex -lcrypto

OBJ=freedesktop/desktop_file.o \
	freedesktop/desktop_files.o \
	gtk/gtk_application_icon_sublimer.o \
	gtk/gtk_box_unknown_application.o \
	gtk/gtk_process_icon.o \
	gtk/gtk_question_window.o \
	dbus_server.o \
	douane.o \
	ip_address_translator.o \
	main.o \
	netlink_listener.o \
	netlink_message_handler.o \
	netlink_socket.o \
	network_activity.o \
	process.o \
	processes_manager.o \
	rule.o \
	rules_manager.o \
	socket.o \
	thread.o \
	tools.o

INSTALL=/usr/bin/install -c
BINDIR=$(DESTDIR)/opt/douane
DATADIR=$(BINDIR)/data
PIDSDIR=$(DESTDIR)/opt/douane/pids
EXEC=douaned
EXECICON=data/douane_128.png

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) -o $(EXEC) $(OBJ) $(LDFLAGS)

%.o: %.cpp
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm -rf $(OBJ) $(EXEC)

dbus:
	dbusxx-xml2cpp d-bus/org.zedroot.douane.xml --adaptor=org_zedroot_douane.h

install: $(EXEC)
	test -d $(BINDIR) || mkdir -p $(BINDIR)
	test -d $(DATADIR) || mkdir -p $(DATADIR)
	test -d $(DESTDIR)/etc/init.d/ || mkdir -p $(DESTDIR)/etc/init.d/
	test -d $(DESTDIR)/etc/dbus-1/system.d/ || mkdir -p $(DESTDIR)/etc/dbus-1/system.d/
	test -d $(DESTDIR)/opt/douane/data/ || mkdir -p $(DESTDIR)/opt/douane/data/
	test -d $(DESTDIR)/usr/share/icons/hicolor/128x128/apps || mkdir -p $(DESTDIR)/usr/share/icons/hicolor/128x128/apps
	install -m 0500 $(EXEC) $(BINDIR)
	install -m 0500 $(EXECICON) $(DATADIR)
	install -m 0755 init.d/douane $(DESTDIR)/etc/init.d/
	install -m 0644 system.d/douane.conf $(DESTDIR)/etc/dbus-1/system.d/
	install -m 0500 data/douane_128.png $(DESTDIR)/opt/douane/data/
	install -m 0500 data/douane_128.png $(DESTDIR)/usr/share/icons/hicolor/128x128/apps
