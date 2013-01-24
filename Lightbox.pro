TEMPLATE = subdirs
include (Common.pri)

android|native: SUBDIRS += App LGL GUI TestGUI
pi|x86|native: SUBDIRS += EventCompiler ExampleEventCompiler
SUBDIRS += Numeric Common
