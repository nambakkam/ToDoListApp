# Test suite for MVCPatternExample. Kept separate from the app .pro so the
# application build is untouched. One executable per class: DBManager is a
# singleton that cannot be reset, so every test class needs its own process.
TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS = \
    dbmanager \
    logger \
    eventlogsmodel \
    todonotesmodel \
    todolistmodel
