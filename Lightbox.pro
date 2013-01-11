TEMPLATE = subdirs
include (Common.pri)
SUBDIRS = Common Numeric
pi|x86|native: SUBDIRS += EventCompiler ExampleEventCompiler
android|!cross: SUBDIRS += App LGL GUI Mark2
