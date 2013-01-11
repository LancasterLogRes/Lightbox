TEMPLATE = subdirs
include (Common.pri)

SUBDIRS = Common Numeric
pi|x86|native: SUBDIRS += EventCompiler ExampleEventCompiler
android|!cross: SUBDIRS += App LGL GUI Mark2

android: QMAKE_EXTRA_TARGETS += deploy
deploy.depends =
deploy.commands = cd Mark2 && make deploy && cd ..
