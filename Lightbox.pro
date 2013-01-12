TEMPLATE = subdirs
include (Common.pri)

android|!cross: SUBDIRS += Mark2 App LGL GUI
pi|x86|native: SUBDIRS += EventCompiler ExampleEventCompiler
SUBDIRS += Numeric Common

android: QMAKE_EXTRA_TARGETS += deploy
deploy.depends = Mark2
deploy.commands = cd Mark2 && make deploy && cd ..
