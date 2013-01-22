TEMPLATE = subdirs
include (Common.pri)

android|!cross: SUBDIRS += App LGL GUI
pi|x86|native: SUBDIRS += EventCompiler ExampleEventCompiler
SUBDIRS += Numeric Common
