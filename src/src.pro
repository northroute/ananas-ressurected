TEMPLATE = subdirs

SUBDIRS += lib
SUBDIRS += plugins
SUBDIRS += designer
SUBDIRS += ananas
SUBDIRS += admin
SUBDIRS += extensions

# Dependencies
plugins.depends = lib

designer.depends = lib plugins
ananas.depends   = lib plugins
admin.depends    = lib plugins designer

extensions.depends = lib plugins

#TRANSLATIONS = \
#    ../translations/ananas-en.ts \
#    ../translations/ananas-ru.ts 
