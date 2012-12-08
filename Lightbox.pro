TEMPLATE = subdirs
system(echo $$CONFIG)
SUBDIRS = Common Numeric
android|!cross: SUBDIRS += App LGL GUI Mark2
