#!/usr/bin/make -f
#*****************************************************************************#
# Target for the end user                                                     #
#*****************************************************************************#
# Make it.
all: -all

# Preview in local how it will render.
preview prev: -all-watch

# Upload it to production (only for maintainers).
show: -all -install

.PHONY: preview prev
#*****************************************************************************#
# Internal                                                                    #
#*****************************************************************************#
 JEK := bundle exec jekyll
 ROOT := cake@p7.planet-casio.com:libcasio_doc

# Prepare the bundle.
 -prepare:
	bundle check || bundle install --path vendor/bundle

# Make it all.
 -all: -prepare
	$(JEK) build $(JEKYLL_OPT)

# Make and watch.
 -all-watch: -prepare
	$(JEK) serve --watch $(JEKYLL_OPT)

# Upload.
 -install:
	find _site -type f -exec chmod 644 {} \;
	rsync -Prlt --delete _site/ "$(ROOT)"

.PHONY: -prepare -all -all-watch -install
# End of file.
