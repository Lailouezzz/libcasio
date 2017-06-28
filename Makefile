#!/usr/bin/make -f
#*****************************************************************************#
# Include variables and message subsystem                                     #
#*****************************************************************************#
include Makefile.vars Makefile.msg

#*****************************************************************************#
# General targets                                                             #
#*****************************************************************************#
# Build everything.
all: all-lib $(if $(INSTALL_MANPAGES),all-doc)

# Mostly clean everything (remove everything but the end results).
mostlyclean: mostlyclean-lib mostlyclean-doc
 mclean: mostlyclean

# Clean everything.
clean: clean-lib clean-doc
 fclean: clean

# To original state.
mrproper: clean
	$(call rmsg,Removing configuration.)
	$(call qcmd,$(RM) Makefile.cfg)
	$(call qcmd,$(RM) -r lib$(NAME)-*)
	$(call qcmd,$(RM) $(INCDIR)/lib$(NAME)/config.h)

# Remake everything (clean and build).
re: clean all

# Install everything.
install: install-lib $(if $(INSTALL_MANPAGES),install-doc)

# Uninstall everything. (experimental)
uninstall: uninstall-lib uninstall-doc

# Reinstall everything. (experimental)
reinstall: uninstall install

# Make a distribution tarball
dist: mrproper
	$(call bcmd,mkdir,lib$(NAME)-$(VERSION),\
		$(MD) .dist)
	$(call bcmd,cp,* lib$(NAME)-$(VERSION),\
		$(CP) -R * .dist)
	$(call qcmd,\
		$(MV) .dist lib$(NAME)-$(VERSION))
	$(call bcmd,tarball,lib$(NAME)-$(VERSION),\
		tar czf lib$(NAME)-$(VERSION).tar.gz \
		--exclude .git lib$(NAME)-$(VERSION))
	$(call qcmd,$(RM) -r lib$(NAME)-$(VERSION))

.PHONY: all mostlyclean mclean clean fclean mrproper re
.PHONY: dist install uninstall reinstall
#*****************************************************************************#
# Configuration (version) checking dependencies                               #
#*****************************************************************************#
# Define the dependencies.
 CHECKCFG := $(if $(shell test -f Makefile.cfg || echo y),check-config, \
	$(if $(shell [ "$(VERSION)" = "$(CONFIG_VERSION)" ] || echo y), \
		check-config-version))

# Define the rules.
 check-config:
	@echo -e "\033[1;31mNo configuration file found!"
	@echo -e "You should configure before re-running this target.\033[0m"
	@false
 check-config-version:
	@echo -e "\033[1;31mConfiguration version is incorrect!"
	@echo -e "You should re-configure before re-running this target.\033[0m"
	@false

.PHONY: check-config check-config-version
#*****************************************************************************#
# Information getting from the Makefile variables                             #
#*****************************************************************************#
# Get the project name.
 getname:
	@echo lib$(NAME)

# Get the project version.
 getversion:
	@echo $(VERSION)

# Get the maintainer.
 getmaintainer:
	@echo "$(MAINTAINER_NAME) <$(MAINTAINER_MAIL)>"

.PHONY: getname getversion getmaintainer
#*****************************************************************************#
# Library-specific targets                                                    #
#*****************************************************************************#
# Make the library.
 all-lib: $(CHECKCFG) $(if $(STATIC),$(ANAME),$(SONAME))

# Make a module object directory.
 $(OBJDIR)/ $(DIRS:%=$(OBJDIR)/%):
	$(call bcmd,mkdir,$@,$(MD) $@)

# Make an object out of a source file.
define make-obj-rule
 $(OBJDIR)/$1.c.o: $(SRCDIR)/$1.c $(INC) $(INCI) | $(dir $(OBJDIR)/$1)
	$(call bcmd,cc,$$@,$(CC) -c -o $$@ $$< $(CFLAGS))
endef
$(foreach src,$(basename $(SRC)),\
$(eval $(call make-obj-rule,$(src))))

# Make the shared library.
 $(SONAME): $(SRC:%=$(OBJDIR)/%.o)
	$(call bcmd,ld,$@,$(LD) -o $@ $^ $(LDFLAGS))

# Make the static library.
 lib$(NAME).a: $(SRC:%=$(OBJDIR)/%.o)
	$(call bcmd,ar rc,$@,$(AR) rcs $@ $^)

# Remove the objects directory.
 mostlyclean-lib:
	$(call rmsg,Removing object directory.)
	$(call qcmd,$(RM) -r $(OBJDIR))
 mclean-lib: mostlyclean-lib

# Clean and remove the built library.
 clean-lib: mclean-lib
	$(call rmsg,Removing the library.)
	$(call qcmd,$(RM) $(SONAMES) $(ANAMES))

# Remake the library.
 re-lib: clean-lib all-lib

# Install the library and development files.
 LINK_TO_MAJOR := $(if $(INSTALL_DEVEL),$(if $(STATIC),,\
	$(if $(FOR_WINDOWS),,y)))
 IWINDLL := $(if $(FOR_WINDOWS),$(if $(STATIC),,y))
 install-lib: all-lib $(if $(INSTALL_DEVEL),install-cfgtool)
	$(call imsg,Installing the library.)
	$(call qcmd,$(INST) -m 755 -d "$(ILIBDIR)")
	$(call qcmd,$(INST) -m 755 -t "$(ILIBDIR)" $(if $(STATIC),\
		$(if $(FOR_WINDOWS),lib$(NAME).lib,lib$(NAME).a),\
		$(if $(FOR_WINDOWS),lib$(NAME).dll.a,$(SONAME))))
	
	$(if $(IWINDLL),$(call qcmd,$(INST) -m 755 -d "$(IBINDIR)"))
	$(if $(IWINDLL),$(call qcmd,$(INST) -m 755 -t "$(IBINDIR)" \
		lib$(NAME).dll))
	
	$(if $(LINK_TO_MAJOR),\
		$(call imsg,Linking lib$(NAME).so to lib$(NAME).so.$(MAJOR).))
	$(if $(LINK_TO_MAJOR),\
		$(call qcmd,$(LN) lib$(NAME).so.$(MAJOR) "$(ILIBDIR)/lib$(NAME).so"))
	
	$(if $(INSTALL_DEVEL),\
		$(call imsg,Installing development files.))
	$(if $(INSTALL_DEVEL),\
		$(call qcmd,$(INST) -m 755 -d $(patsubst %,\
			"$(IINCDIR)/lib$(NAME)-$(VERSION)/%", $(sort $(dir $(INCp))))))
	$(if $(INSTALL_DEVEL),$(foreach i,$(INCp),\
		$(call qcmd,$(INST) -m 644 $(INCDIR)/$(i) \
			"$(IINCDIR)/lib$(NAME)-$(VERSION)/$(i)"$(\n))))

# Uninstall the library and development files. (experimental)
 uninstall-lib: $(CHECKCFG) uninstall-cfgtool
	$(call rmsg,Uninstalling the library.)
	$(call qcmd,$(RM) "$(IBINDIR)/lib$(NAME).dll")
	$(call qcmd,$(RM) "$(ILIBDIR)/lib$(NAME).so"* \
		"$(ILIBDIR)/lib$(NAME).a" "$(ILIBDIR)/lib$(NAME).dll"*)
	$(call rmsg,Uninstalling development files.)
	$(call qcmd,$(RM) -r "$(IINCDIR)/lib$(NAME)-$(VERSION)")
	$(call qcmd,$(RM) "$(IINCDIR)/lib$(NAME).h")
	$(call qcmd,$(RM) -r "$(IINCDIR)/lib$(NAME)")

# Reinstall the library and development files.
 reinstall-lib: uninstall-lib install-lib

.PHONY: all-lib mostlyclean-lib mclean-lib clean-lib re-lib
.PHONY: install-lib uninstall-lib reinstall-lib
#*****************************************************************************#
# Configuration tools-related                                                 #
#*****************************************************************************#
# Install it.
 install-cfgtool: $(CHECKCFG)
	$(call imsg,Installing the configuration tool.)
	$(call qcmd,$(INST) -m 755 -d "$(IBINDIR)")
	$(call qcmd,tools/write-config \
		--name=$(NAME) --version=$(VERSION) --target="$(TARGET)" \
		--maintainer="$(MAINTAINER_NAME) <$(MAINTAINER_MAIL)>" \
		>"$(IBINDIR)/lib$(NAME)-config" \
		&& chmod 755 "$(IBINDIR)/lib$(NAME)-config")
	$(if $(TARGET),$(call qcmd,$(INST) -m 755 -d "$(HBINDIR)"))
	$(if $(TARGET),$(call qcmd,$(LN) -r \
		"$(IBINDIR)/lib$(NAME)-config" \
		"$(HBINDIR)/$(TARGET)lib$(NAME)-config"))
	
	$(call imsg,Installing the pkg-config configuration.)
	$(call qcmd,$(INST) -m 755 -d "$(IPKGDIR)")
	$(call qcmd,tools/write-pkg-config \
		--name=$(NAME) --version=$(VERSION) \
		--deps="$(DEPS)" --deps.private="$(DEPS_PRIV)" \
		--description="$(DESCRIPTION)" \
		--incdir="$(OIINCDIR)/lib$(NAME)-$(VERSION)" --libdir="$(OILIBDIR)" \
		>"$(IPKGDIR)/lib$(NAME).pc" \
		&& chmod 644 "$(IPKGDIR)/lib$(NAME).pc")

# Uninstall it
 uninstall-cfgtool: $(CHECKCFG)
	$(call rmsg,Uninstalling configuration tool and package.)
	$(call qcmd,$(RM) "$(IPKGDIR)/lib$(NAME).pc" \
		"$(IBINDIR)/lib$(NAME)-config" "$(IBINDIR)/"*"-lib$(NAME)-config" \
		$(if $(TARGET),"$(HBINDIR)/$(TARGET)lib$(NAME)-config"))

.PHONY: install-cfgtool uninstall-cfgtool
#*****************************************************************************#
# Documentation-related                                                       #
#*****************************************************************************#
# Make all manpages.
 all-doc: $(foreach s,$(MAN_SECTIONS), $(MAN_$(s):%=$(MANDIR)/man$(s)/%.$(s)))

# Make manpages directories.
 $(MAN_SECTIONS:%=$(MANDIR)/man%):
	$(call bcmd,mkdir,$@,$(MD) $@)

# Make a manpage.
define make-manpage-rule
 $(MANDIR)/man$1/%.$1: $(DOCDIR)/%.$1.txt | $(MANDIR)/man$1
	$(call bcmd,a2x,$$<,$(A2X) -f manpage -D $$| $$< 2>/dev/null)
endef
$(foreach section, $(MAN_SECTIONS), \
$(eval $(call make-manpage-rule,$(section))))

# Mostly clean (do nothing, really)
 mostlyclean-doc:
 mclean-doc: mostlyclean-doc

# Remove all built manpages.
 clean-doc:
	$(call rmsg,Removing manpages directory.)
	$(call qcmd,$(RM) -r $(MANDIR))

# Remake all manpages.
# (I don't really know why some people would want to do that though)
 re-doc: clean-doc all-doc

# Install a manpages section.
define make-installmansection-rule
 install-doc-$1: $(MAN_$1:%=$(MANDIR)/man$1/%.$1)
	$(call imsg,Installing manpages section $1.)
	$(call qcmd,$(INST) -m 755 -d "$(IMANDIR)/man$1")
	$(call qcmd,$(INST) -m 644 -t "$(IMANDIR)/man$1" \
		$(MAN_$1:%=$(MANDIR)/man$1/%.$1))
	$(call qcmd,$(GZIP) $(MAN_$1:%="$(IMANDIR)/man$1/%.$1"))
endef
$(foreach section, $(MAN_SECTIONS), \
$(eval $(call make-installmansection-rule,$(section))))

# Install manpages.
 install-doc: $(CHECKCFG) $(MAN_SECTIONS:%=install-doc-%)

# Clean a manpages section.
define make-uninstall-doc-rule
 uninstall-doc-$1:
	$(call rmsg,Uninstalling manpages section $1.)
	$(call qcmd,$(RM) "$(IMANDIR)/man$1/lib$(NAME).$1"* \
		"$(IMANDIR)/man$1/$(NAME)_"*".$1"* \
		"$(IMANDIR)/man$1/lib$(NAME)-config.$1"*)
endef
$(foreach sec,$(MAN_SECTIONS), \
$(eval $(call make-uninstall-doc-rule,$(sec))))

# Uninstall manpages
 uninstall-doc: $(CHECKCFG) $(MAN_SECTIONS:%=uninstall-doc-%)

.PHONY: all-doc mostlyclean-doc mclean-doc clean-doc re-doc
.PHONY: install-doc uninstall-doc
.PHONY: $(foreach s,$(MAN_SECTIONS),install-doc-$(s) uninstall-doc-$(s))
# End of file
