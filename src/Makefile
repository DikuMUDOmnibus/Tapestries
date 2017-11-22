CC      = gcc
PROF    = -g
NOCRYPT =
C_FLAGS = -O -static -Wall $(PROF) $(NOCRYPT)
L_FLAGS = -O -static $(PROF)

O_FILES = act_comm.o act_info.o act_move.o act_obj.o act_olc.o \
	  act_wiz.o bit.o comm.o const.o db.o fight.o handler.o \
	  interp.o magic.o mem.o mob_commands.o mob_prog.o obj.o \
	  olc.o olc_save.o save.o special.o string.o update.o

summit: $(O_FILES) ident
	rm -f tapestries
	$(CC) $(L_FLAGS) -o tapestries $(O_FILES)

ident: getnameauth.o
	rm -f ../area/getnameauth
	$(CC) $(L_FLAGS) -o getnameauth getnameauth.o
	mv getnameauth ../area/getnameauth

.c.o: merc.h
	$(CC) -c $(C_FLAGS) $<
