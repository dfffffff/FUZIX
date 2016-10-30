$(call find-makefile)

ue.srcs = \
	ue.c term-fuzix.c
$(call build, ue, target-exe)

apps: $(ue.result)

