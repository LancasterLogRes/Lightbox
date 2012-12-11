include (Common.pri)
TEMPLATE = subdirs
SUBDIRS = Common Numeric
android|!cross: SUBDIRS += App LGL GUI Mark2
