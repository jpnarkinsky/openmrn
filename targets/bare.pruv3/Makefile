-include ../openmrnpath.mk
include $(OPENMRNPATH)/etc/$(TARGET).mk

# lib here is only needed for clean to work properly. Libraries are copied
# there by the original build rules.
SUBDIRS = lib

# This defines how to create nonexistant directories.
MKSUBDIR_OPENMRNINCLUDE=lib.mk

include $(OPENMRNPATH)/etc/recurse.mk

