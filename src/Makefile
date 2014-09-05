CC      = gcc
PROF    =
NOCRYPT =
#C_FLAGS = -O -Wall $(PROF) $(NOCRYPT)
C_FLAGS = -Wall $(PROF) $(NOCRYPT) -DMERC
L_FLAGS = -O $(PROF)

O_FILES = act_comm.o act_info.o act_move.o act_obj.o act_wiz.o comm.o const.o \
          db.o fight.o handler.o interp.o magic.o save.o special.o update.o \
          clan_table.o kav_wiz.o kav_fight.o kav_info.o clan.o more.o \
          artifact.o home.o rot_wiz.o demon.o werewolf.o olc.o string.o \
          bit.o imc.o imc-interp.o imc-version.o imc-mail.o imc-util.o \
          imc-events.o imc-config.o imc-mercbase.o ice.o icec.o \
          icec-mercbase.o core.o

merc: $(O_FILES)
	rm -f merc
	$(CC) $(L_FLAGS) -o merc $(O_FILES) -lcrypt

.c.o: merc.h
	$(CC) -c $(C_FLAGS) $<
