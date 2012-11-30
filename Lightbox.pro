TEMPLATE = subdirs
system(echo $$CONFIG)
SUBDIRS = Common Numeric
android|x86: SUBDIRS += App LGL GUI Glow
