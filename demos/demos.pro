TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS = \
    fancydemo \
    fancyribbon \
    qtqrcode \
    litedemo \
    qtitanribbon \
    controldemo \
    qtoffice

!macx {
SUBDIRS += licenseDemo
}
