#!/usr/bin/make -f
include Makefile.vars Makefile.msg

# ---
# General targets.
# ---

# Build everything.

all: all-lib all-utils $(if $(INSTALL_MANPAGES),all-man)

# Mostly clean everything (remove everything but the end results).

mostlyclean:

 mclean: mostlyclean

# Clean everything.

clean:
	$(call rmsg,Removing the build folder.)
	$(call qcmd,$(RM) -r build obj)
	$(call qcmd,$(RM) $(SONAMES) $(ANAMES))

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

install: install-lib install-utils $(if $(INSTALL_MANPAGES),install-man)

# Make a distribution tarball.

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
.PHONY: dist install

# ---
# Configuration (version) checking dependencies.
# ---

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

# ---
# Targets for gathering information from the Makefile variables.
# ---

# Get the project name.

 getname:
	@echo $(NAME)

# Get the project version.

 getversion:
	@echo $(VERSION)

# Check if is indev.

 isindev:
	@$(if $(INDEV),echo $(INDEV),true)

# Get the maintainer.

 getmaintainer:
	@echo "$(MAINTAINER_NAME) <$(MAINTAINER_MAIL)>"

.PHONY: getname getversion isindev getmaintainer

# ---
# Make directories.
# ---

 ./build/ $(L_OBJDIRS) $(U_OBJDIRS) $(M_SECTIONS:%=$(M_MANDIR)/man%/):
	$(call bcmd,mkdir,$@,$(MD) $@)

# ---
# Library specific targets.
# ---

# Make the library.

 all-lib: $(CHECKCFG) $(if $(STATIC),./build/$(L_ANAME),./build/$(L_SONAME))

# Make the shared or static library.

ifeq ($(FOR_WINDOWS),)
 ./build/lib$(LIB).so: ./build/$(L_SONAME)
	$(call bcmd,ln,$@,$(LN) $(L_SONAME) $@)
endif

 ./build/$(L_SONAME): $(L_SRC:%=$(L_OBJDIR)/%.o)
	$(call bcmd,ld,$(L_SONAME),$(LD) -o $@ $^ $(L_LDFLAGS))
 ./build/lib$(L_ANAME).a: $(L_SRC:%=$(L_OBJDIR)/%.o)
	$(call bcmd,ar rc,lib$(L_ANAME).a,$(AR) rcs $@ $^)

# Make an object out of a source file.

define make-obj-rule
 $(L_OBJDIR)/$1.c.o: $(L_SRCDIR)/$1.c $(L_INC) | $(dir $(L_OBJDIR)/$1)
	$(call bcmd,cc,$$@,$(CC) -c -o $$@ $$< $(L_CFLAGS))
endef
$(foreach src,$(basename $(L_SRC)),\
$(eval $(call make-obj-rule,$(src))))

# Install the library and development files.

 LINK_TO_MAJOR := $(if $(INSTALL_DEVEL),$(if $(STATIC),,\
	$(if $(FOR_WINDOWS),,y)))
 IWINDLL := $(if $(FOR_WINDOWS),$(if $(STATIC),,y))

 install-lib-cfgtool:
	$(call imsg,Installing the configuration tool.)
	$(call qcmd,$(INST) -m 755 -d "$(IBINDIR)")
	$(call qcmd,tools/write-config \
		--name=$(LIB) --version=$(VERSION) --target="$(TARGET)" \
		--maintainer="$(MAINTAINER_NAME) <$(MAINTAINER_MAIL)>" \
		>"$(IBINDIR)/lib$(LIB)-config" \
		&& chmod 755 "$(IBINDIR)/lib$(LIB)-config")
	$(if $(TARGET),$(call qcmd,$(INST) -m 755 -d "$(HBINDIR)"))
	$(if $(TARGET),$(call qcmd,$(LN) -r \
		"$(IBINDIR)/lib$(LIB)-config" \
		"$(HBINDIR)/$(TARGET)lib$(LIB)-config"))

 install-lib-pkgconfig:
	$(call imsg,Installing the pkg-config configuration.)
	$(call qcmd,$(INST) -m 755 -d "$(IPKGDIR)")
	$(call qcmd,tools/write-pkg-config \
		--name=$(LIB) --version=$(VERSION) \
		--deps="$(DEPS)" --deps.private="$(DEPS_PRIV)" \
		--description="$(DESCRIPTION)" \
		--incdir="$(OIINCDIR)/lib$(LIB)-$(VERSION)" --libdir="$(OILIBDIR)" \
		>"$(IPKGDIR)/lib$(LIB).pc" \
		&& chmod 644 "$(IPKGDIR)/lib$(LIB).pc")

 install-lib-shared:
	$(call imsg,Installing the library.)
	$(call qcmd,$(INST) -m 755 -d "$(ILIBDIR)")
	$(call qcmd,$(INST) -m 755 -t "$(ILIBDIR)" $(if $(STATIC),\
		./build/$(if $(FOR_WINDOWS),lib$(LIB).lib,lib$(LIB).a),\
		./build/$(if $(FOR_WINDOWS),lib$(LIB).dll.a,$(L_SONAME))))

 install-lib-dll:
	$(call qcmd,$(INST) -m 755 -d "$(IBINDIR)")
	$(call qcmd,$(INST) -m 755 -t "$(IBINDIR)" lib$(LIB).dll)

 install-lib-link:
	$(call imsg,Linking lib$(LIB).so to lib$(LIB).so.$(MAJOR).)
	$(call qcmd,$(LN) lib$(LIB).so.$(MAJOR) "$(ILIBDIR)/lib$(LIB).so")

 install-lib-headers:
	$(call imsg,Installing development files.)
	$(foreach d,$(sort $(dir $(patsubst %,lib$(LIB)-$(VERSION)/%, \
		$(L_INCp)))),$(call qcmd,$(INST) -m 755 -d "$(IINCDIR)/$(d)"$(\n)))
	$(foreach i,$(L_INCp),$(call qcmd,$(INST) -m 644 $(L_INCDIR)/$(i) \
		"$(IINCDIR)/lib$(LIB)-$(VERSION)/$(i)"$(\n)))

 install-lib: $(CHECKCFG) all-lib install-lib-shared \
 	$(if $(LINK_TO_MAJOR),install-lib-link) $(if $(IWINDLL),install-lib-dll) \
 	$(if $(INSTALL_DEVEL),install-lib-cfgtool install-lib-pkgconfig \
 		install-lib-headers)

.PHONY: all-lib install-lib install-lib-shared install-lib-link
.PHONY: install-lib-dll install-lib-cfgtool install-lib-pkgconfig
.PHONY: install-lib-headers

# ---
# Utilities related.
# ---

# Make the utilities.

 all-utils: $(CHECKCFG) $(DEFAULT_UTILS:%=all-%.exe)

# Make a utility.

define make-util-rules
 ./build/$1$(if $(FOR_WINDOWS),.exe): $(U_OBJ_$1) \
	$(if $(filter libcasio,$(U_DEPS_$1)),$(L_AS_DEP)) | ./build/
	$(call bcmd,ld,$$@,$(LD) -o $$@ $(U_OBJ_$1) $(U_LDFLAGS_$1))

 all-$1 all-$1.exe: ./build/$1$(if $(FOR_WINDOWS),.exe)
endef
$(foreach util,$(UTILS),\
$(eval $(call make-util-rules,$(util))))

# Make an object out of a file.

define make-util-obj-rules
ifeq ($(suffix $(U_OBJDIR)/$1/$2),.c)
 $(U_OBJDIR)/$1/$2.o: $(U_SRCDIR)/$1/$2 $(U_INC_$1) \
	| $(dir $(U_OBJDIR)/$1/$2)
	$(call bcmd,cc,$$@,$(CC) -c -o $$@ $$< $(U_CFLAGS_$1))
endif
endef
$(foreach util,$(UTILS),\
$(foreach src,$(U_SRC_$(util)),\
$(eval $(call make-util-obj-rules,$(util),$(src)))))

# Install utils.

 install-utils: $(DEFAULT_UTILS:%=install-%)

# Install a utility.

define make-util-install-rule
 install-$1 install-$1.exe: $(CHECKCFG) all-$1.exe
	$(call imsg,Installing $1$(if $(FOR_WINDOWS),.exe).)
	$(call qcmd,$(INST) -m 755 -d "$(IBINDIR)")
	$(call qcmd,$(INST) -m 755 -t "$(IBINDIR)" \
		./build/$1$(if $(FOR_WINDOWS),.exe))
endef
$(foreach util,$(UTILS),\
$(eval $(call make-util-install-rule,$(util))))

.PHONY: all-utils install-utils $(foreach util,$(UTILS),\
	all-$(util) all-$(util).exe install-$(util) install-$(util).exe)

# ---
# Manpages related.
# ---

# Produce the manpages.

 all-man: $(foreach sec,$(M_SECTIONS),\
	$(M_PAGES_$(sec):%=$(M_MANDIR)/man$(sec)/%.$(sec)))

# Produce one page.

define make-manpage-rules
 $(M_MANDIR)/man$1/$2.$1.gz: $(M_MANDIR)/man$1/$2.$1
	$(call bcmd,gzip,$$<,$(GZIP) $$<)

 $(M_MANDIR)/man$1/$2.$1: $(M_SRCDIR)/$2.$1.txt | $(M_MANDIR)/man$1/
	$(call bcmd,a2x,$$<,$(A2X) -f manpage -D $$| $$< 2>/dev/null)
endef
$(foreach sec,$(M_SECTIONS),\
$(foreach pg,$(M_PAGES_$(sec)),\
$(eval $(call make-manpage-rules,$(sec),$(pg)))))

# Install the manpages.

 install-man: $(CHECKCFG) all-man
	$(call imsg,Installing manpages.)
	$(foreach s,$(M_SECTIONS),$(call qcmd,$(INST) -m 755 \
		-d "$(IMANDIR)/man$(s)"$(\n)))
	$(foreach s,$(M_SECTIONS),$(foreach p,$(M_PAGES_$(s)),\
		$(call qcmd,$(INST) -m 644 $(M_MANDIR)/man$(s)/$(p).$(s) \
			"$(IMANDIR)/man$(s)/$(p).$(s)" && \
			$(GZIP) "$(IMANDIR)/man$(s)/$(p).$(s)"$(\n))))

.PHONY: all-man install-man

# ---
# Documentation-related targets.
# ---

html:
	make -C docs

.PHONY: html

# End of file
