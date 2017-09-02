#!/usr/bin/make -f
#*****************************************************************************#
# Target for the end user                                                     #
#*****************************************************************************#
# Preview in local how it will render.
preview prev: -all-watch

.PHONY: preview prev
#*****************************************************************************#
# Internal                                                                    #
#*****************************************************************************#
 JEK := bundle exec jekyll

# Prepare the bundle.
 -prepare:
	bundle check || bundle install --path vendor/bundle

# Make it all.
 -all: -prepare
	$(JEK) build $(JEKYLL_OPT)

# Make and watch.
 -all-watch: -prepare
	$(JEK) serve --watch $(JEKYLL_OPT)

.PHONY: -prepare -all -all-watch
# End of file.
