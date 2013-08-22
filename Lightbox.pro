TEMPLATE = subdirs
include (Common.pri)

llr {
	SUBDIRS += App LGL
	android|native: SUBDIRS += GUI TestGUI
}

pi|x86|native: SUBDIRS += EventCompiler Compute Preprocessors ExampleEventCompiler

SUBDIRS += Numeric Common
