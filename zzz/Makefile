UK_ROOT ?= $(PWD)/../../unikraft
UK_LIBS ?= $(PWD)/../../libs
LIBS :=  $(UK_LIBS)/pthread-embedded:$(UK_LIBS)/glib:$(UK_LIBS)/newlib:$(UK_LIBS)/lwip:$(UK_LIBS)/zzz:$(UK_LIBS)/jsonc:$(UK_LIBS)/xentools:$(UK_LIBS)/vmi
#LIBS :=  $(UK_LIBS)/newlib:$(UK_LIBS)/zzz:$(UK_LIBS)/jsonc:$(UK_LIBS)/glib
#:$(UK_LIBS)/pthread-embedded


all:
	@$(MAKE) -C $(UK_ROOT) A=$(PWD) L=$(LIBS)

$(MAKECMDGOALS):
	@$(MAKE) -C $(UK_ROOT) A=$(PWD) L=$(LIBS) $(MAKECMDGOALS)

