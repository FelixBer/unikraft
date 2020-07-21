UK_ROOT ?= $(PWD)/../../unikraft
UK_LIBS ?= $(PWD)/../../libs
LIBS :=  $(UK_LIBS)/pthread-embedded:$(UK_LIBS)/glib:$(UK_LIBS)/newlib:$(UK_LIBS)/zzz:$(UK_LIBS)/jsonc:$(UK_LIBS)/vmi:$(UK_LIBS)/xentools
#LIBS :=  $(UK_LIBS)/newlib:$(UK_LIBS)/zzz:$(UK_LIBS)/jsonc:$(UK_LIBS)/glib
#:$(UK_LIBS)/pthread-embedded


all:
	@$(MAKE) -C $(UK_ROOT) A=$(PWD) L=$(LIBS)

$(MAKECMDGOALS):
	@$(MAKE) -C $(UK_ROOT) A=$(PWD) L=$(LIBS) $(MAKECMDGOALS)

