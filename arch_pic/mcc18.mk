CC           = '$(MCCPATH)/bin/mcc18'
LD           = '$(MCCPATH)/bin/mplink'
AS           = '$(MCCPATH)/mpasm/mpasm'
MD           = gcc

SRC += $(ARCHDIR)/c018iz_mcc18.c \
	  $(ARCHDIR)/ifi_util_mcc18.asm

MCCPATH      = /opt/mcc18
WINPATH      = $(srcdir)/$(ARCHDIR)/winpath.sh
IPATH        = '$(MCCPATH)/h'
ICPATH       = '$(srcdir)/$(ARCHDIR)/h_mcc18'
IAPATH       = '$(srcdir)/$(ARCHDIR)'
WIPATH      := '$(shell $(WINPATH) $(IPATH))'
WICPATH     := '$(shell $(WINPATH) $(ICPATH))'
WIAPATH     := '$(shell $(WINPATH) $(IAPATH))'
LIBPATH      = '$(MCCPATH)/lib'
WLIBPATH    := '$(shell $(WINPATH) $(LIBPATH))'

ARCH_CFLAGS  = -I=$(WICPATH) -I=$(WIPATH) -I=$(srcdir) -I=$(WIAPATH)   \
               -p=18F8520 /DARCH_PIC
ARCH_ASFLAGS = /p18f8520
ARCH_LDFLAGS = $(ARCHDIR)/18f8520user_mcc18.lkr /l $(WLIBPATH) /a INHX32

OBJ     += $(SRC:=.o)
TRASH       += $(TARGET:.hex=.cod) \
               $(TARGET:.hex=.lst) \
               $(OBJ:.o=.err)  \
               $(OBJ:.o=.d)

.PHONY : clean
.SECONDARY:
clean :
	@echo "CLEAN"
	@$(RM) $(OBJ) $(TARGET) $(TRASH)

%.hex : $(OBJ)
	@echo "LD $(@F)"
	@$(LD) $(ALL_LDFLAGS) $^ /o$@ /m$(@:.hex=.map)

#-include $(OBJ:.o=.d)

%.c.o : %.c
	@echo "CC $(@F)"
	@$(CC) $(ALL_CFLAGS) $< -fo=$@

%.asm.o : %.asm
	@echo "AS $(@F)"
	@$(AS) /q $(ALL_ASFLAGS) $< /o$@

