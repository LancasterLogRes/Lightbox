TEMPLATE = subdirs
include (Common.pri)

llr {
	android|native: SUBDIRS += App LGL GUI TestGUI
}

pi|x86|native: SUBDIRS += EventCompiler Compute Preprocessors ExampleEventCompiler

SUBDIRS += Numeric Common
