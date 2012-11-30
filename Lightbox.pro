TEMPLATE = subdirs
system(echo $$CONFIG)
SUBDIRS = Common Numeric
android|!crosscompilation: SUBDIRS += App LGL GUI Glow
